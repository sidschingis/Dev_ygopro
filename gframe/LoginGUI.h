#include "config.h"

namespace ygo {

	class LoginGUI
	{
	public:
		LoginGUI() {};
		void Load(irr::gui::IGUIEnvironment* env);
		void Hide() { wLoginMenu->setVisible(false); };
		void Show() { wLoginMenu->setVisible(true); };
	private:
		irr::gui::IGUIWindow* wLoginMenu;
	};
}

