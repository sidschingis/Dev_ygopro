#ifndef CHAT_H
#define CHAT_H

#include "config.h"
#include "data_manager.h"

namespace ygo {

	class GUIChat : public irr::IEventReceiver {
	public:
		virtual bool OnEvent(const irr::SEvent& event);
		void Load();
		virtual void Hide();
		virtual void Show();
		void OnResize();		
	private:
		irr::gui::IGUIWindow* wChat;
		irr::gui::IGUIEditBox* ebChatInput;
	};

}

#endif // !CHAT_H

