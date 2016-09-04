#ifndef AI_MODE_H
#define AI_MODE_H

#include "config.h"
#include "data_manager.h"

namespace ygo {

	class GUIAIMode : public irr::IEventReceiver {
	public:
		virtual bool OnEvent(const irr::SEvent&);
		void Load();
		virtual void Hide();
		virtual void Show();
		void OnResize();
		int GetComboBoxIndex(unsigned int);
		bool IsChecked(unsigned int);
		bool IsVisible() { return wHost->isVisible(); };
		const wchar_t* GetText(unsigned int item);
	private:
		void RefreshDeck();
		irr::gui::IGUIWindow* wHost;
		std::unordered_map<unsigned int, irr::gui::IGUIButton*> _buttons;
		std::unordered_map<unsigned int, irr::gui::IGUIEditBox*> _editBox;
		std::unordered_map<unsigned int, irr::gui::IGUIComboBox*> _comboBox;
		std::unordered_map<unsigned int, irr::gui::IGUICheckBox*> _checkBox;
	};

}

#endif // !AI_MODE_H

#define CHECKBOX_AI_DEBUG 700