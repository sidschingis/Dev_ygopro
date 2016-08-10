#ifndef REPLAY_LIST_H
#define REPLAY_LIST_H

#include "config.h"
#include "data_manager.h"

namespace ygo {

	class GUIReplayList : public irr::IEventReceiver {
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
		bool IsVisible() { return wReplayList->isVisible(); };
		void ClearReplayList();
		void AddListItem(wchar_t*);
		const wchar_t* GetListData();
		int GetSelected();
		void SetSelected(const wchar_t*);
	private:
		irr::gui::IGUIWindow* wReplayList;
		irr::gui::IGUIListBox* _list;
		std::unordered_map<unsigned int, irr::gui::IGUIButton*> _buttons;
		std::unordered_map<unsigned int, irr::gui::IGUIEditBox*> _editBox;
		std::unordered_map<unsigned int, irr::gui::IGUIStaticText*> _staticText;
	};

}

#endif // !Replay_List_H

#define EDITBOX_STARTTURN 600
#define STATICTEXT_REPLAYINFO	601
