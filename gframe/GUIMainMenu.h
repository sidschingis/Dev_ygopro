#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "config.h"

namespace ygo {

	class GUIMainMenu {
	public:
		GUIMainMenu() {};
		void Load();
		void Hide() { wMenu->setVisible(false); };
		void Show() { wMenu->setVisible(true); };
		void OnResize();
		irr::gui::IGUIElement* GetGUIElement(const char* args);
		irr::gui::IGUIWindow* GetWindow() { return wMenu; };		
	private:
		irr::gui::IGUIWindow* wMenu;
		std::unordered_map<unsigned int, irr::gui::IGUIButton*> _buttons;
	};

}

#endif // !MAIN_MENU_H

