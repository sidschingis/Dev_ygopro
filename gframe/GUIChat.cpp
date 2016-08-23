#include "GUIChat.h"
#include "game.h"
#include "duelclient.h"

namespace ygo {

	void GUIChat::Load() {
		IGUIEnvironment* env = mainGame->env;
		wChat = env->addWindow(rect<s32>(305, 615, 1020, 640), false, L"");
		wChat->getCloseButton()->setVisible(false);
		wChat->setDraggable(false);
		wChat->setDrawTitlebar(false);
		wChat->setVisible(false);
		ebChatInput = env->addEditBox(L"", rect<s32>(3, 2, 710, 22), true, wChat, EDITBOX_CHAT);
	}


	void GUIChat::OnResize() {
		wChat->setRelativePosition(mainGame->ResizeWin(305, 615, 1020, 640, true));
		ebChatInput->setRelativePosition(recti(3, 2, mainGame->window_size.Width - wChat->getRelativePosition().UpperLeftCorner.X - 6, 22));
	}
	void GUIChat::Show() {
		wChat->setVisible(true);
		//dont set event handler, have other UI elements call it insted
	}

	void GUIChat::Hide() {
		wChat->setVisible(false);
	}

	bool GUIChat::OnEvent(const irr::SEvent& event) {
		//handle events here
		switch (event.EventType) {
		case irr::EET_GUI_EVENT: {
			s32 id = event.GUIEvent.Caller->getID();
			switch (event.GUIEvent.EventType) {
			case irr::gui::EGET_EDITBOX_ENTER: {
				switch (id) {
				case EDITBOX_CHAT: {
					if (mainGame->dInfo.isReplay)
						break;
					const wchar_t* input = ebChatInput->getText();
					if (input[0]) {
						unsigned short msgbuf[256];
						if (mainGame->dInfo.isStarted) {
							if (mainGame->dInfo.player_type < 7) {
								if (mainGame->dInfo.isTag && (mainGame->dInfo.player_type % 2))
									mainGame->AddChatMsg((wchar_t*)input, 2);
								else
									mainGame->AddChatMsg((wchar_t*)input, 0);
							}
							else
								mainGame->AddChatMsg((wchar_t*)input, 10);
						}
						else
							mainGame->AddChatMsg((wchar_t*)input, 7);
						int len = BufferIO::CopyWStr(input, msgbuf, 256);
						DuelClient::SendBufferToServer(CTOS_CHAT, msgbuf, (len + 1) * sizeof(short));
						ebChatInput->setText(L"");
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
