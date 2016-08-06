#ifndef DECK_EDIT
#define DECK_EDIT

#include "config.h"
#include "data_manager.h"

namespace ygo {

	class GUIDeckEdit : public irr::IEventReceiver {
	public:
		virtual bool OnEvent(const irr::SEvent& event);
		void Load();
		void Hide();
		void ShowDeckEdit();
		void ShowSiding();
		void OnResize();
		irr::gui::IGUIScrollBar* GetScrollBar() { return scrFilter; };
		void FilterCards(bool checkDescription);
		void FilterStart();
		void SortList();
		void ClearFilter();

		static bool CardNameCompare(const wchar_t *sa, const wchar_t *sb);

		long long filter_effect;
		unsigned int filter_type;
		unsigned int filter_type2;
		unsigned int filter_attrib;
		unsigned int filter_race;
		unsigned int filter_atktype;
		int filter_atk;
		unsigned int filter_deftype;
		int filter_def;
		unsigned int filter_lvtype;
		unsigned int filter_lv;
		unsigned int filter_scltype;
		unsigned int filter_scl;
		uint32 filter_setcode;
		int filter_lm;
		int hovered_code;
		int hovered_pos;
		int hovered_seq;
		int click_pos;
		bool is_draging;
		int dragx;
		int dragy;
		size_t pre_mainc;
		size_t pre_extrac;
		size_t pre_sidec;
		code_pointer draging_pointer;

		std::unordered_map<int, int>* filterList;
		std::vector<code_pointer> results;
		wchar_t result_string[8];
	private:
		irr::gui::IGUIStaticText* wDeckEdit;
		irr::gui::IGUIStaticText* wSort;
		irr::gui::IGUIStaticText* wFilter;
		std::unordered_map<unsigned int, irr::gui::IGUIStaticText*> _staticText;
		std::unordered_map<unsigned int, irr::gui::IGUIButton*> _buttons;
		std::unordered_map<unsigned int, irr::gui::IGUIComboBox*> _ComboBox;
		std::unordered_map<unsigned int, irr::gui::IGUIEditBox*> _editBox;
		
		irr::gui::IGUIScrollBar* scrFilter;
		irr::gui::IGUIWindow* wCategories;
		irr::gui::IGUICheckBox* chkCategory[32];
	};

}

#endif // !DECK_EDIT
#define COMBOBOX_DBLFLIST			301
#define COMBOBOX_DBDECKS			302
#define COMBOBOX_MAINTYPE			310
#define SCROLL_FILTER				314
#define EDITBOX_KEYWORD				315


//new
#define COMBOBOX_MAINTYPE2			400
#define COMBOBOX_LIMIT				401
#define COMBOBOX_ATTRIBUTE			402
#define COMBOBOX_RACE				403
#define COMBOBOX_SETCODE			404
#define EDITBOX_ATTACK				405
#define EDITBOX_LEVEL				406
#define EDITBOX_SCALE				407
#define EDITBOX_DEFENCE				408
#define EDITBOX_DECKNAME			409
#define STATICTEXT_BANLIST			410
#define STATICTEXT_ATTRIBUTE		411
#define STATICTEXT_DECK				412
#define STATICTEXT_LIMIT			413
#define STATICTEXT_CATEGORY			414
#define STATICTEXT_ATTACK			415
#define STATICTEXT_DEFENCE			416
#define STATICTEXT_LEVEL			417
#define STATICTEXT_SCALE			418
#define STATICTEXT_SEARCH			419
#define STATICTEXT_TYPE				420
#define STATICTEXT_SET				421