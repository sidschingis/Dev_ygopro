#ifndef LAN_H
#define LAN_H

#include "config.h"
#include "data_manager.h"

namespace ygo {

	class GUILanWindow : public irr::IEventReceiver {
	public:
		virtual bool OnEvent(const irr::SEvent& event);
		void Load();
		virtual void Hide();
		virtual void Show();
		void OnResize();
		const wchar_t* GetText(unsigned int value);
		irr::gui::IGUIButton* GetBtnElement(unsigned int item);
		void SetText(unsigned int item, const wchar_t* text);
		int GetSelectedHost();
		void ClearHostList();
		void AddHost(const wchar_t* host);
		bool isVisible() { return wLan->isVisible(); };
		void OnJoinHost();
	private:
		
		irr::gui::IGUIWindow* wLan;
		irr::gui::IGUIListBox* lstHostList;
		std::unordered_map<unsigned int, irr::gui::IGUIButton*> _buttons;
		std::unordered_map<unsigned int, irr::gui::IGUIEditBox*> _editBox;
	};

}

#endif // !LAN_H

#define EDITBOX_NICKNAME		400
#define EDITBOX_JOINIP			401
#define EDITBOX_JOINPORT		402
#define EDITBOX_JOINPASSWORD	403
