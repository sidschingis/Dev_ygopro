#include "GUILanWindow.h"
#include "game.h"
#include "deck_manager.h"
#include "duelclient.h"

namespace ygo {

	void GUILanWindow::Load() {
		IGUIEnvironment* env = mainGame->env;
		wLan = env->addWindow(rect<s32>(220, 100, 800, 520), false, dataManager.GetSysString(1200));
		wLan->getCloseButton()->setVisible(false);
		wLan->setVisible(false);
		env->addStaticText(dataManager.GetSysString(1220), rect<s32>(10, 30, 220, 50), false, false, wLan);
		_editBox[EDITBOX_NICKNAME] = env->addEditBox(mainGame->gameConf.nickname, rect<s32>(110, 25, 450, 50), true, wLan, EDITBOX_NICKNAME);
		_editBox[EDITBOX_NICKNAME]->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
		lstHostList = env->addListBox(rect<s32>(10, 60, 570, 320), wLan, LISTBOX_LAN_HOST, true);
		lstHostList->setItemHeight(18);
		_buttons[BUTTON_LAN_REFRESH] = env->addButton(rect<s32>(240, 325, 340, 350), wLan, BUTTON_LAN_REFRESH, dataManager.GetSysString(1217));
		env->addStaticText(dataManager.GetSysString(1221), rect<s32>(10, 360, 220, 380), false, false, wLan);
		_editBox[EDITBOX_JOINIP] = env->addEditBox(mainGame->gameConf.lastip, rect<s32>(110, 355, 250, 380), true, wLan, EDITBOX_JOINIP);
		_editBox[EDITBOX_JOINIP]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		_editBox[EDITBOX_JOINPORT] = env->addEditBox(mainGame->gameConf.lastport, rect<s32>(260, 355, 320, 380), true, wLan, EDITBOX_JOINPORT);
		_editBox[EDITBOX_JOINPORT]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		env->addStaticText(dataManager.GetSysString(1222), rect<s32>(10, 390, 220, 410), false, false, wLan);
		_editBox[EDITBOX_JOINPASSWORD] = env->addEditBox(mainGame->gameConf.roompass, rect<s32>(110, 385, 250, 410), true, wLan, EDITBOX_JOINPASSWORD);
		_editBox[EDITBOX_JOINPASSWORD]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		_buttons[BUTTON_JOIN_HOST] = env->addButton(rect<s32>(460, 355, 570, 380), wLan, BUTTON_JOIN_HOST, dataManager.GetSysString(1223));
		_buttons[BUTTON_JOIN_CANCEL] = env->addButton(rect<s32>(460, 385, 570, 410), wLan, BUTTON_JOIN_CANCEL, dataManager.GetSysString(1212));
		_buttons[BUTTON_CREATE_HOST] = env->addButton(rect<s32>(460, 25, 570, 50), wLan, BUTTON_CREATE_HOST, dataManager.GetSysString(1224));
	}

	const wchar_t* GUILanWindow::GetText(unsigned int item) {
		auto cdit = _editBox.find(item);
		if (cdit == _editBox.end())
			return L"Value Not Found";
		return _editBox[item]->getText();
	}

	irr::gui::IGUIButton* GUILanWindow::GetBtnElement(unsigned int item) {
		auto cdit = _buttons.find(item);
		if (cdit == _buttons.end())
			return NULL;
		return _buttons[item];
	}

	void GUILanWindow::SetText(unsigned int item, const wchar_t* text) {
		auto cdit = _editBox.find(item);
		if (cdit != _editBox.end())
			_editBox[item]->setText(text);
	}

	int GUILanWindow::GetSelectedHost() {
		return lstHostList->getSelected();
	}

	void GUILanWindow::ClearHostList() {
		lstHostList->clear();
	}

	void GUILanWindow::AddHost(const wchar_t* host) {
		lstHostList->addItem(host);
	}

	void GUILanWindow::OnResize() {
		wLan->setRelativePosition(mainGame->ResizeWin(220, 100, 800, 520));
	}
	void GUILanWindow::Show() {
		mainGame->PopupElement(wLan);
		mainGame->device->setEventReceiver(&mainGame->wLan);
	}

	void GUILanWindow::Hide() {
		mainGame->HideElement(wLan);
	}

	bool GUILanWindow::OnEvent(const irr::SEvent& event) {
		//handle events here
		switch (event.EventType) {
		case irr::EET_GUI_EVENT: {
			s32 id = event.GUIEvent.Caller->getID();
			switch (event.GUIEvent.EventType) {
			case irr::gui::EGET_BUTTON_CLICKED: {
				switch (id) {
				case BUTTON_JOIN_HOST: {
					OnJoinHost();
					break;
				}
				case BUTTON_JOIN_CANCEL: {
					mainGame->wLan.Hide();
					mainGame->wMenu.Show();
					break;
				}
				case BUTTON_LAN_REFRESH: {
					DuelClient::BeginRefreshHost();
					break;
				}
				case BUTTON_CREATE_HOST: {
					mainGame->btnHostConfirm->setEnabled(true);
					mainGame->btnHostCancel->setEnabled(true);
					mainGame->wLan.Hide();
					mainGame->ShowElement(mainGame->wCreateHost);
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
	void GUILanWindow::OnJoinHost()
	{
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
		if (DuelClient::StartClient(remote_addr, remote_port, false)) {
			mainGame->wLan.Hide();
		}
	}
}
