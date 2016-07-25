#include "GUILogin.h"
#include "game.h"

namespace ygo {

	void GUILogin::Load()
	{
//		wchar_t strbuf[256];
		wLoginMenu = mainGame->env->addWindow(rect<s32>(370, 200, 650, 415), false, L"YGOPro DevPro Login");
		wLoginMenu->getCloseButton()->setVisible(false);
		
		
		
		Hide();
	}
}
