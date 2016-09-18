#ifndef LOGIN_H
#define LOGIN_H

#include "config.h"

namespace ygo {

	class GUILogin
	{
	public:
		GUILogin() {};
		void Load();
		void Hide() { wLoginMenu->setVisible(false); };
		void Show() { wLoginMenu->setVisible(true); };
	private:
		irr::gui::IGUIWindow* wLoginMenu;
	};
}

#endif // !LOGIN_H