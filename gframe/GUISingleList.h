#ifndef SINGLE_LIST_H
#define SINGLE_LIST_H

#include "config.h"
#include "data_manager.h"

namespace ygo {

	class GUISingleList : public irr::IEventReceiver {
	public:
		virtual bool OnEvent(const irr::SEvent&);
		void Load();
		virtual void Hide();
		virtual void Show();
		void OnResize();
		irr::gui::IGUIElement* GetBtnElement(unsigned int);
		const wchar_t* GetText(unsigned int);
		void SetText(unsigned int, const wchar_t*);
		void SetStaticText(unsigned int, const wchar_t*);
		bool IsVisible() { return wSingleList->isVisible(); };
		void ClearList();
		void AddListItem(wchar_t*);
		const wchar_t* GetListData();
		int GetSelected();
		void SetSelected(const wchar_t*);
	private:
		irr::gui::IGUIWindow* wSingleList;
		irr::gui::IGUIListBox* _list;
		std::unordered_map<unsigned int, irr::gui::IGUIButton*> _buttons;
		std::unordered_map<unsigned int, irr::gui::IGUIEditBox*> _editBox;
		std::unordered_map<unsigned int, irr::gui::IGUIStaticText*> _staticText;
	};

}

#endif // !Single_List_H

#define STATICTEXT_SINGLEINFO 700
