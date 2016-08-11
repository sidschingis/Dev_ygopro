#ifndef HOST_ROOM_H
#define HOST_ROOM_H

#include "config.h"
#include "data_manager.h"

namespace ygo {

	class GUIHostRoom : public irr::IEventReceiver {
	public:
		virtual bool OnEvent(const irr::SEvent&);
		void Load();
		virtual void Hide();
		virtual void Show();
		void OnResize();
		irr::gui::IGUIElement* GetBtnElement(unsigned int);	
		bool IsVisible() { return wHostRoom->isVisible(); };
		void EnableDeckSelect(bool value) { _deckSelect->setEnabled(value); };
		void RoomSetUp(bool isTag, std::wstring);
		void TypeChange(int type);
		void SetPlayerName(int index, const wchar_t* name);
		void SetPlayerElo(int index, const wchar_t* elo);
		const wchar_t* GetPlayerName(int index);
		void PlayerStateChange(int index, int state, int watching);
		void UpdateObserverText(int watching);
	private:
		irr::gui::IGUIWindow* wHostRoom;
		std::unordered_map<unsigned int, irr::gui::IGUIButton*> _buttons;
		std::unordered_map<unsigned int, irr::gui::IGUIStaticText*> _staticText;
		irr::gui::IGUIComboBox* _deckSelect;
		irr::gui::IGUIStaticText* stHostPrepDuelistElo[4];
		irr::gui::IGUIStaticText* stHostPrepDuelist[4];
		irr::gui::IGUICheckBox* chkHostPrepReady[4];
		irr::gui::IGUIButton* btnHostPrepKick[4];
	};

}

#endif // !HOST_ROOM_H

#define STATICTEXT_OB			1000
#define STATICTEXT_RULE			1001
