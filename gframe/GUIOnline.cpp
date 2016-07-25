#include "GUIOnline.h"
#include "game.h"

namespace ygo {

	void GUIOnline::Load()
	{
		wchar_t strbuf[256];
		myswprintf(strbuf, L"YGOPro DevPro Online (Version:%X.0%X.%X)", PRO_VERSION >> 12, (PRO_VERSION >> 4) & 0xff, PRO_VERSION & 0xf);
		wOnlineMenu = mainGame->env->addWindow(rect<s32>(370, 200, 650, 415), false, strbuf);
		wOnlineMenu->getCloseButton()->setVisible(false);
		Hide();
	}
}
