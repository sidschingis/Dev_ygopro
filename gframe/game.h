#ifndef GAME_H
#define GAME_H

#include "config.h"
#include "client_field.h"
#include "GUIMainMenu.h"
#include "GUIOnline.h"
#include "GUILanWindow.h"
#include "GUIDeckEdit.h"
#include "GUICreateHost.h"
#include "GUIReplayList.h"
#include "GUISingleList.h"
#include "GUIHostRoom.h"
#include "GUIInfoTab.h"
#include "GUIChat.h"
#include "GUIAIMode.h"
#include <unordered_map>
#include <vector>
#include <list>
#include <irrKlang.h>
#pragma comment(lib, "irrKlang.lib")
#include "CGUISkinSystem/CGUISkinSystem.h"

namespace ygo {

struct Config {
	bool use_d3d;
	unsigned short antialias;
	unsigned short serverport;
	unsigned char textfontsize;
	wchar_t lastip[20];
	wchar_t lastport[10];
	wchar_t nickname[20];
	wchar_t gamename[20];
	wchar_t lastdeck[64];
	wchar_t textfont[256];
	wchar_t numfont[256];
	wchar_t roompass[30];
	wchar_t lastreplay[256];
	wchar_t lastpuzzle[256];
	bool autoplace;
	bool randomplace;
	bool autochain;
	bool nodelay;
	bool chkHideSetname;
	bool chkHideChainButton;
	bool enablesound;
	double soundvolume;
	bool enablemusic;
	double musicvolume;
	int skin_index;
	bool fullscreen;
	bool enablesleeveloading;
	bool muteopponent;
	bool mutespectator;
	bool forced;
	bool savereplay;
	int control_mode;
	bool old_replay_mode;
	wchar_t botname[20];
};

struct DuelInfo {
	bool isStarted;
	bool isReplay;
	bool isYRP2;
	bool isReplaySkiping;
	bool isFirst;
	bool isTag;
	bool isSingleMode;
	bool is_shuffling;
	bool tag_player[2];
	int lp[2];
	int turn;
	short curMsg;
	wchar_t hostname[20];
	wchar_t clientname[20];
	wchar_t hostname_tag[20];
	wchar_t clientname_tag[20];
	wchar_t strLP[2][16];
	wchar_t strTurn[8];
	wchar_t* vic_string;
	unsigned char player_type;
	unsigned char time_player;
	unsigned short time_limit;
	unsigned short time_left[2];
};

struct FadingUnit {
	bool signalAction;
	bool isFadein;
	int fadingFrame;
	int autoFadeoutFrame;
	irr::gui::IGUIElement* guiFading;
	irr::core::recti fadingSize;
	irr::core::vector2di fadingUL;
	irr::core::vector2di fadingLR;
	irr::core::vector2di fadingDiff;
};

class Game {

public:
	bool Initialize();
	void MainLoop();
	void BuildProjectionMatrix(irr::core::matrix4& mProjection, f32 left, f32 right, f32 bottom, f32 top, f32 znear, f32 zfar);
	void InitStaticText(irr::gui::IGUIStaticText* pControl, u32 cWidth, u32 cHeight, irr::gui::CGUITTFont* font, const wchar_t* text);
    void SetStaticText(irr::gui::IGUIStaticText* pControl, u32 cWidth, irr::gui::CGUITTFont* font, const wchar_t* text, u32 pos = 0);
	void RefreshReplay();
	void RefreshSingleplay();
	void DrawSelectionLine(irr::video::S3DVertex* vec, bool strip, int width, float* cv);
	void DrawBackGround();
	void DrawCards();
	void DrawCard(ClientCard* pcard);
	void DrawMisc();
	void DrawGUI();
	void DrawSpec();
	void ShowElement(irr::gui::IGUIElement* element, int autoframe = 0);
	void HideElement(irr::gui::IGUIElement* element, bool set_action = false);
	void PopupElement(irr::gui::IGUIElement* element, int hideframe = 0);
	void WaitFrameSignal(int frame);
	void DrawThumb(code_pointer cp, position2di pos, std::unordered_map<int, int>* lflist, bool drag = false);
	void DrawDeckBd();
	void LoadConfig();
	void AddChatMsg(wchar_t* msg, int player);
	void ClearTextures();
	void CloseDuelWindow();
	void PlayMusic(char* song, bool loop);
	void PlaySound(char* sound);
	void DrawRectangle(IVideoDriver *driver, recti position);
	void DrawShadowA(CGUITTFont *font, const stringw &text, recti position);
	void DrawShadowB(CGUITTFont *font, const stringw &text, recti position);

	int LocalPlayer(int player);
	const wchar_t* LocalName(int local_player);	
	
	bool HasFocus(EGUI_ELEMENT_TYPE type) const {
		irr::gui::IGUIElement* focus = env->getFocus();
		return focus && focus->hasType(type);
	}

	void OnResize();
	recti Resize(s32 x, s32 y, s32 x2, s32 y2);
	recti Resize(s32 x, s32 y, s32 x2, s32 y2, s32 dx, s32 dy, s32 dx2, s32 dy2);
	position2di Resize(s32 x, s32 y, bool reverse = false);
	recti ResizeWin(s32 x, s32 y, s32 x2, s32 y2, bool chat = false);
	recti ResizeElem(s32 x, s32 y, s32 x2, s32 y2);
	int ExtractColor(const stringw name, CGUISkinSystem *skinSystem, unsigned int normalColor);

	Mutex gMutex;
	Mutex gBuffer;
	Signal frameSignal;
	Signal actionSignal;
	Signal replaySignal;
	Signal singleSignal;
	Signal closeSignal;
	Signal closeDoneSignal;
	Config gameConf;
	DuelInfo dInfo;

	std::list<FadingUnit> fadingList;
	std::vector<int> logParam;
	std::wstring chatMsg[8];

	int hideChatTimer;
	bool hideChat;
	int chatTiming[8];
	int chatType[8];
	unsigned short linePattern;
	int waitFrame;
	int signalFrame;
	int actionParam;
	int showcard;
	int showcardcode;
	int showcarddif;
	int showcardp;
	int is_attacking;
	int attack_sv;
	irr::core::vector3df atk_r;
	irr::core::vector3df atk_t;
	float atkdy;
	int lpframe;
	int lpd;
	int lpplayer;
	int lpccolor;
	int turncolor;
	int playerlpcolor;
	int extracolor;
	int statcolor;
	int bonuscolor;
	int negativecolor;
	int setcolor;
	int special_color;
	wchar_t* lpcstring;
	bool always_chain;
	bool ignore_chain;
	bool chain_when_avail;

	bool is_building;
	bool is_siding;
	bool is_aimode;
	int displayedcard;

	irr::core::dimension2d<irr::u32> window_size;

	CGUISkinSystem *skinSystem;

	ClientField dField;
	irr::IrrlichtDevice* device;
	irr::video::IVideoDriver* driver;
	irr::scene::ISceneManager* smgr;
	irr::scene::ICameraSceneNode* camera;
	//GUI
	irr::gui::IGUIEnvironment* env;
	irr::gui::CGUITTFont* guiFont;
	irr::gui::CGUITTFont* textFont;
	irr::gui::CGUITTFont* numFont;
	irr::gui::CGUITTFont* adFont;
	irr::gui::CGUITTFont* lpcFont;
	std::map<irr::gui::CGUIImageButton*, int> imageLoading;
	//card image
	//hint text
	irr::gui::IGUIStaticText* stHintMsg;
	irr::gui::IGUIStaticText* stTip;
	//infos
	//create host
	//host panel
	//replay
	//single play
	//hand
	irr::gui::IGUIWindow* wHand;
	irr::gui::IGUIButton* btnHand[3];
	//
	irr::gui::IGUIWindow* wFTSelect;
	irr::gui::IGUIButton* btnFirst;
	irr::gui::IGUIButton* btnSecond;
	//message
	irr::gui::IGUIWindow* wMessage;
	irr::gui::IGUIStaticText* stMessage;
	irr::gui::IGUIButton* btnMsgOK;
	//card display
	irr::gui::IGUIWindow* wCardDisplay;
	irr::gui::CGUIImageButton* btnCardDisplay[5];
	irr::gui::IGUIStaticText *stDisplayPos[5];
	irr::gui::IGUIScrollBar *scrDisplayList;
	irr::gui::IGUIButton* btnDisplayOK;
	//auto close message
	irr::gui::IGUIWindow* wACMessage;
	irr::gui::IGUIStaticText* stACMessage;
	//yes/no
	irr::gui::IGUIWindow* wQuery;
	irr::gui::IGUIStaticText* stQMessage;
	irr::gui::IGUIButton* btnYes;
	irr::gui::IGUIButton* btnNo;
	//options
	irr::gui::IGUIWindow* wOptions;
	irr::gui::IGUIStaticText* stOptions;
	irr::gui::IGUIButton* btnOptionp;
	irr::gui::IGUIButton* btnOptionn;
	irr::gui::IGUIButton* btnOptionOK;
	//pos selection
	irr::gui::IGUIWindow* wPosSelect;
	irr::gui::CGUIImageButton* btnPSAU;
	irr::gui::CGUIImageButton* btnPSAD;
	irr::gui::CGUIImageButton* btnPSDU;
	irr::gui::CGUIImageButton* btnPSDD;
	//card selection
	irr::gui::IGUIWindow* wCardSelect;
	irr::gui::CGUIImageButton* btnCardSelect[5];
	irr::gui::IGUIStaticText *stCardPos[5];
	irr::gui::IGUIScrollBar *scrCardList;
	irr::gui::IGUIButton* btnSelectOK;
	//announce number
	irr::gui::IGUIWindow* wANNumber;
	irr::gui::IGUIComboBox* cbANNumber;
	irr::gui::IGUIButton* btnANNumberOK;
	//announce card
	irr::gui::IGUIWindow* wANCard;
	irr::gui::IGUIEditBox* ebANCard;
	irr::gui::IGUIListBox* lstANCard;
	irr::gui::IGUIButton* btnANCardOK;
	//announce attribute
	irr::gui::IGUIWindow* wANAttribute;
	irr::gui::IGUICheckBox* chkAttribute[7];
	//announce race
	irr::gui::IGUIWindow* wANRace;
	irr::gui::IGUICheckBox* chkRace[24];
	//cmd menu
	irr::gui::IGUIWindow* wCmdMenu;
	irr::gui::IGUIButton* btnActivate;
	irr::gui::IGUIButton* btnSummon;
	irr::gui::IGUIButton* btnSPSummon;
	irr::gui::IGUIButton* btnMSet;
	irr::gui::IGUIButton* btnSSet;
	irr::gui::IGUIButton* btnRepos;
	irr::gui::IGUIButton* btnAttack;
	irr::gui::IGUIButton* btnShowList;
	irr::gui::IGUIButton* btnOperation;
	irr::gui::IGUIButton* btnReset;
	irr::gui::IGUIButton* btnShuffle;
	//chat window
	//phase button
	irr::gui::IGUIStaticText* wPhase;
	irr::gui::IGUIButton* btnDP;
	irr::gui::IGUIButton* btnSP;
	irr::gui::IGUIButton* btnM1;
	irr::gui::IGUIButton* btnBP;
	irr::gui::IGUIButton* btnM2;
	irr::gui::IGUIButton* btnEP;
	//replay save
	irr::gui::IGUIWindow* wReplaySave;
	irr::gui::IGUIEditBox* ebRSName;
	irr::gui::IGUIButton* btnRSYes;
	irr::gui::IGUIButton* btnRSNo;
	//replay control
	irr::gui::IGUIStaticText* wReplayControl;
	irr::gui::IGUIButton* btnReplayStart;
	irr::gui::IGUIButton* btnReplayPause;
	irr::gui::IGUIButton* btnReplayStep;
	irr::gui::IGUIButton* btnReplayUndo;
	irr::gui::IGUIButton* btnReplayExit;
	irr::gui::IGUIButton* btnReplaySwap;
	//surrender/leave
	irr::gui::IGUIButton* btnLeaveGame;
	//chain control
	irr::gui::IGUIButton* btnChainIgnore;
	irr::gui::IGUIButton* btnChainAlways;
	irr::gui::IGUIButton* btnChainWhenAvail;
	//cancel or finish
	irr::gui::IGUIButton* btnCancelOrFinish;
	//soundEngine
	irrklang::ISoundEngine* engineSound;
	irrklang::ISoundEngine* engineMusic;

	GUIMainMenu wMenu;
	GUIDeckEdit wEdit;
	GUIOnline wOnline;
	GUILanWindow wLan;
	GUICreateHost wHost;
	GUIReplayList wReplayList;
	GUISingleList wSingleList;
	GUIHostRoom wHostRoom;
	GUIInfoTab wInfoTab;
	GUIChat wChat;
	GUIAIMode wAI;
};

extern Game* mainGame;

}

#define UEVENT_EXIT			0x1
#define UEVENT_TOWINDOW		0x2

#define COMMAND_ACTIVATE	0x0001
#define COMMAND_SUMMON		0x0002
#define COMMAND_SPSUMMON	0x0004
#define COMMAND_MSET		0x0008
#define COMMAND_SSET		0x0010
#define COMMAND_REPOS		0x0020
#define COMMAND_ATTACK		0x0040
#define COMMAND_LIST		0x0080
#define COMMAND_OPERATION	0x0100
#define COMMAND_RESET		0x0200

#define POSITION_HINT		0x8000

#define BUTTON_LAN_MODE				100
#define BUTTON_SINGLE_MODE			101
#define BUTTON_REPLAY_MODE			102
#define BUTTON_TEST_MODE			103
#define BUTTON_DECK_EDIT			104
#define BUTTON_MODE_EXIT			105
#define BUTTON_AI_MODE				106
#define LISTBOX_LAN_HOST			110
#define BUTTON_JOIN_HOST			111
#define BUTTON_JOIN_CANCEL			112
#define BUTTON_CREATE_HOST			113
#define BUTTON_HOST_CONFIRM			114
#define BUTTON_HOST_CANCEL			115
#define BUTTON_LAN_REFRESH			116
#define BUTTON_HP_DUELIST			120
#define BUTTON_HP_OBSERVER			121
#define BUTTON_HP_START				122
#define BUTTON_HP_CANCEL			123
#define BUTTON_HP_KICK				124
#define CHECKBOX_HP_READY			125
#define LISTBOX_REPLAY_LIST			130
#define BUTTON_LOAD_REPLAY			131
#define BUTTON_CANCEL_REPLAY		132
#define EDITBOX_CHAT				140
#define BUTTON_MSG_OK				200
#define BUTTON_YES					201
#define BUTTON_NO					202
#define BUTTON_HAND1				205
#define BUTTON_HAND2				206
#define BUTTON_HAND3				207
#define BUTTON_FIRST				208
#define BUTTON_SECOND				209
#define BUTTON_POS_AU				210
#define BUTTON_POS_AD				211
#define BUTTON_POS_DU				212
#define BUTTON_POS_DD				213
#define BUTTON_OPTION_PREV			220
#define BUTTON_OPTION_NEXT			221
#define BUTTON_OPTION_OK			222
#define BUTTON_CARD_0				230
#define BUTTON_CARD_1				231
#define BUTTON_CARD_2				232
#define BUTTON_CARD_3				233
#define BUTTON_CARD_4				234
#define SCROLL_CARD_SELECT			235
#define BUTTON_CARD_SEL_OK			236
#define BUTTON_CMD_ACTIVATE			240
#define BUTTON_CMD_SUMMON			241
#define BUTTON_CMD_SPSUMMON			242
#define BUTTON_CMD_MSET				243
#define BUTTON_CMD_SSET				244
#define BUTTON_CMD_REPOS			245
#define BUTTON_CMD_ATTACK			246
#define BUTTON_CMD_SHOWLIST			247
#define BUTTON_CMD_SHUFFLE			248
#define BUTTON_CMD_RESET			249
#define BUTTON_ANNUMBER_OK			250
#define BUTTON_ANCARD_OK			251
#define EDITBOX_ANCARD				252
#define LISTBOX_ANCARD				253
#define CHECK_ATTRIBUTE				254
#define CHECK_RACE					255
#define BUTTON_BP					260
#define BUTTON_M2					261
#define BUTTON_EP					262
#define BUTTON_LEAVE_GAME			263
#define BUTTON_CHAIN_IGNORE			264
#define BUTTON_CHAIN_ALWAYS			265
#define BUTTON_CHAIN_WHENAVAIL		266
#define BUTTON_CANCEL_OR_FINISH		267
#define BUTTON_CLEAR_LOG			270
#define LISTBOX_LOG					271
#define SCROLL_CARDTEXT				280
#define SCROLL_SOUND				281
#define SCROLL_MUSIC				282
#define BUTTON_DISPLAY_0			290
#define BUTTON_DISPLAY_1			291
#define BUTTON_DISPLAY_2			292
#define BUTTON_DISPLAY_3			293
#define BUTTON_DISPLAY_4			294
#define SCROLL_CARD_DISPLAY			295
#define BUTTON_CARD_DISP_OK			296
#define BUTTON_CATEGORY_OK			300
#define BUTTON_CLEAR_DECK			303
#define BUTTON_SAVE_DECK			304
#define BUTTON_SAVE_DECK_AS			305
#define BUTTON_DBEXIT				306
#define BUTTON_SORT_DECK			307
#define BUTTON_SIDE_OK				308
#define BUTTON_SHUFFLE_DECK			309
#define BUTTON_EFFECT_FILTER		311
#define BUTTON_START_FILTER			312
#define BUTTON_REPLAY_START			320
#define BUTTON_REPLAY_PAUSE			321
#define BUTTON_REPLAY_STEP			322
#define BUTTON_REPLAY_UNDO			323
#define BUTTON_REPLAY_EXIT			324
#define BUTTON_REPLAY_SWAP			325
#define BUTTON_REPLAY_SAVE			330
#define BUTTON_REPLAY_CANCEL		331
#define LISTBOX_SINGLEPLAY_LIST		350
#define BUTTON_LOAD_SINGLEPLAY		351
#define BUTTON_CANCEL_SINGLEPLAY	352
#define CHECKBOX_ENABLE_SOUND		353
#define CHECKBOX_ENABLE_MUSIC		354
#define COMBOBOX_SORTTYPE			370
#define BUTTON_CLEAR_FILTER			1001
#define BUTTON_DELETE_DECK			1002
#define CHECKBOX_ENABLE_HIDECHAIN	1003
#endif // GAME_H
