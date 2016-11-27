#include "GUIInfoTab.h"
#include "game.h"
#include "image_manager.h"

namespace ygo {

	void GUIInfoTab::Load() {
		IGUIEnvironment* env = mainGame->env;
		_staticText[STATICTEXT_CARDIMAGE] = env->addStaticText(L"", rect<s32>(1, 1, 199, 273), true, false, 0, STATICTEXT_CARDIMAGE, true);
		_staticText[STATICTEXT_CARDIMAGE]->setBackgroundColor(0xc0c0c0c0);
		_staticText[STATICTEXT_CARDIMAGE]->setVisible(false);
		imgCard = env->addImage(rect<s32>(10, 9, 187, 263), _staticText[STATICTEXT_CARDIMAGE]);
		imgCard->setUseAlphaChannel(true);

		wInfos = env->addTabControl(rect<s32>(1, 275, 301, 639), 0, true);
		wInfos->setVisible(false);
		//info
		irr::gui::IGUITab* tabInfo = wInfos->addTab(dataManager.GetSysString(1270));
		_staticText[STATICTEXT_CARDNAME] = env->addStaticText(L"", rect<s32>(10, 10, 287, 32), true, false, tabInfo, STATICTEXT_CARDNAME, false);
		_staticText[STATICTEXT_CARDNAME]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		_staticText[STATICTEXT_CARDINFO] = env->addStaticText(L"", rect<s32>(15, 37, 296, 60), false, true, tabInfo, STATICTEXT_CARDINFO, false);
		_staticText[STATICTEXT_CARDINFO]->setOverrideColor(mainGame->special_color);
		_staticText[STATICTEXT_CARDDATA] = env->addStaticText(L"", rect<s32>(15, 60, 296, 83), false, true, tabInfo, STATICTEXT_CARDDATA, false);
		_staticText[STATICTEXT_CARDDATA]->setOverrideColor(mainGame->special_color);
		_staticText[STATICTEXT_CARDSETNAME] = env->addStaticText(L"", rect<s32>(15, 83, 316, 106), false, true, tabInfo, STATICTEXT_CARDSETNAME, false);
		_staticText[STATICTEXT_CARDSETNAME]->setOverrideColor(mainGame->setcolor);
		_staticText[STATICTEXT_CARDTEXT] = env->addStaticText(L"", rect<s32>(15, 83, 287, 324), false, true, tabInfo, STATICTEXT_CARDTEXT, false);
		scrCardText = env->addScrollBar(false, rect<s32>(267, 83, 287, 324), tabInfo, SCROLL_CARDTEXT);
		scrCardText->setLargeStep(1);
		scrCardText->setSmallStep(1);
		scrCardText->setVisible(false);
		//log
		irr::gui::IGUITab* tabLog = wInfos->addTab(dataManager.GetSysString(1271));
		lstLog = env->addListBox(rect<s32>(10, 10, 290, 290), tabLog, LISTBOX_LOG, false);
		lstLog->setItemHeight(18);
		_buttons[BUTTON_CLEAR_LOG] = env->addButton(rect<s32>(160, 300, 260, 325), tabLog, BUTTON_CLEAR_LOG, dataManager.GetSysString(1272));
		//system
		irr::gui::IGUITab* tabSystem = wInfos->addTab(dataManager.GetSysString(1273));
		_checkBox[CHECKBOX_AUTOPOS] = env->addCheckBox(mainGame->gameConf.autoplace, rect<s32>(20, 20, 280, 45), tabSystem, CHECKBOX_AUTOPOS, dataManager.GetSysString(1274));
		_checkBox[CHECKBOX_RANDOMPOS] = env->addCheckBox(mainGame->gameConf.randomplace, rect<s32>(40, 50, 300, 75), tabSystem, CHECKBOX_RANDOMPOS, dataManager.GetSysString(1275));
		_checkBox[CHECKBOX_AUTOCHAIN] = env->addCheckBox(mainGame->gameConf.autochain, rect<s32>(20, 80, 280, 105), tabSystem, CHECKBOX_AUTOCHAIN, dataManager.GetSysString(1276));
		_checkBox[CHECKBOX_WAITCHAIN] = env->addCheckBox(false, rect<s32>(20, 110, 280, 135), tabSystem, CHECKBOX_WAITCHAIN, dataManager.GetSysString(1277));
		_checkBox[CHECKBOX_MUTEOPPONENT] = env->addCheckBox(mainGame->gameConf.muteopponent, rect<s32>(20, 170, 280, 195), tabSystem, CHECKBOX_MUTEOPPONENT, dataManager.GetSysString(1290));
		_checkBox[CHECKBOX_MUTESPECTATORS] = env->addCheckBox(mainGame->gameConf.mutespectator, rect<s32>(20, 200, 280, 225), tabSystem, CHECKBOX_MUTESPECTATORS, dataManager.GetSysString(1291));
		_checkBox[CHECKBOX_HIDESETNAMES] = env->addCheckBox(false, rect<s32>(20, 140, 280, 165), tabSystem, CHECKBOX_HIDESETNAMES, dataManager.GetSysString(1354));
		_checkBox[CHECKBOX_HIDESETNAMES]->setChecked(mainGame->gameConf.chkHideSetname != 0);
		_checkBox[CHECKBOX_HIDE_CHAINBUTTONS] = env->addCheckBox(false, rect<s32>(150, 80, 410, 105), tabSystem, CHECKBOX_HIDE_CHAINBUTTONS, dataManager.GetSysString(1355));
		_checkBox[CHECKBOX_HIDE_CHAINBUTTONS]->setChecked(mainGame->gameConf.chkHideChainButton != 0);
		_checkBox[CHECKBOX_HIDE_HINTBUTTON] = env->addCheckBox(false, rect<s32>(150, 110, 410, 135), tabSystem, CHECKBOX_HIDE_HINTBUTTON, dataManager.GetSysString(1356));
		_checkBox[CHECKBOX_HIDE_HINTBUTTON]->setChecked(mainGame->gameConf.chkHideHintButton != 0);
		_checkBox[CHECKBOX_ENABLE_SOUND] = env->addCheckBox(mainGame->gameConf.enablesound, rect<s32>(20, 230, 280, 255), tabSystem, CHECKBOX_ENABLE_SOUND, dataManager.GetSysString(2046));
		scrSound = env->addScrollBar(true, rect<s32>(20, 260, 280, 270), tabSystem, SCROLL_SOUND);
		scrSound->setMax(100);
		scrSound->setMin(0);
		scrSound->setPos(mainGame->gameConf.soundvolume * 100);
		scrSound->setLargeStep(1);
		scrSound->setSmallStep(1);
		_checkBox[CHECKBOX_ENABLE_MUSIC] = env->addCheckBox(mainGame->gameConf.enablemusic, rect<s32>(20, 275, 280, 300), tabSystem, CHECKBOX_ENABLE_MUSIC, dataManager.GetSysString(2047));
		scrMusic = env->addScrollBar(true, rect<s32>(20, 305, 280, 315), tabSystem, SCROLL_MUSIC);
		scrMusic->setMax(100);
		scrMusic->setMin(0);
		scrMusic->setPos(mainGame->gameConf.musicvolume * 100);
		scrMusic->setLargeStep(1);
		scrMusic->setSmallStep(1);
	}

	void GUIInfoTab::ShowCardInfo(int code) {
		CardData cd;
		wchar_t formatBuffer[256];
		displayedcard = code;
		if (!dataManager.GetData(code, &cd))
			memset(&cd, 0, sizeof(CardData));
		SetImage(code);
		if (cd.alias != 0 && (cd.alias - code < 10 || code - cd.alias < 10))
			myswprintf(formatBuffer, L"%ls[%08d]", dataManager.GetName(cd.alias), cd.alias);
		else myswprintf(formatBuffer, L"%ls[%08d]", dataManager.GetName(code), code);
		_staticText[STATICTEXT_CARDNAME]->setText(formatBuffer);
		int offset = 0;
		if (!_checkBox[CHECKBOX_HIDESETNAMES]->isChecked()) {
			unsigned long long sc = cd.setcode;
			if (cd.alias) {
				auto aptr = dataManager._datas.find(cd.alias);
				if (aptr != dataManager._datas.end())
					sc = aptr->second.setcode;
			}
			if (sc) {
				offset = 30;
				myswprintf(formatBuffer, L"%ls%ls", dataManager.GetSysString(1329), dataManager.FormatSetName(sc));
				_staticText[STATICTEXT_CARDSETNAME]->setText(formatBuffer);
			}
			else
				_staticText[STATICTEXT_CARDSETNAME]->setText(L"");
		}
		else {
			_staticText[STATICTEXT_CARDSETNAME]->setText(L"");
		}
		if (cd.type & TYPE_MONSTER) {
			myswprintf(formatBuffer, L"[%ls] %ls/%ls", dataManager.FormatType(cd.type), dataManager.FormatRace(cd.race), dataManager.FormatAttribute(cd.attribute));
			_staticText[STATICTEXT_CARDINFO]->setText(formatBuffer);
			int form = 0x2605;
			if (cd.type & TYPE_XYZ) ++form;
			myswprintf(formatBuffer, L"[%c%d] ", form, cd.level);
			wchar_t adBuffer[16];
			if (cd.attack < 0 && cd.defence < 0)
				myswprintf(adBuffer, L"?/?");
			else if (cd.attack < 0)
				myswprintf(adBuffer, L"?/%d", cd.defence);
			else if (cd.defence < 0)
				myswprintf(adBuffer, L"%d/?", cd.attack);
			else
				myswprintf(adBuffer, L"%d/%d", cd.attack, cd.defence);
			wcscat(formatBuffer, adBuffer);
			if (cd.type & TYPE_PENDULUM) {
				wchar_t scaleBuffer[16];
				myswprintf(scaleBuffer, L" %d/%d", cd.lscale, cd.rscale);
				wcscat(formatBuffer, scaleBuffer);
			}
			_staticText[STATICTEXT_CARDDATA]->setText(formatBuffer);
			_staticText[STATICTEXT_CARDSETNAME]->setRelativePosition(rect<s32>(15, 83, 316 * mainGame->window_size.Width / 1024 - 30, 116));
			_staticText[STATICTEXT_CARDTEXT]->setRelativePosition(recti(15, 83 + offset, 296 * mainGame->window_size.Width / 1024 - 30, 324 * mainGame->window_size.Height / 640));
			scrCardText->setRelativePosition(rect<s32>(267, 83 + offset, 287, 324 * mainGame->window_size.Height / 640));
		}
		else {
			myswprintf(formatBuffer, L"[%ls]", dataManager.FormatType(cd.type));
			_staticText[STATICTEXT_CARDINFO]->setText(formatBuffer);
			_staticText[STATICTEXT_CARDDATA]->setText(L"");
			_staticText[STATICTEXT_CARDSETNAME]->setRelativePosition(rect<s32>(15, 60, 316 * mainGame->window_size.Height / 640, 83));
			_staticText[STATICTEXT_CARDTEXT]->setRelativePosition(rect<s32>(15, 60 + offset, 287 * mainGame->window_size.Width / 1024 - 30, 324 * mainGame->window_size.Height / 640));
			scrCardText->setRelativePosition(rect<s32>(267, 60 + offset, 287, 324 * mainGame->window_size.Height / 640));
		}
		showingtext = dataManager.GetText(code);
		const auto& tsize = _staticText[STATICTEXT_CARDTEXT]->getRelativePosition();
		mainGame->InitStaticText(_staticText[STATICTEXT_CARDTEXT], tsize.getWidth() - 30, tsize.getHeight(), mainGame->textFont, showingtext);
	}

	void GUIInfoTab::LogAddItem(wchar_t* text) {
		lstLog->addItem(text);
	}

	void GUIInfoTab::SetImage(int code) {
		if (code > 0) {
			imgCard->setImage(imageManager.GetTexture(code));
			imgCard->setScaleImage(true);
		}
		else
			imgCard->setImage(0);
	}

	void GUIInfoTab::SetImage(irr::video::ITexture* texture) {
		imgCard->setImage(texture);
	}

	void GUIInfoTab::ShowImage(bool value) {
		_staticText[STATICTEXT_CARDIMAGE]->setVisible(value);
	}

	void GUIInfoTab::OnResize() {
		_staticText[STATICTEXT_CARDIMAGE]->setRelativePosition(mainGame->Resize(1, 1, 199, 273));
		imgCard->setRelativePosition(mainGame->Resize(10, 9, 187, 263));

		wInfos->setRelativePosition(mainGame->Resize(1, 275, 301, 639));
		_staticText[STATICTEXT_CARDNAME]->setRelativePosition(recti(10, 10, 287 * mainGame->window_size.Width / 1024, 32));
		_staticText[STATICTEXT_CARDINFO]->setRelativePosition(recti(15, 37, 296 * mainGame->window_size.Width / 1024, 60));
		_staticText[STATICTEXT_CARDDATA]->setRelativePosition(recti(15, 60, 296 * mainGame->window_size.Width / 1024, 83));
		_staticText[STATICTEXT_CARDTEXT]->setRelativePosition(recti(15, 83, 296 * mainGame->window_size.Width / 1024 - 30, 324 * mainGame->window_size.Height / 640));
		scrCardText->setRelativePosition(recti(_staticText[STATICTEXT_CARDINFO]->getRelativePosition().getWidth() - 20, 83,
			_staticText[STATICTEXT_CARDINFO]->getRelativePosition().getWidth(), 324 * mainGame->window_size.Height / 640));
		lstLog->setRelativePosition(mainGame->Resize(10, 10, 290, 290));
		_buttons[BUTTON_CLEAR_LOG]->setRelativePosition(mainGame->Resize(160, 300, 260, 325));

		if (displayedcard > 0)
			ShowCardInfo(displayedcard);
	}
	void GUIInfoTab::Show() {
		ShowImage(true);
		wInfos->setVisible(true);
		SetImage(imageManager.tCover[0]);
	}

	void GUIInfoTab::Hide() {
		wInfos->setVisible(false);
		ShowImage(false);
		ClearText();
		ClearLog();
	}

	void GUIInfoTab::SetActiveTab(int index) {
		wInfos->setActiveTab(index);
	}

	int GUIInfoTab::GetLogSelectedIndex() {
		return lstLog->getSelected();
	}

	void GUIInfoTab::ClearText() {
		_staticText[STATICTEXT_CARDNAME]->setText(L"");
		_staticText[STATICTEXT_CARDINFO]->setText(L"");
		_staticText[STATICTEXT_CARDDATA]->setText(L"");
		_staticText[STATICTEXT_CARDSETNAME]->setText(L"");
		_staticText[STATICTEXT_CARDTEXT]->setText(L"");
		scrCardText->setVisible(false);
	}

	void GUIInfoTab::ClearLog() {
		lstLog->clear();
	}

	void GUIInfoTab::EnableScrollBar(bool value, int step) {
		scrCardText->setVisible(value);
		if (value) {
			scrCardText->setMin(0);
			scrCardText->setMax(step);
			scrCardText->setPos(0);
		}
	}

	bool GUIInfoTab::IsChecked(unsigned int item) {
		auto cdit = _checkBox.find(item);
		if (cdit == _checkBox.end())
			return false;
		return _checkBox[item]->isChecked();
	}

	void GUIInfoTab::SetChecked(unsigned int item, bool value) {
		auto cdit = _checkBox.find(item);
		if (cdit == _checkBox.end())
			return;
		_checkBox[item]->setChecked(value);
	}

	bool GUIInfoTab::OnEvent(const irr::SEvent& event) {
		//handle events here
		switch (event.EventType) {
		case irr::EET_GUI_EVENT: {
			s32 id = event.GUIEvent.Caller->getID();
			switch (event.GUIEvent.EventType) {
			case irr::gui::EGET_BUTTON_CLICKED: {
				switch (id) {
				case BUTTON_CLEAR_LOG: {
					mainGame->wInfoTab.ClearLog();
					mainGame->logParam.clear();
					break;
				}
				}
			case irr::gui::EGET_SCROLL_BAR_CHANGED: {
				switch (id) {
				case SCROLL_CARDTEXT: {
					u32 pos = scrCardText->getPos();
					mainGame->SetStaticText(_staticText[STATICTEXT_CARDTEXT], _staticText[STATICTEXT_CARDTEXT]->getRelativePosition().getWidth() - 30, mainGame->textFont, showingtext, pos);
					break;
				}
				case SCROLL_SOUND: {
					mainGame->gameConf.soundvolume = (double)scrSound->getPos() / 100;
					mainGame->engineSound->setSoundVolume(mainGame->gameConf.soundvolume);
					break;
				}
				case SCROLL_MUSIC: {
					mainGame->gameConf.musicvolume = (double)scrMusic->getPos() / 100;
					mainGame->engineMusic->setSoundVolume(mainGame->gameConf.musicvolume);
					break;
				}
								   break;
				}
			}
			case irr::gui::EGET_CHECKBOX_CHANGED: {
				switch (id) {
				case CHECKBOX_ENABLE_SOUND: {
					mainGame->gameConf.enablesound = mainGame->wInfoTab.IsChecked(CHECKBOX_ENABLE_SOUND);
					break;
				}
				case CHECKBOX_ENABLE_MUSIC: {
					mainGame->gameConf.enablemusic = mainGame->wInfoTab.IsChecked(CHECKBOX_ENABLE_MUSIC);
					if (!mainGame->gameConf.enablemusic)
						mainGame->engineMusic->stopAllSounds();
					break;
				}
				case CHECKBOX_ENABLE_HIDECHAIN: {
					mainGame->gameConf.chkHideChainButton = mainGame->wInfoTab.IsChecked(CHECKBOX_HIDE_CHAINBUTTONS);
					if (!mainGame->is_building) {
						if (!mainGame->gameConf.chkHideChainButton) {
							mainGame->btnChainAlways->setVisible(true);
							mainGame->btnChainIgnore->setVisible(true);
							mainGame->btnChainWhenAvail->setVisible(true);
						}
						else {
							mainGame->btnChainAlways->setVisible(false);
							mainGame->btnChainIgnore->setVisible(false);
							mainGame->btnChainWhenAvail->setVisible(false);
						}
					}
				}
				}
				break;
			}
			break;
			}
			}
			break;
		}
		}
		return false;
	}
}
