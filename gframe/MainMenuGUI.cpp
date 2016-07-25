#include "MainMenuGUI.h"
#include "data_manager.h"

namespace ygo {

	void MainMenuGUI::Load(irr::gui::IGUIEnvironment* env)
	{
		wchar_t strbuf[256];
		myswprintf(strbuf, L"YGOPro DevPro (Version:%X.0%X.%X)", PRO_VERSION >> 12, (PRO_VERSION >> 4) & 0xff, PRO_VERSION & 0xf);
		wMenu = env->addWindow(rect<s32>(370, 200, 650, 415), false, strbuf);
		wMenu->getCloseButton()->setVisible(false);
		btnLanMode = env->addButton(rect<s32>(10, 30, 270, 60), wMenu, BUTTON_LAN_MODE, dataManager.GetSysString(1200));
		btnServerMode = env->addButton(rect<s32>(10, 65, 270, 95), wMenu, BUTTON_SINGLE_MODE, dataManager.GetSysString(1201));
		btnReplayMode = env->addButton(rect<s32>(10, 100, 270, 130), wMenu, BUTTON_REPLAY_MODE, dataManager.GetSysString(1202));
		//	btnTestMode = env->addButton(rect<s32>(10, 135, 270, 165), wMenu, BUTTON_TEST_MODE, dataManager.GetSysString(1203));
		btnDeckEdit = env->addButton(rect<s32>(10, 135, 270, 165), wMenu, BUTTON_DECK_EDIT, dataManager.GetSysString(1204));
		btnModeExit = env->addButton(rect<s32>(10, 170, 270, 200), wMenu, BUTTON_MODE_EXIT, dataManager.GetSysString(1210));
		
	}

	irr::gui::IGUIElement* MainMenuGUI::GetGUIElement(const char* args)
	{
		if (!strcmp(args, "-r"))
			return btnReplayMode;
		else if (!strcmp(args, "-d"))
			return btnDeckEdit;
		else if(!strcmp(args, "-s"))
			return btnServerMode;
		return NULL;
	}

	void MainMenuGUI::OnResize()
	{
		//wMenu->setRelativePosition(mainGame->ResizeWin(370, 200, 650, 415));
	}
}
