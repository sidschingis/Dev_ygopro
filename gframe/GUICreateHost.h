#ifndef CREATE_HOST_H
#define CREATE_HOST_H

#include "config.h"
#include "data_manager.h"

namespace ygo {

	class GUICreateHost : public irr::IEventReceiver {
	public:
		virtual bool OnEvent(const irr::SEvent&);
		void Load();
		virtual void Hide();
		virtual void Show();
		void OnResize();
		const wchar_t* GetText(unsigned int);
		irr::gui::IGUIElement* GetBtnElement(unsigned int);	
		int GetComboBoxIndex(unsigned int);
		bool IsChecked(unsigned int);
		unsigned int GetLFData();
		bool IsVisible() { return wHost->isVisible(); };
	private:
		irr::gui::IGUIWindow* wHost;
		std::unordered_map<unsigned int, irr::gui::IGUIButton*> _buttons;
		std::unordered_map<unsigned int, irr::gui::IGUIEditBox*> _editBox;
		std::unordered_map<unsigned int, irr::gui::IGUIComboBox*> _comboBox;
		std::unordered_map<unsigned int, irr::gui::IGUICheckBox*> _checkBox;
	};

}

#endif // !CREATE_HOST_H

#define COMBOBOX_LFLIST			500
#define COMBOBOX_RULE			501
#define COMBOBOX_MATCHMODE		502
#define EDITBOX_TIMELIMIT		503
#define EDITBOX_STARTLP			504
#define EDITBOX_STARTHAND		505
#define EDITBOX_DRAWCOUNT		506
#define EDITBOX_SERVERNAME		507
#define EDITBOX_SERVERPASS		508
#define CHECKBOX_PRIORITY		509
#define CHECKBOX_NOCHECKDECK	510
#define CHECKBOX_NOSHUFFLE		511
