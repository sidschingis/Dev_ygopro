#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include "config.h"
#include "data_manager.h"

namespace ygo {

	class GUIMainMenu : public irr::IEventReceiver {
	public:
		virtual bool OnEvent(const irr::SEvent& event);
		void Load();
		virtual void Hide() { wMenu->setVisible(false); };
		virtual void Show();
		void OnResize();
		irr::gui::IGUIElement* GetBtnElement(unsigned int);
		irr::gui::IGUIWindow* GetWindow() { return wMenu; };		
	private:
		irr::gui::IGUIWindow* wMenu;
		std::unordered_map<unsigned int, irr::gui::IGUIButton*> _buttons;
	};

}

#endif // !MAIN_MENU_H

