#include "GUIHostRoom.h"
#include "game.h"
#include "deck_manager.h"
#include "duelclient.h"

#ifdef _WIN32
#include "dirent.h"
#define strcasecmp _stricmp
#endif

namespace ygo {

	void GUIHostRoom::Load() {
		IGUIEnvironment* env = mainGame->env;

		wHostRoom = env->addWindow(rect<s32>(270, 120, 750 + 50, 440), false, dataManager.GetSysString(1250));
		wHostRoom->getCloseButton()->setVisible(false);
		wHostRoom->setVisible(false);
		_buttons[BUTTON_HP_DUELIST] = env->addButton(rect<s32>(10, 30, 110, 55), wHostRoom, BUTTON_HP_DUELIST, dataManager.GetSysString(1251));
		for (int i = 0; i < 2; ++i) {
			stHostPrepDuelist[i] = env->addStaticText(L"", rect<s32>(40, 65 + i * 25, 240, 85 + i * 25), true, false, wHostRoom);
			stHostPrepDuelistElo[i] = env->addStaticText(L"", rect<s32>(250, 65 + i * 25, 240 + 50, 85 + i * 25), true, false, wHostRoom);
			btnHostPrepKick[i] = env->addButton(rect<s32>(10, 65 + i * 25, 30, 85 + i * 25), wHostRoom, BUTTON_HP_KICK, L"X");
			chkHostPrepReady[i] = env->addCheckBox(false, rect<s32>(250 + 50, 65 + i * 25, 270 + 50, 85 + i * 25), wHostRoom, CHECKBOX_HP_READY, L"");
			chkHostPrepReady[i]->setEnabled(false);
		}
		for (int i = 2; i < 4; ++i) {
			stHostPrepDuelist[i] = env->addStaticText(L"", rect<s32>(40, 75 + i * 25, 240, 95 + i * 25), true, false, wHostRoom);
			stHostPrepDuelistElo[i] = env->addStaticText(L"", rect<s32>(250, 65 + i * 25, 240 + 50, 95 + i * 25), true, false, wHostRoom);
			btnHostPrepKick[i] = env->addButton(rect<s32>(10, 75 + i * 25, 30, 95 + i * 25), wHostRoom, BUTTON_HP_KICK, L"X");
			chkHostPrepReady[i] = env->addCheckBox(false, rect<s32>(250 + 50, 75 + i * 25, 270 + 50, 95 + i * 25), wHostRoom, CHECKBOX_HP_READY, L"");
			chkHostPrepReady[i]->setEnabled(false);
		}
		_buttons[BUTTON_HP_OBSERVER] = env->addButton(rect<s32>(10, 180, 110, 205), wHostRoom, BUTTON_HP_OBSERVER, dataManager.GetSysString(1252));
		myswprintf(dataManager.strBuffer, L"%ls%d", dataManager.GetSysString(1253), 0);
		_staticText[STATICTEXT_OB] = env->addStaticText(dataManager.strBuffer, rect<s32>(10, 210, 270, 230), false, false, wHostRoom, STATICTEXT_OB);
		_staticText[STATICTEXT_RULE] = env->addStaticText(L"", rect<s32>(280 + 50, 30, 460 + 50, 230), false, true, wHostRoom, STATICTEXT_RULE);
		env->addStaticText(dataManager.GetSysString(1254), rect<s32>(10, 235, 110, 255), false, false, wHostRoom);
		_deckSelect = env->addComboBox(rect<s32>(120, 230, 270, 255), wHostRoom);
		_buttons[BUTTON_HP_START] = env->addButton(rect<s32>(230 + 50, 280, 340 + 50, 305), wHostRoom, BUTTON_HP_START, dataManager.GetSysString(1215));
		_buttons[BUTTON_HP_CANCEL] = env->addButton(rect<s32>(350 + 50, 280, 460 + 50, 305), wHostRoom, BUTTON_HP_CANCEL, dataManager.GetSysString(1212));
	}

	irr::gui::IGUIElement* GUIHostRoom::GetBtnElement(unsigned int item) {
		auto cdit = _buttons.find(item);
		if (cdit == _buttons.end())
			return NULL;
		return _buttons[item];
	}

	void GUIHostRoom::OnResize() {
		wHostRoom->setRelativePosition(mainGame->ResizeWin(270, 120, 750 + 50, 440));
	}
	void GUIHostRoom::Show() {
		mainGame->PopupElement(wHostRoom);
		mainGame->device->setEventReceiver(&mainGame->wHostRoom);
	}

	void GUIHostRoom::Hide() {
		mainGame->HideElement(wHostRoom);
	}

	void GUIHostRoom::RoomSetUp(bool isTag, std::wstring str) {
		if (isTag) {
			mainGame->dInfo.isTag = true;
			chkHostPrepReady[2]->setVisible(true);
			chkHostPrepReady[3]->setVisible(true);
			stHostPrepDuelist[2]->setVisible(true);
			stHostPrepDuelist[3]->setVisible(true);
			stHostPrepDuelistElo[2]->setVisible(true);
			stHostPrepDuelistElo[3]->setVisible(true);
		}
		else {
			mainGame->dInfo.isTag = false;
			chkHostPrepReady[2]->setVisible(false);
			chkHostPrepReady[3]->setVisible(false);
			stHostPrepDuelist[2]->setVisible(false);
			stHostPrepDuelist[3]->setVisible(false);
			stHostPrepDuelistElo[2]->setVisible(false);
			stHostPrepDuelistElo[3]->setVisible(false);
		}
		for (int i = 0; i < 4; ++i)
			chkHostPrepReady[i]->setChecked(false);

		stHostPrepDuelist[0]->setText(L"");
		stHostPrepDuelist[1]->setText(L"");
		stHostPrepDuelist[2]->setText(L"");
		stHostPrepDuelist[3]->setText(L"");
		_staticText[STATICTEXT_OB]->setText(L"");
		mainGame->SetStaticText(_staticText[STATICTEXT_RULE], 180, mainGame->guiFont, (wchar_t*)str.c_str());
		RefreshDeck();
		_deckSelect->setEnabled(true);

		//forced mode for DevPro ranked duels
		if (mainGame->gameConf.forced) {
			if (_deckSelect->getSelected() == -1 ||
				!deckManager.LoadDeck(_deckSelect->getItem(_deckSelect->getSelected()))) {

				return;
			}
			BufferIO::CopyWStr(_deckSelect->getItem(_deckSelect->getSelected()),
				mainGame->gameConf.lastdeck, 20);
			char deckbuf[1024];
			char* pdeck = deckbuf;
			BufferIO::WriteInt32(pdeck, deckManager.current_deck.main.size() + deckManager.current_deck.extra.size());
			BufferIO::WriteInt32(pdeck, deckManager.current_deck.side.size());
			for (size_t i = 0; i < deckManager.current_deck.main.size(); ++i)
				BufferIO::WriteInt32(pdeck, deckManager.current_deck.main[i]->first);
			for (size_t i = 0; i < deckManager.current_deck.extra.size(); ++i)
				BufferIO::WriteInt32(pdeck, deckManager.current_deck.extra[i]->first);
			for (size_t i = 0; i < deckManager.current_deck.side.size(); ++i)
				BufferIO::WriteInt32(pdeck, deckManager.current_deck.side[i]->first);
			DuelClient::SendBufferToServer(CTOS_UPDATE_DECK, deckbuf, pdeck - deckbuf);
			DuelClient::SendPacketToServer(CTOS_HS_READY);
			_deckSelect->setEnabled(false);
			int selftype = (chkHostPrepReady[0]->isEnabled()) ? 0 : 1;
			chkHostPrepReady[selftype]->setChecked(true);
		}
	}

	void GUIHostRoom::RefreshDeck() {
		_deckSelect->clear();
		DIR * dir;
		struct dirent * dirp;
		if ((dir = opendir("./deck/")) == NULL)
			return;
		while ((dirp = readdir(dir)) != NULL) {
			size_t len = strlen(dirp->d_name);
			if (len < 5 || strcasecmp(dirp->d_name + len - 4, ".ydk") != 0)
				continue;
			dirp->d_name[len - 4] = 0;
			wchar_t wname[256];
			BufferIO::DecodeUTF8(dirp->d_name, wname);
			_deckSelect->addItem(wname);
		}
		closedir(dir);
		for (size_t i = 0; i < _deckSelect->getItemCount(); ++i) {
			if (!wcscmp(_deckSelect->getItem(i), mainGame->gameConf.lastdeck)) {
				_deckSelect->setSelected(i);
				break;
			}
		}
	}

	void GUIHostRoom::SetPlayerName(int index, const wchar_t* name) {
		stHostPrepDuelist[index]->setText(name);
	}

	void GUIHostRoom::SetPlayerElo(int index, const wchar_t* elo) {
		stHostPrepDuelistElo[index]->setText(elo);
	}

	const wchar_t* GUIHostRoom::GetPlayerName(int index) {
		 return stHostPrepDuelist[index]->getText();
	}

	void GUIHostRoom::UpdateObserverText(int watching) {
		if (watching > 0) {
			wchar_t watchbuf[32];
			myswprintf(watchbuf, L"%ls%d", dataManager.GetSysString(1253), watching);
			_staticText[STATICTEXT_OB]->setText(watchbuf);
		}
		else {
			_staticText[STATICTEXT_OB]->setText(L"");
		}
	}

	void GUIHostRoom::PlayerStateChange(int pos, int state, int watching) {
		if (state < 8) {
			mainGame->PlaySound("./sound/playerenter.wav");
			wchar_t* prename = (wchar_t*)stHostPrepDuelist[pos]->getText();
			wchar_t* elo = (wchar_t*)stHostPrepDuelistElo[pos]->getText();
			SetPlayerElo(state, elo);
			SetPlayerName(state, prename);
			SetPlayerName(pos, L"");
			SetPlayerElo(pos, L"");
			chkHostPrepReady[pos]->setChecked(false);
			if (pos == 0)
				BufferIO::CopyWStr(prename, mainGame->dInfo.hostname, 20);
			else if (pos == 1)
				BufferIO::CopyWStr(prename, mainGame->dInfo.hostname_tag, 20);
			else if (pos == 2)
				BufferIO::CopyWStr(prename, mainGame->dInfo.clientname, 20);
			else if (pos == 3)
				BufferIO::CopyWStr(prename, mainGame->dInfo.clientname_tag, 20);
		}
		else if (state == PLAYERCHANGE_READY) {
			chkHostPrepReady[pos]->setChecked(true);
		}
		else if (state == PLAYERCHANGE_NOTREADY) {
			chkHostPrepReady[pos]->setChecked(false);
		}
		else if (state == PLAYERCHANGE_LEAVE) {
			SetPlayerName(pos, L"");
			SetPlayerElo(pos, L"");
			chkHostPrepReady[pos]->setChecked(false);
		}
		else if (state == PLAYERCHANGE_OBSERVE) {
			watching++;
			SetPlayerName(pos, L"");
			SetPlayerElo(pos, L"");
			chkHostPrepReady[pos]->setChecked(false);
			UpdateObserverText(watching);
		}
	}

	void GUIHostRoom::TypeChange(int type, int selftype) {
		bool is_host = ((type >> 4) & 0xf) != 0;
		if (!mainGame->dInfo.isTag) {
			btnHostPrepKick[2]->setVisible(false);
			btnHostPrepKick[3]->setVisible(false);
			if (is_host) {
				_buttons[BUTTON_HP_START]->setVisible(true);
				btnHostPrepKick[0]->setVisible(true);
				btnHostPrepKick[1]->setVisible(true);
			}
			else {
				_buttons[BUTTON_HP_START]->setVisible(false);
				btnHostPrepKick[0]->setVisible(false);
				btnHostPrepKick[1]->setVisible(false);
			}
			chkHostPrepReady[0]->setEnabled(false);
			chkHostPrepReady[0]->setChecked(false);
			chkHostPrepReady[1]->setEnabled(false);
			chkHostPrepReady[1]->setChecked(false);
			if (selftype < 2) {
				chkHostPrepReady[selftype]->setEnabled(true);
				_buttons[BUTTON_HP_DUELIST]->setEnabled(false);
				_buttons[BUTTON_HP_OBSERVER]->setEnabled(true);
			}
			else {
				_buttons[BUTTON_HP_DUELIST]->setEnabled(true);
				_buttons[BUTTON_HP_OBSERVER]->setEnabled(false);
			}
		}
		else {
			if (selftype < 4) {
				chkHostPrepReady[selftype]->setEnabled(false);
				chkHostPrepReady[selftype]->setChecked(false);
			}
			_buttons[BUTTON_HP_DUELIST]->setEnabled(true);
			if (is_host) {
				_buttons[BUTTON_HP_START]->setVisible(true);
				for (int i = 0; i < 4; ++i)
					btnHostPrepKick[i]->setVisible(true);
			}
			else {
				_buttons[BUTTON_HP_START]->setVisible(false);
				for (int i = 0; i < 4; ++i)
					btnHostPrepKick[i]->setVisible(false);
			}
			if (selftype < 4) {
				chkHostPrepReady[selftype]->setEnabled(true);
				_buttons[BUTTON_HP_OBSERVER]->setEnabled(true);
			}
			else {
				_buttons[BUTTON_HP_OBSERVER]->setEnabled(false);
			}
		}
		mainGame->dInfo.player_type = selftype;
	}

	bool GUIHostRoom::OnEvent(const irr::SEvent& event) {
		//handle events here
		irr::gui::IGUIElement* caller = event.GUIEvent.Caller;
		switch (event.EventType) {
		case irr::EET_GUI_EVENT: {
			s32 id = event.GUIEvent.Caller->getID();
			switch (event.GUIEvent.EventType) {
			case irr::gui::EGET_BUTTON_CLICKED: {
				switch (id) {
				case BUTTON_HP_DUELIST: {
					DuelClient::SendPacketToServer(CTOS_HS_TODUELIST);
					break;
				}
				case BUTTON_HP_OBSERVER: {
					DuelClient::SendPacketToServer(CTOS_HS_TOOBSERVER);
					break;
				}
				case BUTTON_HP_KICK: {
					int id = 0;
					while (id < 4) {
						if (btnHostPrepKick[id] == caller)
							break;
						id++;
					}
					CTOS_Kick csk;
					csk.pos = id;
					DuelClient::SendPacketToServer(CTOS_HS_KICK, csk);
					break;
				}
				case BUTTON_HP_START: {
					if (!chkHostPrepReady[0]->isChecked()
						|| !chkHostPrepReady[1]->isChecked())
						break;
					DuelClient::SendPacketToServer(CTOS_HS_START);
					break;
				}
				case BUTTON_HP_CANCEL: {
					DuelClient::StopClient();
					Hide();
					mainGame->wLan.Show();
					mainGame->wChat->setVisible(false);
					if (exit_on_return)
						mainGame->device->closeDevice();
					break;
				}
				break;
				}
			}
			case irr::gui::EGET_CHECKBOX_CHANGED: {
				switch (id) {
				case CHECKBOX_HP_READY: {
					if (!caller->isEnabled())
						break;
					mainGame->env->setFocus(wHostRoom);
					if (static_cast<irr::gui::IGUICheckBox*>(caller)->isChecked()) {
						if (_deckSelect->getSelected() == -1 ||
							!deckManager.LoadDeck(_deckSelect->getItem(_deckSelect->getSelected()))) {
							static_cast<irr::gui::IGUICheckBox*>(caller)->setChecked(false);
							break;
						}
						BufferIO::CopyWStr(_deckSelect->getItem(_deckSelect->getSelected()),
							mainGame->gameConf.lastdeck, 64);
						char deckbuf[1024];
						char* pdeck = deckbuf;
						BufferIO::WriteInt32(pdeck, deckManager.current_deck.main.size() + deckManager.current_deck.extra.size());
						BufferIO::WriteInt32(pdeck, deckManager.current_deck.side.size());
						for (size_t i = 0; i < deckManager.current_deck.main.size(); ++i)
							BufferIO::WriteInt32(pdeck, deckManager.current_deck.main[i]->first);
						for (size_t i = 0; i < deckManager.current_deck.extra.size(); ++i)
							BufferIO::WriteInt32(pdeck, deckManager.current_deck.extra[i]->first);
						for (size_t i = 0; i < deckManager.current_deck.side.size(); ++i)
							BufferIO::WriteInt32(pdeck, deckManager.current_deck.side[i]->first);
						DuelClient::SendBufferToServer(CTOS_UPDATE_DECK, deckbuf, pdeck - deckbuf);
						DuelClient::SendPacketToServer(CTOS_HS_READY);
						_deckSelect->setEnabled(false);
					}
					else {
						DuelClient::SendPacketToServer(CTOS_HS_NOTREADY);
						_deckSelect->setEnabled(true);
					}
					break;
				}
				}
				break;
			}
			break;
			}
		}
		break;
		}
		return false;
	}
}
