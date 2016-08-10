#include "config.h"
#include "game.h"
#include "image_manager.h"
#include "data_manager.h"
#include "deck_manager.h"
#include "replay.h"
#include "materials.h"
#include "duelclient.h"
#include "netserver.h"
#include "single_mode.h"
#include <sstream>

#ifdef _WIN32
#include <io.h>
#else
#include <sys/types.h>
#include <dirent.h>
#endif

const unsigned short PRO_VERSION = 0x1339;

namespace ygo {

Game* mainGame;

bool Game::Initialize() {
	srand(time(0));
	LoadConfig();
	irr::SIrrlichtCreationParameters params = irr::SIrrlichtCreationParameters();
	params.AntiAlias = gameConf.antialias;
	if(gameConf.use_d3d)
		params.DriverType = irr::video::EDT_DIRECT3D9;
	else
		params.DriverType = irr::video::EDT_OPENGL;
#ifdef _DEBUG
	params.DriverType = irr::video::EDT_DIRECT3D9;
#endif // DEBUG
	params.WindowSize = irr::core::dimension2d<u32>(1024, 640);
	device = irr::createDeviceEx(params);
	if(!device)
		return false;

	// Apply skin
	unsigned int special_color = 0xFF0000FF;
	turncolor = 0x8000ffff;
	playerlpcolor = 0xffffff00;
	extracolor = 0xffffff00;
	statcolor = 0xffffffff;
	bonuscolor = 0xffffff00;
	negativecolor = 0xffff2090;
	setcolor = 0xff0000ff;

	if (gameConf.skin_index >= 0)
	{
		skinSystem = new CGUISkinSystem("skins", device);
		core::array<core::stringw> skins = skinSystem->listSkins();
		if ((size_t)gameConf.skin_index < skins.size())
		{
			int index = skins.size() - gameConf.skin_index - 1; // reverse index
			if (skinSystem->applySkin(skins[index].c_str()))
			{
				special_color = ExtractColor(L"SpecialColor", skinSystem, special_color);
				turncolor = ExtractColor(L"TurnColor", skinSystem, turncolor);
				playerlpcolor = ExtractColor(L"LPColor", skinSystem, playerlpcolor);
				extracolor = ExtractColor(L"ExtraColor", skinSystem, extracolor);
				statcolor = ExtractColor(L"StatColor", skinSystem, statcolor);
				bonuscolor = ExtractColor(L"BonusColor", skinSystem, bonuscolor);
				negativecolor = ExtractColor(L"NegativeColor", skinSystem, negativecolor);
				setcolor = ExtractColor(L"SetColor", skinSystem, setcolor);
			}
		}
	}

	linePattern = 0x0f0f;
	waitFrame = 0;
	signalFrame = 0;
	showcard = 0;
	is_attacking = false;
	lpframe = 0;
	lpcstring = 0;
	always_chain = false;
	ignore_chain = false;
	is_building = false;
	memset(&dInfo, 0, sizeof(DuelInfo));
	memset(chatTiming, 0, sizeof(chatTiming));
	driver = device->getVideoDriver();
	driver->setTextureCreationFlag(irr::video::ETCF_CREATE_MIP_MAPS, false);
	driver->setTextureCreationFlag(irr::video::ETCF_OPTIMIZED_FOR_QUALITY, true);
	imageManager.SetDevice(device);
	if(!imageManager.Initial())
		return false;
	if(!dataManager.LoadDB("cards.cdb"))
		return false;
	deckManager.LoadLFList();
	if(!dataManager.LoadStrings("strings.conf"))
		return false; 
#ifdef _WIN32
	char fpath[1000];
	WIN32_FIND_DATAW fdataw;
	HANDLE fh = FindFirstFileW(L"./expansions/*.cdb", &fdataw);
	if (fh != INVALID_HANDLE_VALUE) {
		do {
			if (!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				sprintf(fpath, "./expansions/%ls", fdataw.cFileName);
				dataManager.LoadDB(fpath);
			}
		} while (FindNextFileW(fh, &fdataw));
		FindClose(fh);
	}
#else
	DIR * dir;
	struct dirent * dirp;
	const char *foldername = "./expansions/";
	if ((dir = opendir(foldername)) != NULL) {
		while ((dirp = readdir(dir)) != NULL) {
			size_t len = strlen(dirp->d_name);
			if (len < 5 || strcasecmp(dirp->d_name + len - 4, ".cdb") != 0)
				+ continue;
			char *filepath = (char *)malloc(sizeof(char)*(len + strlen(foldername)));
			strncpy(filepath, foldername, strlen(foldername) + 1);
			strncat(filepath, dirp->d_name, len);
			std::cout << "Found file " << filepath << std::endl;
			if (!dataManager.LoadDB(filepath))
					std::cout << "Error loading file" << std::endl;
			free(filepath);
			
		}
		closedir(dir);
#endif
	env = device->getGUIEnvironment();
	numFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.numfont, 16);
	adFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.numfont, 12);
	lpcFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.numfont, 48);
	guiFont = irr::gui::CGUITTFont::createTTFont(env, gameConf.textfont, gameConf.textfontsize);
	textFont = guiFont;
	smgr = device->getSceneManager();
	device->setWindowCaption(L"YGOPro DevPro");
	device->setResizable(true);
	wchar_t strbuf[256];
	//main menu
	wMenu.Load();
	//online mode
	wOnline.Load();
	//lan mode
	wLan.Load();
	//create host
	wHost.Load();
	//host(single)
	wHostPrepare = env->addWindow(rect<s32>(270, 120, 750+50, 440), false, dataManager.GetSysString(1250));
	wHostPrepare->getCloseButton()->setVisible(false);
	wHostPrepare->setVisible(false);
	btnHostPrepDuelist = env->addButton(rect<s32>(10, 30, 110, 55), wHostPrepare, BUTTON_HP_DUELIST, dataManager.GetSysString(1251));
	for(int i = 0; i < 2; ++i) {
		stHostPrepDuelist[i] = env->addStaticText(L"", rect<s32>(40, 65 + i * 25, 240, 85 + i * 25), true, false, wHostPrepare);
		stHostPrepDuelistElo[i] = env->addStaticText(L"", rect<s32>(250, 65 + i * 25, 240+50, 85 + i * 25), true, false, wHostPrepare);
		btnHostPrepKick[i] = env->addButton(rect<s32>(10, 65 + i * 25, 30, 85 + i * 25), wHostPrepare, BUTTON_HP_KICK, L"X");
		chkHostPrepReady[i] = env->addCheckBox(false, rect<s32>(250+50, 65 + i * 25, 270+50, 85 + i * 25), wHostPrepare, CHECKBOX_HP_READY, L"");
		chkHostPrepReady[i]->setEnabled(false);
	}
	for(int i = 2; i < 4; ++i) {
		stHostPrepDuelist[i] = env->addStaticText(L"", rect<s32>(40, 75 + i * 25, 240, 95 + i * 25), true, false, wHostPrepare);
		stHostPrepDuelistElo[i] = env->addStaticText(L"", rect<s32>(250, 65 + i * 25, 240+50, 95 + i * 25), true, false, wHostPrepare);
		btnHostPrepKick[i] = env->addButton(rect<s32>(10, 75 + i * 25, 30, 95 + i * 25), wHostPrepare, BUTTON_HP_KICK, L"X");
		chkHostPrepReady[i] = env->addCheckBox(false, rect<s32>(250+50, 75 + i * 25, 270+50, 95 + i * 25), wHostPrepare, CHECKBOX_HP_READY, L"");
		chkHostPrepReady[i]->setEnabled(false);
	}
	btnHostPrepOB = env->addButton(rect<s32>(10, 180, 110, 205), wHostPrepare, BUTTON_HP_OBSERVER, dataManager.GetSysString(1252));
	myswprintf(dataManager.strBuffer, L"%ls%d", dataManager.GetSysString(1253), 0);
	stHostPrepOB = env->addStaticText(dataManager.strBuffer, rect<s32>(10, 210, 270, 230), false, false, wHostPrepare);
	stHostPrepRule = env->addStaticText(L"", rect<s32>(280+50, 30, 460+50, 230), false, true, wHostPrepare);
	env->addStaticText(dataManager.GetSysString(1254), rect<s32>(10, 235, 110, 255), false, false, wHostPrepare);
	cbDeckSelect = env->addComboBox(rect<s32>(120, 230, 270, 255), wHostPrepare);
	btnHostPrepStart = env->addButton(rect<s32>(230+50, 280, 340+50, 305), wHostPrepare, BUTTON_HP_START, dataManager.GetSysString(1215));
	btnHostPrepCancel = env->addButton(rect<s32>(350+50, 280, 460+50, 305), wHostPrepare, BUTTON_HP_CANCEL, dataManager.GetSysString(1212));
	//img
	wCardImg = env->addStaticText(L"", rect<s32>(1, 1, 199, 273), true, false, 0, -1, true);
	wCardImg->setBackgroundColor(0xc0c0c0c0);
	wCardImg->setVisible(false);
	imgCard = env->addImage(rect<s32>(10, 9, 187, 263), wCardImg);
	imgCard->setUseAlphaChannel(true);
	//phase
	wPhase = env->addStaticText(L"", rect<s32>(480, 310, 855, 330));
	wPhase->setVisible(false);
	btnDP = env->addButton(rect<s32>(0, 0, 50, 20), wPhase, -1, L"\xff24\xff30");
	btnDP->setEnabled(false);
	btnDP->setPressed(true);
	btnDP->setVisible(false);
	btnSP = env->addButton(rect<s32>(65, 0, 115, 20), wPhase, -1, L"\xff33\xff30");
	btnSP->setEnabled(false);
	btnSP->setPressed(true);
	btnSP->setVisible(false);
	btnM1 = env->addButton(rect<s32>(130, 0, 180, 20), wPhase, -1, L"\xff2d\xff11");
	btnM1->setEnabled(false);
	btnM1->setPressed(true);
	btnM1->setVisible(false);
	btnBP = env->addButton(rect<s32>(195, 0, 245, 20), wPhase, BUTTON_BP, L"\xff22\xff30");
	btnBP->setVisible(false);
	btnM2 = env->addButton(rect<s32>(260, 0, 310, 20), wPhase, BUTTON_M2, L"\xff2d\xff12");
	btnM2->setVisible(false);
	btnEP = env->addButton(rect<s32>(325, 0, 375, 20), wPhase, BUTTON_EP, L"\xff25\xff30");
	btnEP->setVisible(false);
	btnShuffle = env->addButton(rect<s32>(0, 0, 50, 20), wPhase, BUTTON_CMD_SHUFFLE, dataManager.GetSysString(1307));
	btnShuffle->setVisible(false);
	//chain buttons
	btnChainIgnore = env->addButton(rect<s32>(205, 100, 295, 135), 0, BUTTON_CHAIN_IGNORE, dataManager.GetSysString(1292));
	btnChainAlways = env->addButton(rect<s32>(205, 140, 295, 175), 0, BUTTON_CHAIN_ALWAYS, dataManager.GetSysString(1293));
	btnChainWhenAvail = env->addButton(rect<s32>(205, 180, 295, 215), 0, BUTTON_CHAIN_WHENAVAIL, dataManager.GetSysString(1294));
	btnChainIgnore->setIsPushButton(false);
	btnChainAlways->setIsPushButton(false);
	btnChainWhenAvail->setIsPushButton(true);
	btnChainIgnore->setVisible(false);
	btnChainAlways->setVisible(false);
	btnChainWhenAvail->setVisible(false);
	chain_when_avail = true;
	//tab
	wInfos = env->addTabControl(rect<s32>(1, 275, 301, 639), 0, true);
	wInfos->setVisible(false);
	//info
	irr::gui::IGUITab* tabInfo = wInfos->addTab(dataManager.GetSysString(1270));
	stName = env->addStaticText(L"", rect<s32>(10, 10, 287, 32), true, false, tabInfo, -1, false);
	stName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stInfo = env->addStaticText(L"", rect<s32>(15, 37, 296, 60), false, true, tabInfo, -1, false);
	stInfo->setOverrideColor(special_color);
	stDataInfo = env->addStaticText(L"", rect<s32>(15, 60, 296, 83), false, true, tabInfo, -1, false);
	stDataInfo->setOverrideColor(special_color);
	stSetName = env->addStaticText(L"", rect<s32>(15, 83, 316, 106), false, true, tabInfo, -1, false);
	stSetName->setOverrideColor(setcolor);
	stText = env->addStaticText(L"", rect<s32>(15, 83, 287, 324), false, true, tabInfo, -1, false);
	scrCardText = env->addScrollBar(false, rect<s32>(267, 83, 287, 324), tabInfo, SCROLL_CARDTEXT);
	scrCardText->setLargeStep(1);
	scrCardText->setSmallStep(1);
	scrCardText->setVisible(false);
	//log
	irr::gui::IGUITab* tabLog =  wInfos->addTab(dataManager.GetSysString(1271));
	lstLog = env->addListBox(rect<s32>(10, 10, 290, 290), tabLog, LISTBOX_LOG, false);
	lstLog->setItemHeight(18);
	btnClearLog = env->addButton(rect<s32>(160, 300, 260, 325), tabLog, BUTTON_CLEAR_LOG, dataManager.GetSysString(1272));
	//system
	irr::gui::IGUITab* tabSystem = wInfos->addTab(dataManager.GetSysString(1273));
	chkAutoPos = env->addCheckBox(gameConf.autoplace, rect<s32>(20, 20, 280, 45), tabSystem, -1, dataManager.GetSysString(1274));
	chkRandomPos = env->addCheckBox(gameConf.randomplace, rect<s32>(40, 50, 300, 75), tabSystem, -1, dataManager.GetSysString(1275));
	chkAutoChain = env->addCheckBox(gameConf.autochain, rect<s32>(20, 80, 280, 105), tabSystem, -1, dataManager.GetSysString(1276));
	chkWaitChain = env->addCheckBox(false, rect<s32>(20, 110, 280, 135), tabSystem, -1, dataManager.GetSysString(1277));
	chkIgnore1 = env->addCheckBox(gameConf.muteopponent, rect<s32>(20, 170, 280, 195), tabSystem, -1, dataManager.GetSysString(1290));
	chkIgnore2 = env->addCheckBox(gameConf.mutespectator, rect<s32>(20, 200, 280, 225), tabSystem, -1, dataManager.GetSysString(1291));
	chkHideSetname = env->addCheckBox(false, rect<s32>(20, 140, 280, 165), tabSystem, -1, dataManager.GetSysString(1354));
	chkHideSetname->setChecked(gameConf.chkHideSetname != 0);
	chkHideChainButton = env->addCheckBox(false, rect<s32>(150, 80, 410, 105), tabSystem, CHECKBOX_ENABLE_HIDECHAIN, dataManager.GetSysString(1355));
	chkHideChainButton->setChecked(gameConf.chkHideChainButton != 0);
	chkEnableSound = env->addCheckBox(gameConf.enablesound, rect<s32>(20, 230, 280, 255), tabSystem, CHECKBOX_ENABLE_SOUND, dataManager.GetSysString(2046));
	scrSound = env->addScrollBar(true, rect<s32>(20, 260, 280, 270), tabSystem, SCROLL_SOUND);
	scrSound->setMax(100);
	scrSound->setMin(0);
	scrSound->setPos(gameConf.soundvolume * 100);
	scrSound->setLargeStep(1);
	scrSound->setSmallStep(1);
	chkEnableMusic  = env->addCheckBox(gameConf.enablemusic, rect<s32>(20, 275, 280, 300), tabSystem, CHECKBOX_ENABLE_MUSIC, dataManager.GetSysString(2047));
	scrMusic = env->addScrollBar(true, rect<s32>(20, 305, 280, 315), tabSystem, SCROLL_MUSIC);
	scrMusic->setMax(100);
	scrMusic->setMin(0);
	scrMusic->setPos(gameConf.musicvolume * 100);
	scrMusic->setLargeStep(1);
	scrMusic->setSmallStep(1);
	//
	wHand = env->addWindow(rect<s32>(500, 450, 825, 605), false, L"");
	wHand->getCloseButton()->setVisible(false);
	wHand->setDraggable(false);
	wHand->setDrawTitlebar(false);
	wHand->setVisible(false);
	for(int i = 0; i < 3; ++i) {
		btnHand[i] = env->addButton(rect<s32>(10 + 105 * i, 10, 105 + 105 * i, 144), wHand, BUTTON_HAND1 + i, L"");
		btnHand[i]->setImage(imageManager.tHand[i]);
	}
	//
	wFTSelect = env->addWindow(rect<s32>(550, 240, 780, 340), false, L"");
	wFTSelect->getCloseButton()->setVisible(false);
	wFTSelect->setVisible(false);
	btnFirst = env->addButton(rect<s32>(10, 30, 220, 55), wFTSelect, BUTTON_FIRST, dataManager.GetSysString(100));
	btnSecond = env->addButton(rect<s32>(10, 60, 220, 85), wFTSelect, BUTTON_SECOND, dataManager.GetSysString(101));
	//message (310)
	wMessage = env->addWindow(rect<s32>(490, 200, 840, 340), false, dataManager.GetSysString(1216));
	wMessage->getCloseButton()->setVisible(false);
	wMessage->setVisible(false);
	stMessage =  env->addStaticText(L"", rect<s32>(20, 20, 350, 100), false, true, wMessage, -1, false);
	stMessage->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnMsgOK = env->addButton(rect<s32>(130, 105, 220, 130), wMessage, BUTTON_MSG_OK, dataManager.GetSysString(1211));
	//auto fade message (310)
	wACMessage = env->addWindow(rect<s32>(490, 240, 840, 300), false, L"");
	wACMessage->getCloseButton()->setVisible(false);
	wACMessage->setVisible(false);
	wACMessage->setDrawBackground(false);
	stACMessage = env->addStaticText(L"", rect<s32>(0, 0, 350, 60), true, true, wACMessage, -1, true);
	stACMessage->setBackgroundColor(0xc0c0c0ff);
	stACMessage->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	//yes/no (310)
	wQuery = env->addWindow(rect<s32>(490, 200, 840, 340), false, dataManager.GetSysString(560));
	wQuery->getCloseButton()->setVisible(false);
	wQuery->setVisible(false);
	stQMessage =  env->addStaticText(L"", rect<s32>(20, 20, 350, 100), false, true, wQuery, -1, false);
	stQMessage->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnYes = env->addButton(rect<s32>(100, 105, 150, 130), wQuery, BUTTON_YES, dataManager.GetSysString(1213));
	btnNo = env->addButton(rect<s32>(200, 105, 250, 130), wQuery, BUTTON_NO, dataManager.GetSysString(1214));
	//options (310)
	wOptions = env->addWindow(rect<s32>(490, 200, 840, 340), false, L"");
	wOptions->getCloseButton()->setVisible(false);
	wOptions->setVisible(false);
	stOptions =  env->addStaticText(L"", rect<s32>(20, 20, 350, 100), false, true, wOptions, -1, false);
	stOptions->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
	btnOptionOK = env->addButton(rect<s32>(130, 105, 220, 130), wOptions, BUTTON_OPTION_OK, dataManager.GetSysString(1211));
	btnOptionp = env->addButton(rect<s32>(20, 105, 60, 130), wOptions, BUTTON_OPTION_PREV, L"<<<");
	btnOptionn = env->addButton(rect<s32>(290, 105, 330, 130), wOptions, BUTTON_OPTION_NEXT, L">>>");
	//pos select
	wPosSelect = env->addWindow(rect<s32>(340, 200, 935, 410), false, dataManager.GetSysString(561));
	wPosSelect->getCloseButton()->setVisible(false);
	wPosSelect->setVisible(false);
	btnPSAU = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(10, 45, 150, 185), wPosSelect, BUTTON_POS_AU);
	btnPSAU->setImageScale(core::vector2df(0.5, 0.5));
	btnPSAD = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(155, 45, 295, 185), wPosSelect, BUTTON_POS_AD);
	btnPSAD->setImageScale(core::vector2df(0.5, 0.5));
	btnPSAD->setImage(imageManager.tCover[0], rect<s32>(0, 0, 177, 254));
	btnPSDU = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(300, 45, 440, 185), wPosSelect, BUTTON_POS_DU);
	btnPSDU->setImageScale(core::vector2df(0.5, 0.5));
	btnPSDU->setImageRotation(270);
	btnPSDD = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(445, 45, 585, 185), wPosSelect, BUTTON_POS_DD);
	btnPSDD->setImageScale(core::vector2df(0.5, 0.5));
	btnPSDD->setImageRotation(270);
	btnPSDD->setImage(imageManager.tCover[0], rect<s32>(0, 0, 177, 254));
	//card select
	wCardSelect = env->addWindow(rect<s32>(320, 100, 1000, 400), false, L"");
	wCardSelect->getCloseButton()->setVisible(false);
	wCardSelect->setVisible(false);
	for(int i = 0; i < 5; ++i) {
		stCardPos[i] = env->addStaticText(L"", rect<s32>(40 + 125 * i, 30, 139 + 125 * i, 50), true, false, wCardSelect, -1, true);
		stCardPos[i]->setBackgroundColor(0xffffffff);
		stCardPos[i]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		btnCardSelect[i] = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(30 + 125 * i, 55, 150 + 125 * i, 225), wCardSelect, BUTTON_CARD_0 + i);
		btnCardSelect[i]->setImageScale(core::vector2df(0.6f, 0.6f));
	}
	scrCardList = env->addScrollBar(true, rect<s32>(30, 235, 650, 255), wCardSelect, SCROLL_CARD_SELECT);
	btnSelectOK = env->addButton(rect<s32>(300, 265, 380, 290), wCardSelect, BUTTON_CARD_SEL_OK, dataManager.GetSysString(1211));
	//card display
	wCardDisplay = env->addWindow(rect<s32>(320, 100, 1000, 400), false, L"");
	wCardDisplay->getCloseButton()->setVisible(false);
	wCardDisplay->setVisible(false);
	for (int i = 0; i < 5; ++i) {
		stDisplayPos[i] = env->addStaticText(L"", rect<s32>(40 + 125 * i, 30, 139 + 125 * i, 50), true, false, wCardDisplay, -1, true);
		stDisplayPos[i]->setBackgroundColor(0xffffffff);
		stDisplayPos[i]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		btnCardDisplay[i] = irr::gui::CGUIImageButton::addImageButton(env, rect<s32>(30 + 125 * i, 55, 150 + 125 * i, 225), wCardDisplay, BUTTON_DISPLAY_0 + i);
		btnCardDisplay[i]->setImageScale(core::vector2df(0.6f, 0.6f));
	}
	scrDisplayList = env->addScrollBar(true, rect<s32>(30, 235, 650, 255), wCardDisplay, SCROLL_CARD_DISPLAY);
	btnDisplayOK = env->addButton(rect<s32>(300, 265, 380, 290), wCardDisplay, BUTTON_CARD_DISP_OK, dataManager.GetSysString(1211));
	//announce number
	wANNumber = env->addWindow(rect<s32>(550, 200, 780, 295), false, L"");
	wANNumber->getCloseButton()->setVisible(false);
	wANNumber->setVisible(false);
	cbANNumber =  env->addComboBox(rect<s32>(40, 30, 190, 50), wANNumber, -1);
	cbANNumber->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnANNumberOK = env->addButton(rect<s32>(80, 60, 150, 85), wANNumber, BUTTON_ANNUMBER_OK, dataManager.GetSysString(1211));
	//announce card
	wANCard = env->addWindow(rect<s32>(560, 170, 970, 370), false, L"");
	wANCard->getCloseButton()->setVisible(false);
	wANCard->setVisible(false);
	ebANCard = env->addEditBox(L"", rect<s32>(20, 25, 390, 45), true, wANCard, EDITBOX_ANCARD);
	ebANCard->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	lstANCard = env->addListBox(rect<s32>(20, 50, 390, 160), wANCard, LISTBOX_ANCARD, true);
	btnANCardOK = env->addButton(rect<s32>(60, 165, 350, 190), wANCard, BUTTON_ANCARD_OK, dataManager.GetSysString(1211));
	//announce attribute
	wANAttribute = env->addWindow(rect<s32>(500, 200, 830, 285), false, dataManager.GetSysString(562));
	wANAttribute->getCloseButton()->setVisible(false);
	wANAttribute->setVisible(false);
	for(int filter = 0x1, i = 0; i < 7; filter <<= 1, ++i)
		chkAttribute[i] = env->addCheckBox(false, rect<s32>(10 + (i % 4) * 80, 25 + (i / 4) * 25, 90 + (i % 4) * 80, 50 + (i / 4) * 25),
		                                   wANAttribute, CHECK_ATTRIBUTE, dataManager.FormatAttribute(filter));
	//announce attribute
	wANRace = env->addWindow(rect<s32>(480, 200, 850, 385), false, dataManager.GetSysString(563));
	wANRace->getCloseButton()->setVisible(false);
	wANRace->setVisible(false);
	for(int filter = 0x1, i = 0; i < 24; filter <<= 1, ++i)
		chkRace[i] = env->addCheckBox(false, rect<s32>(10 + (i % 4) * 90, 25 + (i / 4) * 25, 100 + (i % 4) * 90, 50 + (i / 4) * 25),
		                              wANRace, CHECK_RACE, dataManager.FormatRace(filter));
	//selection hint
	stHintMsg = env->addStaticText(L"", rect<s32>(500, 60, 820, 90), true, false, 0, -1, false);
	stHintMsg->setBackgroundColor(0xc0ffffff);
	stHintMsg->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stHintMsg->setVisible(false);
	stTip = env->addStaticText(L"", rect<s32>(0, 0, 150, 150), false, true, 0, -1, true);
	stTip->setOverrideColor(0xff000000);
	stTip->setBackgroundColor(0xc0ffffff);
	stTip->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	stTip->setVisible(false);
	//cmd menu
	wCmdMenu = env->addWindow(rect<s32>(10, 10, 110, 179), false, L"");
	wCmdMenu->setDrawTitlebar(false);
	wCmdMenu->setVisible(false);
	wCmdMenu->getCloseButton()->setVisible(false);
	btnActivate = env->addButton(rect<s32>(1, 1, 99, 21), wCmdMenu, BUTTON_CMD_ACTIVATE, dataManager.GetSysString(1150));
	btnSummon = env->addButton(rect<s32>(1, 22, 99, 42), wCmdMenu, BUTTON_CMD_SUMMON, dataManager.GetSysString(1151));
	btnSPSummon = env->addButton(rect<s32>(1, 43, 99, 63), wCmdMenu, BUTTON_CMD_SPSUMMON, dataManager.GetSysString(1152));
	btnMSet = env->addButton(rect<s32>(1, 64, 99, 84), wCmdMenu, BUTTON_CMD_MSET, dataManager.GetSysString(1153));
	btnSSet = env->addButton(rect<s32>(1, 106, 99, 126), wCmdMenu, BUTTON_CMD_SSET, dataManager.GetSysString(1159));
	btnRepos = env->addButton(rect<s32>(1, 106, 99, 126), wCmdMenu, BUTTON_CMD_REPOS, dataManager.GetSysString(1154));
	btnAttack = env->addButton(rect<s32>(1, 127, 99, 147), wCmdMenu, BUTTON_CMD_ATTACK, dataManager.GetSysString(1157));
	btnShowList = env->addButton(rect<s32>(1, 148, 99, 168), wCmdMenu, BUTTON_CMD_SHOWLIST, dataManager.GetSysString(1158));
	//deck edit
	wEdit.Load();
	//replay window
	wReplayList.Load();
	//single play window
	wSinglePlay = env->addWindow(rect<s32>(220, 100, 800, 520), false, dataManager.GetSysString(1201));
	wSinglePlay->getCloseButton()->setVisible(false);
	wSinglePlay->setVisible(false);
	lstSinglePlayList = env->addListBox(rect<s32>(10, 30, 350, 400), wSinglePlay, LISTBOX_SINGLEPLAY_LIST, true);
	lstSinglePlayList->setItemHeight(18);
	btnLoadSinglePlay = env->addButton(rect<s32>(460, 355, 570, 380), wSinglePlay, BUTTON_LOAD_SINGLEPLAY, dataManager.GetSysString(1211));
	btnSinglePlayCancel = env->addButton(rect<s32>(460, 385, 570, 410), wSinglePlay, BUTTON_CANCEL_SINGLEPLAY, dataManager.GetSysString(1210));
	env->addStaticText(dataManager.GetSysString(1352), rect<s32>(360, 30, 570, 50), false, true, wSinglePlay);
	stSinglePlayInfo = env->addStaticText(L"", rect<s32>(360, 60, 570, 350), false, true, wSinglePlay);
	//replay save
	wReplaySave = env->addWindow(rect<s32>(510, 200, 820, 320), false, dataManager.GetSysString(1340));
	wReplaySave->getCloseButton()->setVisible(false);
	wReplaySave->setVisible(false);
	env->addStaticText(dataManager.GetSysString(1342), rect<s32>(20, 25, 290, 45), false, false, wReplaySave);
	ebRSName =  env->addEditBox(L"", rect<s32>(20, 50, 290, 70), true, wReplaySave, -1);
	ebRSName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	btnRSYes = env->addButton(rect<s32>(70, 80, 140, 105), wReplaySave, BUTTON_REPLAY_SAVE, dataManager.GetSysString(1341));
	btnRSNo = env->addButton(rect<s32>(170, 80, 240, 105), wReplaySave, BUTTON_REPLAY_CANCEL, dataManager.GetSysString(1212));
	//replay control
	wReplayControl = env->addStaticText(L"", rect<s32>(205, 143, 295, 273), true, false, 0, -1, true);
	wReplayControl->setVisible(false);
	btnReplayStart = env->addButton(rect<s32>(5, 5, 85, 25), wReplayControl, BUTTON_REPLAY_START, dataManager.GetSysString(1343));
	btnReplayPause = env->addButton(rect<s32>(5, 5, 85, 25), wReplayControl, BUTTON_REPLAY_PAUSE, dataManager.GetSysString(1344));
	btnReplayStep = env->addButton(rect<s32>(5, 55, 85, 75), wReplayControl, BUTTON_REPLAY_STEP, dataManager.GetSysString(1345));
	btnReplayUndo = env->addButton(rect<s32>(5, 80, 85, 100), wReplayControl, BUTTON_REPLAY_UNDO, dataManager.GetSysString(1360));
	btnReplaySwap = env->addButton(rect<s32>(5, 30, 85, 50), wReplayControl, BUTTON_REPLAY_SWAP, dataManager.GetSysString(1346));
	btnReplayExit = env->addButton(rect<s32>(5, 105, 85, 125), wReplayControl, BUTTON_REPLAY_EXIT, dataManager.GetSysString(1347));
	//chat
	wChat = env->addWindow(rect<s32>(305, 615, 1020, 640), false, L"");
	wChat->getCloseButton()->setVisible(false);
	wChat->setDraggable(false);
	wChat->setDrawTitlebar(false);
	wChat->setVisible(false);
	ebChatInput = env->addEditBox(L"", rect<s32>(3, 2, 710, 22), true, wChat, EDITBOX_CHAT);
	//leave/surrender/exit
	btnLeaveGame = env->addButton(rect<s32>(205, 5, 295, 80), 0, BUTTON_LEAVE_GAME, L"");
	btnLeaveGame->setVisible(false);
	device->setEventReceiver(&wMenu);
	env->getSkin()->setFont(guiFont);
	env->setFocus(wMenu.GetWindow());
	for (u32 i = 0; i < EGDC_COUNT; ++i) {
		SColor col = env->getSkin()->getColor((EGUI_DEFAULT_COLOR)i);
		col.setAlpha(224);
		env->getSkin()->setColor((EGUI_DEFAULT_COLOR)i, col);
	}
    engineSound = irrklang::createIrrKlangDevice();
    engineMusic = irrklang::createIrrKlangDevice(); 
	hideChat=false;
	hideChatTimer=0;
	return true;
}
void Game::MainLoop() {
	wchar_t cap[256];
	camera = smgr->addCameraSceneNode(0);
	irr::core::matrix4 mProjection;
	BuildProjectionMatrix(mProjection, -0.90f, 0.45f, -0.42f, 0.42f, 1.0f, 100.0f);
	camera->setProjectionMatrix(mProjection);

	mProjection.buildCameraLookAtMatrixLH(vector3df(4.2f, 8.0f, 7.8f), vector3df(4.2f, 0, 0), vector3df(0, 0, 1));
	camera->setViewMatrixAffector(mProjection);
	smgr->setAmbientLight(SColorf(1.0f, 1.0f, 1.0f));
	float atkframe = 0.1f;
	irr::ITimer* timer = device->getTimer();
	timer->setTime(0);
	int fps = 0;
	int cur_time = 0;
	while(device->run()) {
		dimension2du size = driver->getScreenSize();
		if (window_size != size) {
			window_size = size;
			OnResize();
		}
		if(gameConf.use_d3d)
			linePattern = (linePattern + 1) % 30;
		else
			linePattern = (linePattern << 1) | (linePattern >> 15);
		atkframe += 0.1f;
		atkdy = (float)sin(atkframe);
		driver->beginScene(true, true, SColor(0, 0, 0, 0));
		if(imageManager.tBackGround)
			driver->draw2DImage(imageManager.tBackGround, Resize(0, 0, 1024, 640), recti(0, 0, imageManager.tBackGround->getOriginalSize().Width, imageManager.tBackGround->getOriginalSize().Height));
		gMutex.Lock();
		if(dInfo.isStarted) {
	      if(imageManager.tBackGround2)
			driver->draw2DImage(imageManager.tBackGround2, Resize(0, 0, 1024, 640), recti(0, 0, imageManager.tBackGround->getOriginalSize().Width, imageManager.tBackGround->getOriginalSize().Height));
		  imageManager.LoadPendingTextures();
		  if(mainGame->showcardcode == 1 || mainGame->showcardcode == 3)
			  Game::PlayMusic("./sound/duelwin.mp3",true);
		  else if(mainGame->showcardcode == 2)
			  Game::PlayMusic("./sound/duellose.mp3",true);
		  else if(mainGame->dInfo.lp[LocalPlayer(0)] <= mainGame->dInfo.lp[LocalPlayer(1)]/2)
			  Game::PlayMusic("./sound/song-disadvantage.mp3",true);
		  else if(mainGame->dInfo.lp[LocalPlayer(0)] >= mainGame->dInfo.lp[LocalPlayer(1)]*2)
			  Game::PlayMusic("./sound/song-advantage.mp3",true);
		  else
			  Game::PlayMusic("./sound/song.mp3",true);
			DrawBackGround();
			DrawCards();
			DrawMisc();
			smgr->drawAll();
			driver->setMaterial(irr::video::IdentityMaterial);
			driver->clearZBuffer();
		} else if(is_building) {
			DrawDeckBd();
			Game::PlayMusic("./sound/deck.mp3",true);
		}
		else {
			Game::PlayMusic("./sound/menu.mp3",true);
		}
		DrawGUI();
		DrawSpec();
		gMutex.Unlock();
		if(signalFrame > 0) {
			signalFrame--;
			if(!signalFrame)
				frameSignal.Set();
		}
		if(waitFrame >= 0) {
			waitFrame++;
			if(waitFrame % 90 == 0) {
				stHintMsg->setText(dataManager.GetSysString(1390));
			} else if(waitFrame % 90 == 30) {
				stHintMsg->setText(dataManager.GetSysString(1391));
			} else if(waitFrame % 90 == 60) {
				stHintMsg->setText(dataManager.GetSysString(1392));
			}
		}
		driver->endScene();
		if(closeSignal.Wait(0))
			CloseDuelWindow();
		if(!device->isWindowActive())
			ignore_chain = false;
		fps++;
		cur_time = timer->getTime();
		if(cur_time < fps * 17 - 20)
#ifdef _WIN32
			Sleep(20);
#else
			usleep(20000);
#endif
		if(cur_time >= 1000) {
			myswprintf(cap, L"YGOPro DevPro | FPS: %d", fps);
			device->setWindowCaption(cap);
			fps = 0;
			cur_time -= 1000;
			timer->setTime(0);
			if(dInfo.time_player == 0 || dInfo.time_player == 1)
				if(dInfo.time_left[dInfo.time_player])
					dInfo.time_left[dInfo.time_player]--;
		}
	}
	DuelClient::StopClient(true);
	if(mainGame->dInfo.isSingleMode)
		SingleMode::StopPlay(true);
#ifdef _WIN32
	Sleep(500);
#else
	usleep(500000);
#endif
	//SaveConfig();
//	device->drop();
	engineMusic->drop(); 
}
void Game::BuildProjectionMatrix(irr::core::matrix4& mProjection, f32 left, f32 right, f32 bottom, f32 top, f32 znear, f32 zfar) {
	for(int i = 0; i < 16; ++i)
		mProjection[i] = 0;
	mProjection[0] = 2.0f * znear / (right - left);
	mProjection[5] = 2.0f * znear / (top - bottom);
	mProjection[8] = (left + right) / (left - right);
	mProjection[9] = (top + bottom) / (bottom - top);
	mProjection[10] = zfar / (zfar - znear);
	mProjection[11] = 1.0f;
	mProjection[14] = znear * zfar / (znear - zfar);
}
void Game::InitStaticText(irr::gui::IGUIStaticText* pControl, u32 cWidth, u32 cHeight, irr::gui::CGUITTFont* font, const wchar_t* text) {
	SetStaticText(pControl, cWidth, font, text);
	if(font->getDimension(dataManager.strBuffer).Height <= cHeight) {
		scrCardText->setVisible(false);
		return;
	}
	SetStaticText(pControl, cWidth-25, font, text);
	u32 fontheight = font->getDimension(L"A").Height + font->getKerningHeight();
	u32 step = (font->getDimension(dataManager.strBuffer).Height - cHeight) / fontheight + 1;
	scrCardText->setVisible(true);
	scrCardText->setMin(0);
	scrCardText->setMax(step);
	scrCardText->setPos(0);
}
void Game::SetStaticText(irr::gui::IGUIStaticText* pControl, u32 cWidth, irr::gui::CGUITTFont* font, const wchar_t* text, u32 pos) {
	int pbuffer = 0, lsnz = 0;
	u32 _width = 0, _height = 0, s = font->getCharDimension(L' ').Width;
	for (size_t i = 0; text[i] != 0 && i < wcslen(text); ++i) {
		if (text[i] == L' ') {
			lsnz = i;
			if (_width + s > cWidth) {
				dataManager.strBuffer[pbuffer++] = L'\n';
				_width = 0;
				_height++;
				if(_height == pos)
					pbuffer = 0;
			}
			else {
				dataManager.strBuffer[pbuffer++] = L' ';
				_width += s;
			}
		}
		else if(text[i] == L'\n') {
			dataManager.strBuffer[pbuffer++] = L'\n';
			_width = 0;
			_height++;
			if(_height == pos)
				pbuffer = 0;
		}
		else {
			if((_width += font->getCharDimension(text[i]).Width) > cWidth) {
				dataManager.strBuffer[lsnz] = L'\n';
				_width = 0;
				for(int j = lsnz + 1; j < i; j++) {
					_width += font->getCharDimension(text[j]).Width;
				}
			}
			dataManager.strBuffer[pbuffer++] = text[i];
		}
	}
	dataManager.strBuffer[pbuffer] = 0;
	pControl->setText(dataManager.strBuffer);
}
void Game::RefreshDeck(irr::gui::IGUIComboBox* cbDeck) {
	cbDeck->clear();
#ifdef _WIN32
	WIN32_FIND_DATAW fdataw;
	HANDLE fh = FindFirstFileW(L"./deck/*.ydk", &fdataw);
	if(fh == INVALID_HANDLE_VALUE)
		return;
	do {
		if(!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			wchar_t* pf = fdataw.cFileName;
			while(*pf) pf++;
			while(*pf != L'.') pf--;
			*pf = 0;
			cbDeck->addItem(fdataw.cFileName);
		}
	} while(FindNextFileW(fh, &fdataw));
	FindClose(fh);
#else
	DIR * dir;
	struct dirent * dirp;
	if((dir = opendir("./deck/")) == NULL)
		return;
	while((dirp = readdir(dir)) != NULL) {
		size_t len = strlen(dirp->d_name);
		if(len < 5 || strcasecmp(dirp->d_name + len - 4, ".ydk") != 0)
			continue;
		dirp->d_name[len - 4] = 0;
		wchar_t wname[256];
		BufferIO::DecodeUTF8(dirp->d_name, wname);
		cbDeck->addItem(wname);
	}
	closedir(dir);
#endif
	for(size_t i = 0; i < cbDeck->getItemCount(); ++i) {
		if(!wcscmp(cbDeck->getItem(i), gameConf.lastdeck)) {
			cbDeck->setSelected(i);
			break;
		}
	}
}
void Game::RefreshReplay() {
	wReplayList.ClearReplayList();
#ifdef _WIN32
	WIN32_FIND_DATAW fdataw;
	HANDLE fh = FindFirstFileW(L"./replay/*.yrp", &fdataw);
	if(fh == INVALID_HANDLE_VALUE)
		return;
	do {
		if(!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && Replay::CheckReplay(fdataw.cFileName)) {
			wReplayList.AddListItem(fdataw.cFileName);
		}
	} while(FindNextFileW(fh, &fdataw));
	FindClose(fh);
#else
	DIR * dir;
	struct dirent * dirp;
	if((dir = opendir("./replay/")) == NULL)
		return;
	while((dirp = readdir(dir)) != NULL) {
		size_t len = strlen(dirp->d_name);
		if(len < 5 || strcasecmp(dirp->d_name + len - 4, ".yrp") != 0)
			continue;
		wchar_t wname[256];
		BufferIO::DecodeUTF8(dirp->d_name, wname);
		if(Replay::CheckReplay(wname))
			lstReplayList->addItem(wname);
	}
	closedir(dir);
#endif
}
void Game::RefreshSingleplay() {
	lstSinglePlayList->clear();
#ifdef _WIN32
	WIN32_FIND_DATAW fdataw;
	HANDLE fh = FindFirstFileW(L"./single/*.lua", &fdataw);
	if(fh == INVALID_HANDLE_VALUE)
		return;
	do {
		if(!(fdataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			lstSinglePlayList->addItem(fdataw.cFileName);
	} while(FindNextFileW(fh, &fdataw));
	FindClose(fh);
#else
	DIR * dir;
	struct dirent * dirp;
	if((dir = opendir("./single/")) == NULL)
		return;
	while((dirp = readdir(dir)) != NULL) {
		size_t len = strlen(dirp->d_name);
		if(len < 5 || strcasecmp(dirp->d_name + len - 4, ".lua") != 0)
			continue;
		wchar_t wname[256];
		BufferIO::DecodeUTF8(dirp->d_name, wname);
		lstSinglePlayList->addItem(wname);
	}
	closedir(dir);
#endif
}
void Game::LoadConfig() {
	FILE* fp = fopen("system.conf", "r");
	if(!fp)
		return;
	char linebuf[256];
	char strbuf[32];
	char valbuf[256];
	wchar_t wstr[256];
	gameConf.antialias = 0;
	gameConf.serverport = 7911;
	gameConf.textfontsize = 12;
	gameConf.nickname[0] = 0;
	gameConf.gamename[0] = 0;
	gameConf.lastdeck[0] = 0;
	gameConf.numfont[0] = 0;
	gameConf.textfont[0] = 0;
	gameConf.lastip[0] = 0;
	gameConf.lastport[0] = 0;
	gameConf.roompass[0] = 0;
	gameConf.lastreplay[0] = 0;
	gameConf.lastpuzzle[0] = 0;
	gameConf.autoplace = true;
	gameConf.randomplace = false;
	gameConf.autochain = true;
	gameConf.nodelay = false;
	gameConf.enablemusic = true;
	gameConf.musicvolume = 1.0;
	gameConf.enablesound = true;
	gameConf.soundvolume = 1.0;
	gameConf.skin_index = -1;
	gameConf.fullscreen = false;
	gameConf.enablesleeveloading = true;
	gameConf.mutespectator = false;
	gameConf.muteopponent = false;
	gameConf.forced = false;
	gameConf.savereplay = false;
	gameConf.control_mode = 0;
	gameConf.chkHideSetname = 0;
	gameConf.chkHideChainButton = 0;
	fseek(fp, 0, SEEK_END);
	int fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	while(ftell(fp) < fsize) {
		fgets(linebuf, 250, fp);
		sscanf(linebuf, "%s = %99[^\n]", strbuf, valbuf);
		if(!strcmp(strbuf, "antialias")) {
			gameConf.antialias = atoi(valbuf);
		} else if(!strcmp(strbuf, "use_d3d")) {
			gameConf.use_d3d = atoi(valbuf) > 0;
		} else if(!strcmp(strbuf, "errorlog")) {
			enable_log = atoi(valbuf);
		} else if(!strcmp(strbuf, "nickname")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.nickname, 20);
		} else if(!strcmp(strbuf, "gamename")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.gamename, 20);
		} else if(!strcmp(strbuf, "lastdeck")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.lastdeck, 64);
		} else if(!strcmp(strbuf, "textfont")) {
			sscanf(linebuf, "%s = %s", strbuf, valbuf);
			BufferIO::DecodeUTF8(valbuf, wstr);
			int textfontsize;
			sscanf(linebuf, "%s = %s %d", strbuf, valbuf, &textfontsize);
			gameConf.textfontsize = textfontsize;
			BufferIO::CopyWStr(wstr, gameConf.textfont, 256);
		} else if(!strcmp(strbuf, "numfont")) {
			sscanf(linebuf, "%s = %s", strbuf, valbuf);
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.numfont, 256);
		} else if(!strcmp(strbuf, "serverport")) {
			gameConf.serverport = atoi(valbuf);
		} else if(!strcmp(strbuf, "lastip")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.lastip, 20);
		} else if(!strcmp(strbuf, "lastport")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.lastport, 20);
		} else if(!strcmp(strbuf, "roompass")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.roompass, 40);
		} else if(!strcmp(strbuf,"auto_card_placing")) {
			gameConf.autoplace = atoi(valbuf) > 0;
		} else if(!strcmp(strbuf,"random_card_placing")) {
			gameConf.randomplace = atoi(valbuf) > 0;
		} else if(!strcmp(strbuf,"auto_chain_order")) {
			gameConf.autochain = atoi(valbuf) > 0;
		} else if(!strcmp(strbuf,"no_delay_for_chain")) {
			gameConf.nodelay = atoi(valbuf) > 0;
		} else if(!strcmp(strbuf,"skin_index")) {
			gameConf.skin_index = atoi(valbuf);
		} else if(!strcmp(strbuf,"fullscreen")) {
			gameConf.fullscreen = atoi(valbuf) > 0;
		} else if(!strcmp(strbuf,"enable_music")) {
			gameConf.enablemusic = atoi(valbuf) > 0;
		} else if(!strcmp(strbuf,"music_volume")) {
			gameConf.musicvolume = atof(valbuf) / 100;
		} else if(!strcmp(strbuf,"enable_sound")) {
			gameConf.enablesound = atoi(valbuf) > 0;
		} else if(!strcmp(strbuf,"sound_volume")) {
			gameConf.soundvolume = atof(valbuf) / 100;
		} else if(!strcmp(strbuf,"lastpuzzle")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.lastpuzzle, 256);
		} else if(!strcmp(strbuf,"lastreplay")) {
			BufferIO::DecodeUTF8(valbuf, wstr);
			BufferIO::CopyWStr(wstr, gameConf.lastreplay, 256);
		} else if(!strcmp(strbuf,"enable_sleeve_loading")) {
			gameConf.enablesleeveloading = atoi(valbuf) > 0;
		} else if(!strcmp(strbuf,"mute_opponent")) {
			gameConf.muteopponent = atoi(valbuf) > 0;
		} else if(!strcmp(strbuf,"mute_spectators")) {
			gameConf.mutespectator = atoi(valbuf) > 0;
		} else if(!strcmp(strbuf,"forced")) {
			gameConf.forced = atoi(valbuf) > 0;
		} else if (!strcmp(strbuf, "save_last_replay")) {
			gameConf.savereplay = atoi(valbuf) > 0;
		} else if (!strcmp(strbuf, "control_mode")) {
			gameConf.control_mode = atoi(valbuf);
		} else if (!strcmp(strbuf, "hide_setname")) {
			gameConf.chkHideSetname = atoi(valbuf) > 0;
		} else if (!strcmp(strbuf, "hide_chain_button")) {
			gameConf.chkHideChainButton = atoi(valbuf) > 0;
		}
	}
	fclose(fp);
}
void Game::PlayMusic(char* song, bool loop){
	if(gameConf.enablemusic){
		if(!engineMusic->isCurrentlyPlaying(song)){
					engineMusic->stopAllSounds();
					engineMusic->play2D(song, loop);
					engineMusic->setSoundVolume(gameConf.musicvolume);
		}
	}
}
void Game::PlaySound(char* sound){
	if(gameConf.enablesound){
		engineSound->play2D(sound);
		engineSound->setSoundVolume(gameConf.soundvolume);
	}
}
void Game::ShowCardInfo(int code) {
	CardData cd;
	wchar_t formatBuffer[256];
	displayedcard = code;
	if(!dataManager.GetData(code, &cd))
		memset(&cd, 0, sizeof(CardData));
	imgCard->setImage(imageManager.GetTexture(code));
	imgCard->setScaleImage(true);
	if(cd.alias != 0 && (cd.alias - code < 10 || code - cd.alias < 10))
		myswprintf(formatBuffer, L"%ls[%08d]", dataManager.GetName(cd.alias), cd.alias);
	else myswprintf(formatBuffer, L"%ls[%08d]", dataManager.GetName(code), code);
	stName->setText(formatBuffer);
	int offset = 0;
	if (!mainGame->chkHideSetname->isChecked()) {
		unsigned long long sc = cd.setcode;
		if (cd.alias) {
			auto aptr = dataManager._datas.find(cd.alias);
			if (aptr != dataManager._datas.end())
				sc = aptr->second.setcode;
		}
		if (sc) {
			offset = 30;
			myswprintf(formatBuffer, L"%ls%ls", dataManager.GetSysString(1329), dataManager.FormatSetName(sc));
			stSetName->setText(formatBuffer);
		}
		else
			stSetName->setText(L"");
	}
	else {
		stSetName->setText(L"");
	}
	if(cd.type & TYPE_MONSTER) {
		myswprintf(formatBuffer, L"[%ls] %ls/%ls", dataManager.FormatType(cd.type), dataManager.FormatRace(cd.race), dataManager.FormatAttribute(cd.attribute));
		stInfo->setText(formatBuffer);
		int form = 0x2605;
		if (cd.type & TYPE_XYZ) ++form;
		myswprintf(formatBuffer, L"[%c%d] ", form, cd.level);
		wchar_t adBuffer[16];
		if(cd.attack < 0 && cd.defence < 0)
			myswprintf(adBuffer, L"?/?");
		else if(cd.attack < 0)
			myswprintf(adBuffer, L"?/%d", cd.defence);
		else if(cd.defence < 0)
			myswprintf(adBuffer, L"%d/?", cd.attack);
		else
			myswprintf(adBuffer, L"%d/%d", cd.attack, cd.defence); 
		wcscat(formatBuffer, adBuffer);
		if(cd.type & TYPE_PENDULUM) {
			wchar_t scaleBuffer[16];
			myswprintf(scaleBuffer, L" %d/%d", cd.lscale, cd.rscale);
			wcscat(formatBuffer, scaleBuffer);
		}
		stDataInfo->setText(formatBuffer);
		stDataInfo->setText(formatBuffer);
		stSetName->setRelativePosition(rect<s32>(15, 83, 316 * window_size.Width / 1024 - 30, 116));
		stText->setRelativePosition(recti(15, 83 + offset, 296 * window_size.Width / 1024 - 30, 324 * window_size.Height / 640));
		scrCardText->setRelativePosition(rect<s32>(267, 83 + offset, 287, 324 * window_size.Height / 640));
	} else {
		myswprintf(formatBuffer, L"[%ls]", dataManager.FormatType(cd.type));
		stInfo->setText(formatBuffer);
		stDataInfo->setText(L"");
		stSetName->setRelativePosition(rect<s32>(15, 60, 316 * window_size.Height / 640, 83));
		stText->setRelativePosition(rect<s32>(15, 60 + offset, 287 * window_size.Width / 1024 - 30, 324 * window_size.Height / 640));
		scrCardText->setRelativePosition(rect<s32>(267, 60 + offset, 287, 324 * window_size.Height / 640));
	}
	showingtext = dataManager.GetText(code);
	const auto& tsize = stText->getRelativePosition();
	InitStaticText(stText, tsize.getWidth() - 30, tsize.getHeight(), textFont, showingtext);
}
void Game::AddChatMsg(wchar_t* msg, int player) {
	for(int i = 7; i > 0; --i) {
		chatMsg[i] = chatMsg[i - 1];
		chatTiming[i] = chatTiming[i - 1];
		chatType[i] = chatType[i - 1];
	}
	chatMsg[0].clear();
	chatTiming[0] = 1200;
	chatType[0] = player;
	switch(player) {
	case 0: //from host
		chatMsg[0].append(dInfo.hostname);
		chatMsg[0].append(L": ");
		break;
	case 1: //from client
		chatMsg[0].append(dInfo.clientname);
		chatMsg[0].append(L": ");
		break;
	case 2: //host tag
		chatMsg[0].append(dInfo.hostname_tag);
		chatMsg[0].append(L": ");
		break;
	case 3: //client tag
		chatMsg[0].append(dInfo.clientname_tag);
		chatMsg[0].append(L": ");
		break;
	case 7: //local name
		chatMsg[0].append(mainGame->wLan.GetText(EDITBOX_NICKNAME));
		chatMsg[0].append(L": ");
		break;
	case 8: //system custom message, no prefix.
		chatMsg[0].append(L"[System]: ");
		break;
	case 9: //error message
		chatMsg[0].append(L"[Script error:] ");
		break;
	default: //from watcher or unknown
		if(player < 11 || player > 19)
			chatMsg[0].append(L"[---]: ");
	}
	chatMsg[0].append(msg);
}
void Game::ClearTextures() {
	matManager.mCard.setTexture(0, 0);
	mainGame->imgCard->setImage(0);
	mainGame->btnPSAU->setImage();
	mainGame->btnPSDU->setImage();
	for (int i = 0; i <= 4; ++i) {
		mainGame->btnCardSelect[i]->setImage();
		mainGame->btnCardDisplay[i]->setImage();
	}
	imageManager.ClearTexture();
}
void Game::CloseDuelWindow() {
	for(auto wit = fadingList.begin(); wit != fadingList.end(); ++wit) {
		if(wit->isFadein)
			wit->autoFadeoutFrame = 1;
	}
	wACMessage->setVisible(false);
	wANAttribute->setVisible(false);
	wANCard->setVisible(false);
	wANNumber->setVisible(false);
	wANRace->setVisible(false);
	wCardImg->setVisible(false);
	wCardSelect->setVisible(false);
	wCardDisplay->setVisible(false);
	wCmdMenu->setVisible(false);
	wFTSelect->setVisible(false);
	wHand->setVisible(false);
	wInfos->setVisible(false);
	wMessage->setVisible(false);
	wOptions->setVisible(false);
	wPhase->setVisible(false);
	wPosSelect->setVisible(false);
	wQuery->setVisible(false);
	wReplayControl->setVisible(false);
	wReplaySave->setVisible(false);
	stHintMsg->setVisible(false);
	wEdit.Hide();
	btnLeaveGame->setVisible(false);
	btnChainIgnore->setVisible(false);
	btnChainAlways->setVisible(false);
	btnChainWhenAvail->setVisible(false);
	wChat->setVisible(false);
	lstLog->clear();
	logParam.clear();
	wLan.ClearHostList();
	DuelClient::hosts.clear();
	ClearTextures();
	closeDoneSignal.Set();
}
int Game::LocalPlayer(int player) {
	return dInfo.isFirst ? player : 1 - player;
}
const wchar_t* Game::LocalName(int local_player) {
	return local_player == 0 ? dInfo.hostname : dInfo.clientname;
}
void Game::OnResize()
{
	wMenu.OnResize();
	wEdit.OnResize();
	wLan.OnResize();
	wHost.OnResize();
	wHostPrepare->setRelativePosition(ResizeWin(270, 120, 750+50, 440));
	wReplayList.OnResize();
	wSinglePlay->setRelativePosition(ResizeWin(220, 100, 800, 520));
	wHand->setRelativePosition(ResizeWin(500, 450, 825, 605));
	wFTSelect->setRelativePosition(ResizeWin(550, 240, 780, 340));
	wMessage->setRelativePosition(ResizeWin(490, 200, 840, 340));
	wACMessage->setRelativePosition(ResizeWin(490, 240, 840, 300));
	wQuery->setRelativePosition(ResizeWin(490, 200, 840, 340));
	wOptions->setRelativePosition(ResizeWin(490, 200, 840, 340));
	wPosSelect->setRelativePosition(ResizeWin(340, 200, 935, 410));
	wCardSelect->setRelativePosition(ResizeWin(320, 100, 1000, 400));
	wANNumber->setRelativePosition(ResizeWin(550, 200, 780, 295));
	wANCard->setRelativePosition(ResizeWin(560, 170, 970, 370));
	wANAttribute->setRelativePosition(ResizeWin(500, 200, 830, 285));
	wANRace->setRelativePosition(ResizeWin(480, 200, 850, 385));
	wReplaySave->setRelativePosition(ResizeWin(510, 200, 820, 320));
	stHintMsg->setRelativePosition(ResizeWin(500, 60, 820, 90));

	wChat->setRelativePosition(ResizeWin(305, 615, 1020, 640, true));
	ebChatInput->setRelativePosition(recti(3, 2, window_size.Width - wChat->getRelativePosition().UpperLeftCorner.X - 6, 22));

	wCardImg->setRelativePosition(Resize(1, 1, 199, 273));
	imgCard->setRelativePosition(Resize(10, 9, 187, 263));
	wInfos->setRelativePosition(Resize(1, 275, 301, 639));
	stName->setRelativePosition(recti(10, 10, 287 * window_size.Width / 1024, 32));
	stInfo->setRelativePosition(recti(15, 37, 296 * window_size.Width / 1024, 60));
	stDataInfo->setRelativePosition(recti(15, 60, 296 * window_size.Width / 1024, 83));
	stText->setRelativePosition(recti(15, 83, 296 * window_size.Width / 1024 - 30, 324 * window_size.Height / 640));
	scrCardText->setRelativePosition(recti(stInfo->getRelativePosition().getWidth() - 20, 83, stInfo->getRelativePosition().getWidth(), 324 * window_size.Height / 640));
	lstLog->setRelativePosition(Resize(10, 10, 290, 290));
	btnClearLog->setRelativePosition(Resize(160, 300, 260, 325));

	btnLeaveGame->setRelativePosition(Resize(205, 5, 295, 80));
	wReplayControl->setRelativePosition(Resize(205, 143, 295, 273));
	btnReplayStart->setRelativePosition(Resize(5, 5, 85, 25));
	btnReplayPause->setRelativePosition(Resize(5, 5, 85, 25));
	btnReplayStep->setRelativePosition(Resize(5, 55, 85, 75));
	btnReplayUndo->setRelativePosition(Resize(5, 80, 85, 100));
	btnReplaySwap->setRelativePosition(Resize(5, 30, 85, 50));
	btnReplayExit->setRelativePosition(Resize(5, 105, 85, 125));

	wPhase->setRelativePosition(Resize(480, 310, 855, 330));
	btnDP->setRelativePosition(Resize(0, 0, 50, 20));
	btnSP->setRelativePosition(Resize(65, 0, 115, 20));
	btnM1->setRelativePosition(Resize(130, 0, 180, 20));
	btnBP->setRelativePosition(Resize(195, 0, 245, 20));
	btnM2->setRelativePosition(Resize(260, 0, 310, 20));
	btnEP->setRelativePosition(Resize(325, 0, 375, 20));
	btnChainAlways->setRelativePosition(Resize(205,140,295,175));
	btnChainIgnore->setRelativePosition(Resize(205,100,295,135));
	btnChainWhenAvail->setRelativePosition(Resize(205,180,295,215));

	if(displayedcard > 0)
		ShowCardInfo(displayedcard);
}
recti Game::Resize(s32 x, s32 y, s32 x2, s32 y2)
{
	x = x * window_size.Width / 1024;
	y = y * window_size.Height / 640;
	x2 = x2 * window_size.Width / 1024;
	y2 = y2 * window_size.Height / 640;
	return recti(x, y, x2, y2);
}
recti Game::Resize(s32 x, s32 y, s32 x2, s32 y2, s32 dx, s32 dy, s32 dx2, s32 dy2)
{
	x = x * window_size.Width / 1024 + dx;
	y = y * window_size.Height / 640 + dy;
	x2 = x2 * window_size.Width / 1024 + dx2;
	y2 = y2 * window_size.Height / 640 + dy2;
	return recti(x, y, x2, y2);
}
position2di Game::Resize(s32 x, s32 y, bool reverse)
{
	if (reverse)
	{
		x = x * 1024 / window_size.Width;
		y = y * 640 / window_size.Height;
	}
	else
	{
		x = x * window_size.Width / 1024;
		y = y * window_size.Height / 640;
	}
	return position2di(x, y);
}
recti Game::ResizeWin(s32 x, s32 y, s32 x2, s32 y2, bool chat)
{
	s32 sx = x2 - x;
	s32 sy = y2 - y;
	if (chat)
	{
		y = window_size.Height - sy;
		x2 = window_size.Width;
		y2 = y + sy;
		return recti(x, y, x2, y2);
	}
	x = (x + sx / 2) * window_size.Width / 1024 - sx / 2;
	y = (y + sy / 2) * window_size.Height / 640 - sy / 2;
	x2 = sx + x;
	y2 = sy + y;
	return recti(x, y, x2, y2);
}
recti Game::ResizeElem(s32 x, s32 y, s32 x2, s32 y2)
{
	s32 sx = x2 - x;
	s32 sy = y2 - y;
	x = (x + sx / 2 - 100) * window_size.Width / 1024 - sx / 2 + 100;
	y = y * window_size.Height / 640;
	x2 = sx + x;
	y2 = sy + y;
	return recti(x, y, x2, y2);
}
int Game::ExtractColor(const stringw name,CGUISkinSystem *skinSystem, unsigned int normalColor)
{
	// Convert and apply special color
	stringw spccolor = skinSystem->getProperty(name);
	if (!spccolor.empty())
	{
		unsigned int x;
		std::wstringstream ss;
		ss << std::hex << spccolor.c_str();
		ss >> x;
		if (!ss.fail())
			return x;
	}
	return normalColor;
}

}
