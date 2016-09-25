#include "GUICreateHost.h"
#include "game.h"
#include "deck_manager.h"
#include "duelclient.h"
#include "netserver.h"

namespace ygo {

	void GUICreateHost::Load() {
		wchar_t strbuf[256];
		IGUIEnvironment* env = mainGame->env;
		wHost = env->addWindow(rect<s32>(320, 100, 700, 520), false, dataManager.GetSysString(1224));
		wHost->getCloseButton()->setVisible(false);
		wHost->setVisible(false);
		env->addStaticText(dataManager.GetSysString(1226), rect<s32>(20, 30, 220, 50), false, false, wHost);
		_comboBox[COMBOBOX_LFLIST] = env->addComboBox(rect<s32>(140, 25, 300, 50), wHost, COMBOBOX_LFLIST);
		for (unsigned int i = 0; i < deckManager._lfList.size(); ++i)
			_comboBox[COMBOBOX_LFLIST]->addItem(deckManager._lfList[i].listName, deckManager._lfList[i].hash);
		env->addStaticText(dataManager.GetSysString(1225), rect<s32>(20, 60, 220, 80), false, false, wHost);
		_comboBox[COMBOBOX_RULE] = env->addComboBox(rect<s32>(140, 55, 300, 80), wHost, COMBOBOX_RULE);
		_comboBox[COMBOBOX_RULE]->addItem(dataManager.GetSysString(1240));
		_comboBox[COMBOBOX_RULE]->addItem(dataManager.GetSysString(1241));
		_comboBox[COMBOBOX_RULE]->addItem(dataManager.GetSysString(1242));
		_comboBox[COMBOBOX_RULE]->addItem(dataManager.GetSysString(1243));
		env->addStaticText(dataManager.GetSysString(1227), rect<s32>(20, 90, 220, 110), false, false, wHost);
		_comboBox[COMBOBOX_MATCHMODE] = env->addComboBox(rect<s32>(140, 85, 300, 110), wHost, COMBOBOX_MATCHMODE);
		_comboBox[COMBOBOX_MATCHMODE]->addItem(dataManager.GetSysString(1244));
		_comboBox[COMBOBOX_MATCHMODE]->addItem(dataManager.GetSysString(1245));
		_comboBox[COMBOBOX_MATCHMODE]->addItem(dataManager.GetSysString(1246));
		env->addStaticText(dataManager.GetSysString(1237), rect<s32>(20, 120, 320, 140), false, false, wHost);
		myswprintf(strbuf, L"%d", 180);
		_editBox[EDITBOX_TIMELIMIT] = env->addEditBox(strbuf, rect<s32>(140, 115, 220, 140), true, wHost, EDITBOX_TIMELIMIT);
		_editBox[EDITBOX_TIMELIMIT]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		env->addStaticText(dataManager.GetSysString(1228), rect<s32>(20, 150, 320, 170), false, false, wHost);
		_checkBox[CHECKBOX_PRIORITY] = env->addCheckBox(false, rect<s32>(20, 180, 360, 200), wHost, CHECKBOX_PRIORITY, dataManager.GetSysString(1236));
		_checkBox[CHECKBOX_NOCHECKDECK] = env->addCheckBox(false, rect<s32>(20, 210, 170, 230), wHost, CHECKBOX_NOCHECKDECK, dataManager.GetSysString(1229));
		_checkBox[CHECKBOX_NOSHUFFLE] = env->addCheckBox(false, rect<s32>(180, 210, 360, 230), wHost, CHECKBOX_NOSHUFFLE, dataManager.GetSysString(1230));
		env->addStaticText(dataManager.GetSysString(1231), rect<s32>(20, 240, 320, 260), false, false, wHost);
		myswprintf(strbuf, L"%d", 8000);
		_editBox[EDITBOX_STARTLP] = env->addEditBox(strbuf, rect<s32>(140, 235, 220, 260), true, wHost, EDITBOX_STARTLP);
		_editBox[EDITBOX_STARTLP]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		env->addStaticText(dataManager.GetSysString(1232), rect<s32>(20, 270, 320, 290), false, false, wHost);
		myswprintf(strbuf, L"%d", 5);
		_editBox[EDITBOX_STARTHAND] = env->addEditBox(strbuf, rect<s32>(140, 265, 220, 290), true, wHost, EDITBOX_STARTHAND);
		_editBox[EDITBOX_STARTHAND]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		env->addStaticText(dataManager.GetSysString(1233), rect<s32>(20, 300, 320, 320), false, false, wHost);
		myswprintf(strbuf, L"%d", 1);
		_editBox[EDITBOX_DRAWCOUNT] = env->addEditBox(strbuf, rect<s32>(140, 295, 220, 320), true, wHost, EDITBOX_DRAWCOUNT);
		_editBox[EDITBOX_DRAWCOUNT]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		env->addStaticText(dataManager.GetSysString(1234), rect<s32>(10, 360, 220, 380), false, false, wHost);
		_editBox[EDITBOX_SERVERNAME] = env->addEditBox(mainGame->gameConf.gamename, rect<s32>(110, 355, 250, 380), true, wHost, EDITBOX_SERVERNAME);
		_editBox[EDITBOX_SERVERNAME]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		env->addStaticText(dataManager.GetSysString(1235), rect<s32>(10, 390, 220, 410), false, false, wHost);
		_editBox[EDITBOX_SERVERPASS] = env->addEditBox(L"", rect<s32>(110, 385, 250, 410), true, wHost, EDITBOX_SERVERPASS);
		_editBox[EDITBOX_SERVERPASS]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		_buttons[BUTTON_HOST_CONFIRM] = env->addButton(rect<s32>(260, 355, 370, 380), wHost, BUTTON_HOST_CONFIRM, dataManager.GetSysString(1211));
		_buttons[BUTTON_HOST_CANCEL] = env->addButton(rect<s32>(260, 385, 370, 410), wHost, BUTTON_HOST_CANCEL, dataManager.GetSysString(1212));
		
	}

	irr::gui::IGUIElement* GUICreateHost::GetBtnElement(unsigned int item) {
		auto cdit = _buttons.find(item);
		if (cdit == _buttons.end())
			return NULL;
		return _buttons[item];
	}

	const wchar_t* GUICreateHost::GetText(unsigned int item) {
		auto cdit = _editBox.find(item);
		if (cdit == _editBox.end())
			return L"Value Not Found";
		return _editBox[item]->getText();
	}

	bool GUICreateHost::IsChecked(unsigned int item) {
		auto cdit = _checkBox.find(item);
		if (cdit == _checkBox.end())
			return false;
		return _checkBox[item]->isChecked();
	}

	unsigned int GUICreateHost::GetLFData() {
		return _comboBox[COMBOBOX_LFLIST]->getItemData(_comboBox[COMBOBOX_LFLIST]->getSelected());
	}

	int GUICreateHost::GetComboBoxIndex(unsigned int item) {
		return _comboBox[item]->getSelected();
	}

	void GUICreateHost::OnResize() {
		wHost->setRelativePosition(mainGame->ResizeWin(320, 100, 700, 520));
	}
	void GUICreateHost::Show() {
		_buttons[BUTTON_HOST_CONFIRM]->setEnabled(true);
		_buttons[BUTTON_HOST_CANCEL]->setEnabled(true);
		mainGame->PopupElement(wHost);
		mainGame->device->setEventReceiver(&mainGame->wHost);
	}

	void GUICreateHost::Hide() {
		mainGame->HideElement(wHost);
	}

	bool GUICreateHost::OnEvent(const irr::SEvent& event) {
		//handle events here
		switch (event.EventType) {
		case irr::EET_GUI_EVENT: {
			s32 id = event.GUIEvent.Caller->getID();
			switch (event.GUIEvent.EventType) {
			case irr::gui::EGET_BUTTON_CLICKED: {
				switch (id) {
				case BUTTON_HOST_CONFIRM: {
					BufferIO::CopyWStr(_editBox[EDITBOX_SERVERNAME]->getText(), mainGame->gameConf.gamename, 20);
					if (mainGame->dInfo.isLan) {
						if (!NetServer::StartServer(mainGame->gameConf.serverport))
							break;
						if (!DuelClient::StartClient(0x7f000001, mainGame->gameConf.serverport)) {
							NetServer::StopServer();
							break;
						}
					}
					else {
						char ip[20];
						int i = 0;
						wchar_t* pstr = (wchar_t *)mainGame->wLan.GetText(EDITBOX_JOINIP);
						while (*pstr && i < 16)
							ip[i++] = *pstr++;
						ip[i] = 0;

						struct addrinfo hints;
						memset(&hints, 0, sizeof(struct addrinfo));
						hints.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
						hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
						hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
						hints.ai_protocol = 0;          /* Any protocol */
						hints.ai_canonname = NULL;
						hints.ai_addr = NULL;
						hints.ai_next = NULL;
						BufferIO::CopyWStr((wchar_t *)mainGame->wLan.GetText(EDITBOX_JOINIP), ip, 20);
						unsigned int remote_addr = htonl(inet_addr(ip));
						unsigned int remote_port = _wtoi(mainGame->wLan.GetText(EDITBOX_JOINPORT));
						BufferIO::CopyWStr(mainGame->wLan.GetText(EDITBOX_JOINIP), mainGame->gameConf.lastip, 20);
						BufferIO::CopyWStr(mainGame->wLan.GetText(EDITBOX_JOINPORT), mainGame->gameConf.lastport, 20);
						DuelClient::StartClient(remote_addr, remote_port, false);
					}
					_buttons[BUTTON_HOST_CONFIRM]->setEnabled(false);
					_buttons[BUTTON_HOST_CANCEL]->setEnabled(false);
					break;
				}
				case BUTTON_HOST_CANCEL: {
					Hide();
					mainGame->wLan.Show();
					mainGame->dInfo.isLan = false;
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
