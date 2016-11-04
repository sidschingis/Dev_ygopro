#include "GUIAIMode.h"
#include "game.h"
#include "deck_manager.h"
#include "duelclient.h"
#include "netserver.h"

#ifdef _WIN32
#include "dirent.h"
#define strcasecmp _stricmp
#endif

namespace ygo {
	void GUIAIMode::Load() {
		wchar_t strbuf[256];
		IGUIEnvironment* env = mainGame->env;
		myswprintf(strbuf, L"%ls - %ls", dataManager.GetSysString(2017), mainGame->gameConf.botname);
		wHost = env->addWindow(rect<s32>(320, 100, 800, 320), false, strbuf);
		wHost->getCloseButton()->setVisible(false);
		wHost->setVisible(false);
		_buttons[BUTTON_HOST_CONFIRM] = env->addButton(rect<s32>(360, 155, 470, 180), wHost, BUTTON_HOST_CONFIRM, dataManager.GetSysString(1211));
		_buttons[BUTTON_HOST_CANCEL] = env->addButton(rect<s32>(360, 185, 470, 210), wHost, BUTTON_HOST_CANCEL, dataManager.GetSysString(1212));
		env->addStaticText(dataManager.GetSysString(1231), rect<s32>(10, 45, 130, 65), false, false, wHost);
		_editBox[EDITBOX_STARTLP] = env->addEditBox(L"8000", rect<s32>(10, 65, 130, 90), true, wHost, EDITBOX_STARTLP);
		_editBox[EDITBOX_STARTLP]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		env->addStaticText(dataManager.GetSysString(1232), rect<s32>(140, 45, 220, 65), false, false, wHost);
		_editBox[EDITBOX_STARTHAND] = env->addEditBox(L"5", rect<s32>(140, 65, 220, 90), true, wHost, EDITBOX_STARTHAND);
		_editBox[EDITBOX_STARTHAND]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		env->addStaticText(dataManager.GetSysString(2015), rect<s32>(230, 45, 310, 65), false, false, wHost);
		_editBox[EDITBOX_DRAWCOUNT] = env->addEditBox(L"1", rect<s32>(230, 65, 310, 90), true, wHost, EDITBOX_DRAWCOUNT);
		_editBox[EDITBOX_DRAWCOUNT]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		env->addStaticText(dataManager.GetSysString(1228), rect<s32>(20, 110, 360, 130), false, false, wHost);
		_checkBox[CHECKBOX_NOSHUFFLE] = env->addCheckBox(false, rect<s32>(30, 140, 210, 160), wHost, CHECKBOX_NOSHUFFLE, dataManager.GetSysString(1230));
		_checkBox[CHECKBOX_PRIORITY] = env->addCheckBox(false, rect<s32>(200, 140, 360, 160), wHost, CHECKBOX_PRIORITY, dataManager.GetSysString(1236));
		_checkBox[CHECKBOX_AI_DEBUG] = env->addCheckBox(false, rect<s32>(30, 170, 210, 190), wHost, CHECKBOX_AI_DEBUG, dataManager.GetSysString(2016));
		env->addStaticText(dataManager.GetSysString(2017), rect<s32>(320, 45, 470, 65), false, false, wHost);
		_comboBox[COMBOBOX_DBDECKS] = env->addComboBox(rect<s32>(320, 65, 470, 90), wHost);
		_comboBox[COMBOBOX_DBDECKS]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
	}

	void GUIAIMode::RefreshDeck() {
		_comboBox[COMBOBOX_DBDECKS]->clear();
		DIR * dir;
		struct dirent * dirp;
		if ((dir = opendir("./AIDecks/")) == NULL)
			return;
		while ((dirp = readdir(dir)) != NULL) {
			size_t len = strlen(dirp->d_name);
			if (len < 5 || strcasecmp(dirp->d_name + len - 4, ".ydk") != 0)
				continue;
			dirp->d_name[len - 4] = 0;
			wchar_t wname[256];
			BufferIO::DecodeUTF8(dirp->d_name, wname);
			_comboBox[COMBOBOX_DBDECKS]->addItem(wname);
		}
		closedir(dir);
	}

	const wchar_t* GUIAIMode::GetText(unsigned int item) {
		auto cdit = _editBox.find(item);
		if (cdit == _editBox.end())
			return L"Value Not Found";
		return _editBox[item]->getText();
	}

	bool GUIAIMode::IsChecked(unsigned int item) {
		auto cdit = _checkBox.find(item); \
			if (cdit == _checkBox.end())
				return false;
		return _checkBox[item]->isChecked();
	}

	int GUIAIMode::GetComboBoxIndex(unsigned int item) {
		return _comboBox[item]->getSelected();
	}

	void GUIAIMode::OnResize() {
		wHost->setRelativePosition(mainGame->ResizeWin(320, 100, 800, 320));
	}
	void GUIAIMode::Show() {
		RefreshDeck();
		mainGame->PopupElement(wHost);
		mainGame->device->setEventReceiver(&mainGame->wAI);
	}

	void GUIAIMode::Hide() {
		mainGame->HideElement(wHost);
	}

	bool GUIAIMode::OnEvent(const irr::SEvent& event) {
		//handle events here
		switch (event.EventType) {
		case irr::EET_GUI_EVENT: {
			s32 id = event.GUIEvent.Caller->getID();
			switch (event.GUIEvent.EventType) {
			case irr::gui::EGET_BUTTON_CLICKED: {
				switch (id) {
				case BUTTON_HOST_CONFIRM: {
					mainGame->is_aimode = true;
					if (!NetServer::StartServer(7911))
						break;
					if (!DuelClient::StartClient(0x7f000001, 7911)) {
						NetServer::StopServer();
						mainGame->is_aimode = false;
						break;
					}
					Hide();
					wchar_t args[256];
					wsprintf(args, L"%ls %ls 0x%X", mainGame->gameConf.botname, _comboBox[COMBOBOX_DBDECKS]->getItem(GetComboBoxIndex(COMBOBOX_DBDECKS)), PRO_VERSION);
					ShellExecute(NULL, L"open", L"devbot.exe", args, NULL, IsChecked(CHECKBOX_AI_DEBUG) ? SW_SHOWDEFAULT : SW_HIDE);
					break;
				}
				case BUTTON_HOST_CANCEL: {
					Hide();
					mainGame->is_aimode = false;
					mainGame->wMenu.Show();
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