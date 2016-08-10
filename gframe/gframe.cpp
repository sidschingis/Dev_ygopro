#include "config.h"
#include "game.h"
#include "data_manager.h"
#include "menu_handler.h"
#include <event2/thread.h>

int enable_log = 0;
bool exit_on_return = false;

int main(int argc, char* argv[]) {
#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
	evthread_use_windows_threads();
#else
	evthread_use_pthreads();
#endif //_WIN32
	ygo::Game _game;
	ygo::mainGame = &_game;
	_game.window_size = dimension2du(1024, 640);
	if(!ygo::mainGame->Initialize())
		return 0;

	for(int i = 1; i < argc; ++i) {
		/*command line args:
		 * -j: join host (host info from system.conf)
		 * -f: forced join host (host info from system.conf, immediately set ready)
		 * -d: deck edit
		 * -r: replay */
		if(argv[i][0] == '-' && argv[i][1] == 'e') {
			ygo::dataManager.LoadDB(&argv[i][2]);
		} else if(!strcmp(argv[i], "-j") || !strcmp(argv[i], "-f") || !strcmp(argv[i], "-d") || !strcmp(argv[i], "-r") || !strcmp(argv[i], "-s")) {
			exit_on_return = true;
			irr::SEvent event;
			event.EventType = irr::EET_GUI_EVENT;
			event.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
			if(!strcmp(argv[i], "-j")) {
				ygo::mainGame->wMenu.Hide();
				ygo::mainGame->wLan.OnJoinHost();
			} else if(!strcmp(argv[i], "-f")) {
				ygo::mainGame->wMenu.Hide();
				ygo::mainGame->wLan.OnJoinHost();
			} else if(!strcmp(argv[i], "-d")) {
				event.GUIEvent.Caller = ygo::mainGame->wMenu.GetBtnElement(BUTTON_DECK_EDIT);
				ygo::mainGame->device->postEventFromUser(event);
			} else if(!strcmp(argv[i], "-r")) {
				event.GUIEvent.Caller = ygo::mainGame->wMenu.GetBtnElement(BUTTON_REPLAY_MODE);
				ygo::mainGame->device->postEventFromUser(event);
				ygo::mainGame->wReplayList.SetSelected(ygo::mainGame->gameConf.lastreplay);
				if(ygo::mainGame->wReplayList.GetSelected() != -1){
					event.GUIEvent.Caller = ygo::mainGame->wReplayList.GetBtnElement(BUTTON_LOAD_REPLAY);
					ygo::mainGame->device->postEventFromUser(event);
				}
			} else if(!strcmp(argv[i], "-s")) {
				event.GUIEvent.Caller = ygo::mainGame->wMenu.GetBtnElement(BUTTON_SINGLE_MODE);
				ygo::mainGame->device->postEventFromUser(event);
				ygo::mainGame->wSingleList.SetSelected(ygo::mainGame->gameConf.lastpuzzle);
				if(ygo::mainGame->wSingleList.GetSelected() != -1){
					event.GUIEvent.Caller = ygo::mainGame->wSingleList.GetBtnElement(BUTTON_LOAD_SINGLEPLAY);
					ygo::mainGame->device->postEventFromUser(event);
				}
			}

		}
	}
	ygo::mainGame->MainLoop();
#ifdef _WIN32
	WSACleanup();
#else

#endif //_WIN32
	return EXIT_SUCCESS;
}
