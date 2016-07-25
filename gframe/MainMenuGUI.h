#include "config.h"

namespace ygo {

	class MainMenuGUI
	{
	public:
		MainMenuGUI() {};
		void Load(irr::gui::IGUIEnvironment* env);
		void Hide() { wMenu->setVisible(false); };
		void Show() { wMenu->setVisible(true); };
		void OnResize();
		irr::gui::IGUIElement* GetGUIElement(const char* args);
		irr::gui::IGUIWindow* GetWindow() { return wMenu; };		
	private:
		irr::gui::IGUIWindow* wMenu;
		irr::gui::IGUIButton* btnLanMode;
		irr::gui::IGUIButton* btnTestMode;
		irr::gui::IGUIButton* btnServerMode;
		irr::gui::IGUIButton* btnReplayMode;
		irr::gui::IGUIButton* btnDeckEdit;
		irr::gui::IGUIButton* btnModeExit;
	};

}
#define BUTTON_LAN_MODE				100
#define BUTTON_SINGLE_MODE			101
#define BUTTON_REPLAY_MODE			102
#define BUTTON_TEST_MODE			103
#define BUTTON_DECK_EDIT			104
#define BUTTON_MODE_EXIT			105

