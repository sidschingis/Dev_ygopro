#include "config.h"

namespace ygo {

	class OnlineGUI
	{
	public:
		OnlineGUI() {};
		void Load(irr::gui::IGUIEnvironment* env);
		void Hide() { wOnlineMenu->setVisible(false); };
		void Show() { wOnlineMenu->setVisible(true); };
	private:
		irr::gui::IGUIWindow* wOnlineMenu;
	};
}

