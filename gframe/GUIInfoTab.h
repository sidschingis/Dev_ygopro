#ifndef INFO_TAB_H
#define INFO_TAB_H

#include "config.h"
#include "data_manager.h"

namespace ygo {

	class GUIInfoTab : public irr::IEventReceiver {
	public:
		virtual bool OnEvent(const irr::SEvent& event);
		void Load();
		virtual void Hide();
		virtual void Show();
		void OnResize();
		void SetImage(int code);
		void SetImage(irr::video::ITexture* image);
		void ShowImage(bool value);
		void ClearText();
		void LogAddItem(wchar_t* text);
		void ClearLog();
		void EnableScrollBar(bool value, int step = 0);
		void ShowCardInfo(int code);
		int GetLogSelectedIndex();
		void SetActiveTab(int index);
		bool IsChecked(unsigned int);
		void SetChecked(unsigned int, bool checked);
	private:
		int displayedcard = 0;
		const wchar_t* showingtext;
		irr::gui::IGUIImage* imgCard;
		irr::gui::IGUITabControl* wInfos;
		irr::gui::IGUIListBox* lstLog;
		std::unordered_map<unsigned int, irr::gui::IGUIStaticText*> _staticText;
		std::unordered_map<unsigned int, irr::gui::IGUIButton*> _buttons;
		std::unordered_map<unsigned int, irr::gui::IGUICheckBox*> _checkBox;

		irr::gui::IGUIScrollBar* scrCardText;
		irr::gui::IGUIScrollBar* scrSound;
		irr::gui::IGUIScrollBar* scrMusic;
	};

}

#endif // !INFO_TAB_H

#define STATICTEXT_CARDIMAGE		500
#define STATICTEXT_CARDNAME			501
#define STATICTEXT_CARDINFO			502
#define STATICTEXT_CARDDATA			503
#define STATICTEXT_CARDSETNAME		504
#define STATICTEXT_CARDTEXT			505
#define CHECKBOX_AUTOPOS			600
#define CHECKBOX_RANDOMPOS			601
#define CHECKBOX_AUTOCHAIN			602
#define CHECKBOX_WAITCHAIN			603
#define CHECKBOX_MUTEOPPONENT		604
#define CHECKBOX_MUTESPECTATORS		605
#define CHECKBOX_HIDESETNAMES		606
#define CHECKBOX_HIDE_CHAINBUTTONS	607
#define CHECKBOX_HIDE_HINTBUTTON	608