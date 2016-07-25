#ifndef ONLINE_H
#define ONLINE_H

#include "config.h"

namespace ygo {

	class GUIOnline
	{
	public:
		GUIOnline() {};
		void Load();
		void Hide() { wOnlineMenu->setVisible(false); };
		void Show() { wOnlineMenu->setVisible(true); };
	private:
		irr::gui::IGUIWindow* wOnlineMenu;
	};
}

#endif // !ONLINE_H