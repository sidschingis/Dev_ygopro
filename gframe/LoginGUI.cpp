#include "LoginGUI.h"

namespace ygo {

	void LoginGUI::Load(irr::gui::IGUIEnvironment* env)
	{
//		wchar_t strbuf[256];
		wLoginMenu = env->addWindow(rect<s32>(370, 200, 650, 415), false, L"YGOPro DevPro Login");
		wLoginMenu->getCloseButton()->setVisible(false);
		
		
		
		Hide();
	}
}
