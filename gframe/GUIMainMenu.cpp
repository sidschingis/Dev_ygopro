#include "GUIMainMenu.h"
#include "game.h"
#include "deck_manager.h"

namespace ygo {

	void GUIMainMenu::Load() {
		wchar_t strbuf[256];
		myswprintf(strbuf, L"YGOPro DevPro (Version:%X.0%X.%X)", PRO_VERSION >> 12, (PRO_VERSION >> 4) & 0xff, PRO_VERSION & 0xf);
		wMenu = mainGame->env->addWindow(rect<s32>(370, 200, 650, 415), false, strbuf);
		wMenu->getCloseButton()->setVisible(false);
		_buttons[BUTTON_LAN_MODE] = mainGame->env->addButton(rect<s32>(10, 30, 270, 60), wMenu, BUTTON_LAN_MODE, dataManager.GetSysString(1200));
		_buttons[BUTTON_SINGLE_MODE] = mainGame->env->addButton(rect<s32>(10, 65, 270, 95), wMenu, BUTTON_SINGLE_MODE, dataManager.GetSysString(1201));
		_buttons[BUTTON_REPLAY_MODE] = mainGame->env->addButton(rect<s32>(10, 100, 270, 130), wMenu, BUTTON_REPLAY_MODE, dataManager.GetSysString(1202));
		//_buttons[BUTTON_TEST_MODE]  = env->addButton(rect<s32>(10, 135, 270, 165), wMenu, BUTTON_TEST_MODE, dataManager.GetSysString(1203));
		_buttons[BUTTON_DECK_EDIT] = mainGame->env->addButton(rect<s32>(10, 135, 270, 165), wMenu, BUTTON_DECK_EDIT, dataManager.GetSysString(1204));
		_buttons[BUTTON_MODE_EXIT] = mainGame->env->addButton(rect<s32>(10, 170, 270, 200), wMenu, BUTTON_MODE_EXIT, dataManager.GetSysString(1210));
		
	}

	irr::gui::IGUIElement* GUIMainMenu::GetBtnElement(unsigned int item) {
		auto cdit = _buttons.find(item);
		if (cdit == _buttons.end())
			return NULL;
		return _buttons[item];
	}

	void GUIMainMenu::OnResize() {
		wMenu->setRelativePosition(mainGame->ResizeWin(370, 200, 650, 415));
	}
	void GUIMainMenu::Show() {
		mainGame->PopupElement(wMenu);
		mainGame->device->setEventReceiver(&mainGame->wMenu);
	}

	bool GUIMainMenu::OnEvent(const irr::SEvent& event) {
		//handle events here
		switch (event.EventType) 
		{
			case irr::EET_GUI_EVENT: 
			{
				s32 id = event.GUIEvent.Caller->getID();
				switch (event.GUIEvent.EventType) 
				{
					case irr::gui::EGET_BUTTON_CLICKED: 
					{
						switch (id) 
						{
							case BUTTON_LAN_MODE: {
								mainGame->btnCreateHost->setEnabled(true);
								mainGame->btnJoinHost->setEnabled(true);
								mainGame->btnJoinCancel->setEnabled(true);
								mainGame->wMenu.Hide();
								mainGame->ShowElement(mainGame->wLanWindow);
								mainGame->device->setEventReceiver(&mainGame->menuHandler);//use this for now
								break;
							}
							case BUTTON_REPLAY_MODE: {
								mainGame->wMenu.Hide();
								mainGame->ShowElement(mainGame->wReplay);
								mainGame->ebRepStartTurn->setText(L"1");
								mainGame->RefreshReplay();
								mainGame->device->setEventReceiver(&mainGame->menuHandler);//use this for now
								break;
							}
							case BUTTON_SINGLE_MODE: {
								mainGame->wMenu.Hide();
								mainGame->ShowElement(mainGame->wSinglePlay);
								mainGame->RefreshSingleplay();
								mainGame->device->setEventReceiver(&mainGame->menuHandler);//use this for now
								break;
							}
							case BUTTON_DECK_EDIT: {
								mainGame->RefreshDeck(mainGame->cbDBDecks);
								if (mainGame->cbDBDecks->getSelected() != -1)
									deckManager.LoadDeck(mainGame->cbDBDecks->getItem(mainGame->cbDBDecks->getSelected()));
								mainGame->wMenu.Hide();
								mainGame->is_building = true;
								mainGame->is_siding = false;
								mainGame->wInfos->setVisible(true);
								mainGame->wCardImg->setVisible(true);
								mainGame->wDeckEdit->setVisible(true);
								mainGame->wFilter->setVisible(true);
								mainGame->wSort->setVisible(true);
								mainGame->btnSideOK->setVisible(false);
								mainGame->deckBuilder.filterList = deckManager._lfList[0].content;
								mainGame->cbDBLFList->setSelected(0);
								mainGame->cbCardType->setSelected(0);
								mainGame->cbCardType2->setSelected(0);
								mainGame->cbAttribute->setSelected(0);
								mainGame->cbRace->setSelected(0);
								mainGame->ebAttack->setText(L"");
								mainGame->ebDefence->setText(L"");
								mainGame->ebStar->setText(L"");
								mainGame->ebScale->setText(L"");
								mainGame->cbCardType2->setEnabled(false);
								mainGame->cbAttribute->setEnabled(false);
								mainGame->cbRace->setEnabled(false);
								mainGame->ebAttack->setEnabled(false);
								mainGame->ebDefence->setEnabled(false);
								mainGame->ebStar->setEnabled(false);
								mainGame->ebScale->setEnabled(false);
								mainGame->deckBuilder.filter_effect = 0;
								mainGame->deckBuilder.result_string[0] = L'0';
								mainGame->deckBuilder.result_string[1] = 0;
								mainGame->deckBuilder.results.clear();
								mainGame->deckBuilder.is_draging = false;
								mainGame->device->setEventReceiver(&mainGame->deckBuilder);
								for (int i = 0; i < 32; ++i)
									mainGame->chkCategory[i]->setChecked(false);
								break;
							}
							case BUTTON_MODE_EXIT: {
								mainGame->device->closeDevice();
								break;
							}
							break;
						}
					}
					break;
				}
			}
			break;
		}

		return false;
	}
}
