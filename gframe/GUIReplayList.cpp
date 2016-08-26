#include "GUIReplayList.h"
#include "game.h"
#include "deck_manager.h"
#include "duelclient.h"
#include "netserver.h"
#include "image_manager.h"
#include "replay_mode.h"

#ifdef _WIN32
#include "dirent.h"
#define strcasecmp _stricmp
#endif

namespace ygo {

	void GUIReplayList::Load() {
		IGUIEnvironment* env = mainGame->env;
		wReplayList = env->addWindow(rect<s32>(320, 100, 800, 510), false, dataManager.GetSysString(1202));
		wReplayList->getCloseButton()->setVisible(false);
		wReplayList->setVisible(false);
		_list = env->addListBox(rect<s32>(10, 30, 350, 400), wReplayList, LISTBOX_REPLAY_LIST, true);
		_list->setItemHeight(18);
		_buttons[BUTTON_LOAD_REPLAY] = env->addButton(rect<s32>(360, 345, 470, 370), wReplayList, BUTTON_LOAD_REPLAY, dataManager.GetSysString(1348));
		_buttons[BUTTON_CANCEL_REPLAY] = env->addButton(rect<s32>(360, 375, 470, 400), wReplayList, BUTTON_CANCEL_REPLAY, dataManager.GetSysString(1347));
		env->addStaticText(dataManager.GetSysString(1349), rect<s32>(360, 30, 570, 50), false, true, wReplayList);
		_staticText[STATICTEXT_REPLAYINFO] = env->addStaticText(L"", rect<s32>(360, 60, 570, 350), false, true, wReplayList, STATICTEXT_REPLAYINFO);
		env->addStaticText(dataManager.GetSysString(1353), rect<s32>(360, 285, 570, 305), false, true, wReplayList);
		_editBox[EDITBOX_STARTTURN] = env->addEditBox(L"", rect<s32>(360, 310, 460, 330), true, wReplayList, EDITBOX_STARTTURN);
		_editBox[EDITBOX_STARTTURN]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		_replayDebug = env->addCheckBox(false, rect<s32>(360, 255, 570, 275), wReplayList, -1, L"Debug");
		_replayDebug->setChecked(mainGame->gameConf.old_replay_mode);
	}

	irr::gui::IGUIElement* GUIReplayList::GetBtnElement(unsigned int item) {
		auto cdit = _buttons.find(item);
		if (cdit == _buttons.end())
			return NULL;
		return _buttons[item];
	}

	const wchar_t* GUIReplayList::GetText(unsigned int item) {
		auto cdit = _editBox.find(item);
		if (cdit == _editBox.end())
			return L"Value Not Found";
		return _editBox[item]->getText();
	}

	void GUIReplayList::SetText(unsigned int item, const wchar_t* text) {
		auto cdit = _editBox.find(item);
		if (cdit != _editBox.end())
			_editBox[item]->setText(text);
	}

	void GUIReplayList::SetStaticText(unsigned int item, const wchar_t* text) {
		auto cdit = _staticText.find(item);
		if (cdit != _staticText.end())
			mainGame->SetStaticText(_staticText[item], 180, mainGame->guiFont, text);
	}

	const wchar_t* GUIReplayList::GetListData() {
		return _list->getListItem(_list->getSelected());
	}

	int GUIReplayList::GetSelected() {
		return _list->getSelected();
	}

	void GUIReplayList::SetSelected(const wchar_t* item) {
		_list->setSelected(item);
	}

	void GUIReplayList::RefreshReplayList() {
		_list->clear();
		FindReplays("./replay/", ".yrp");
	}

	void GUIReplayList::FindReplays(const char* path, const char* extension) {
		DIR * dir;
		struct dirent * dirp;
		if ((dir = opendir(path)) == NULL)
			return;
		while ((dirp = readdir(dir)) != NULL) {
			size_t len = strlen(dirp->d_name);
			if (len < 5 || strcasecmp(dirp->d_name + len - strlen(extension), extension) != 0)
				continue;
			wchar_t wname[256];
			BufferIO::DecodeUTF8(dirp->d_name, wname);
			if (Replay::CheckReplay(wname))
				AddListItem(wname);
		}
		closedir(dir);
	}

	void GUIReplayList::AddListItem(wchar_t* item) {
		_list->addItem(item);
	}

	void GUIReplayList::OnResize() {
		wReplayList->setRelativePosition(mainGame->ResizeWin(320, 100, 800, 510));
	}
	void GUIReplayList::Show() {
		mainGame->PopupElement(wReplayList);
		
		mainGame->device->setEventReceiver(&mainGame->wReplayList);
	}

	void GUIReplayList::Hide() {
		mainGame->HideElement(wReplayList);
	}

	bool GUIReplayList::OnEvent(const irr::SEvent& event) {
		//handle events here
		switch (event.EventType) {
		case irr::EET_GUI_EVENT: {
			s32 id = event.GUIEvent.Caller->getID();
			switch (event.GUIEvent.EventType) {
			case irr::gui::EGET_BUTTON_CLICKED: {
				switch (id) {
				case BUTTON_LOAD_REPLAY: {
					if (GetSelected() == -1)
						break;
					const wchar_t* selected = GetListData();
					mainGame->dInfo.isYRP2 = !_replayDebug->isChecked();
					if (!ReplayMode::cur_replay.OpenReplay(selected))
						break;
					mainGame->wInfoTab.SetImage(imageManager.tCover[0]);
					mainGame->wInfoTab.ShowImage(true);
					mainGame->wInfoTab.Show();
					mainGame->wInfoTab.ClearText();
					mainGame->wReplayControl->setVisible(true);
					mainGame->btnReplayStart->setVisible(false);
					mainGame->btnReplayPause->setVisible(true);
					mainGame->btnReplayStep->setVisible(false);
					mainGame->btnReplayUndo->setVisible(false);
					mainGame->wPhase->setVisible(true);
					mainGame->dField.panel = 0;
					mainGame->dField.hovered_card = 0;
					mainGame->dField.clicked_card = 0;
					mainGame->dField.Clear();
					mainGame->wReplayList.Hide();
					mainGame->device->setEventReceiver(&mainGame->dField);
					unsigned int start_turn = _wtoi(GetText(EDITBOX_STARTTURN));
					if (start_turn == 1)
						start_turn = 0;
					ReplayMode::StartReplay(start_turn);
					break;
				}
				case BUTTON_CANCEL_REPLAY: {
					Hide();
					if (exit_on_return)
						mainGame->device->closeDevice();
					else
						mainGame->wMenu.Show();
					break;
				}
				break;
				}
			}
			case irr::gui::EGET_LISTBOX_CHANGED: {
				switch (id) {
				case LISTBOX_REPLAY_LIST: {
					int sel = GetSelected();
					if (sel == -1)
						break;
					if (!ReplayMode::cur_replay.OpenReplay(GetListData()))
						break;
					wchar_t infobuf[256];
					std::wstring repinfo;
					time_t curtime = ReplayMode::cur_replay.pheader.seed;
					tm* st = localtime(&curtime);
					myswprintf(infobuf, L"%d/%d/%d %02d:%02d:%02d\n", st->tm_year + 1900, st->tm_mon + 1, st->tm_mday, st->tm_hour, st->tm_min, st->tm_sec);
					repinfo.append(infobuf);
					wchar_t namebuf[4][20];
					BufferIO::CopyWStr((unsigned short*)&ReplayMode::cur_replay.replay_data[0], namebuf[0], 20);
					BufferIO::CopyWStr((unsigned short*)&ReplayMode::cur_replay.replay_data[40], namebuf[1], 20);
					if (ReplayMode::cur_replay.pheader.flag & REPLAY_TAG) {
						BufferIO::CopyWStr((unsigned short*)&ReplayMode::cur_replay.replay_data[80], namebuf[2], 20);
						BufferIO::CopyWStr((unsigned short*)&ReplayMode::cur_replay.replay_data[120], namebuf[3], 20);
					}
					if (ReplayMode::cur_replay.pheader.flag & REPLAY_TAG)
						myswprintf(infobuf, L"%ls\n%ls\n===VS===\n%ls\n%ls\n", namebuf[0], namebuf[1], namebuf[2], namebuf[3]);
					else
						myswprintf(infobuf, L"%ls\n===VS===\n%ls\n", namebuf[0], namebuf[1]);
					repinfo.append(infobuf);
					SetText(EDITBOX_STARTTURN, L"1");
					SetStaticText(STATICTEXT_REPLAYINFO, (wchar_t*)repinfo.c_str());
					break;
				}
				}
			break;
			}
		}
		break;
		}
		break;
		}
		return false;
	}
}
