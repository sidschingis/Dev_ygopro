#include "GUIDeckEdit.h"
#include "game.h"
#include "deck_manager.h"
#include "image_manager.h"

namespace ygo {

	void GUIDeckEdit::Load() {
		wDeckEdit = mainGame->env->addStaticText(L"", rect<s32>(309, 8, 605, 130), true, false, 0, -1, true);
		wSort = mainGame->env->addStaticText(L"", rect<s32>(930, 132, 1020, 156), true, false, 0, -1, true);
		wFilter = mainGame->env->addStaticText(L"", rect<s32>(610, 8, 1020, 130), true, false, 0, -1, true);

		_staticText[STATICTEXT_BANLIST] = mainGame->env->addStaticText(dataManager.GetSysString(1300), rect<s32>(10, 9, 100, 29), false, false, wDeckEdit, STATICTEXT_BANLIST);
		_staticText[STATICTEXT_DECK] = mainGame->env->addStaticText(dataManager.GetSysString(1301), rect<s32>(10, 39, 100, 59), false, false, wDeckEdit, STATICTEXT_DECK);
		_staticText[STATICTEXT_CATEGORY] = mainGame->env->addStaticText(dataManager.GetSysString(1311), rect<s32>(10, 5, 70, 25), false, false, wFilter, STATICTEXT_CATEGORY);
		_staticText[STATICTEXT_LIMIT] = mainGame->env->addStaticText(dataManager.GetSysString(1315), rect<s32>(205, 5, 280, 25), false, false, wFilter, STATICTEXT_LIMIT);
		_staticText[STATICTEXT_ATTRIBUTE] = mainGame->env->addStaticText(dataManager.GetSysString(1319), rect<s32>(10, 28, 70, 48), false, false, wFilter, STATICTEXT_ATTRIBUTE);
		_staticText[STATICTEXT_ATTACK] = mainGame->env->addStaticText(dataManager.GetSysString(1322), rect<s32>(205, 28, 280, 48), false, false, wFilter, STATICTEXT_ATTACK);
		_staticText[STATICTEXT_DEFENCE] = mainGame->env->addStaticText(dataManager.GetSysString(1323), rect<s32>(205, 51, 280, 71), false, false, wFilter, STATICTEXT_DEFENCE);
		_staticText[STATICTEXT_LEVEL] = mainGame->env->addStaticText(dataManager.GetSysString(1324), rect<s32>(10, 74, 80, 94), false, false, wFilter, STATICTEXT_LEVEL);
		_staticText[STATICTEXT_SCALE] = mainGame->env->addStaticText(dataManager.GetSysString(1336), rect<s32>(101, 74, 150, 94), false, false, wFilter, STATICTEXT_SCALE);
		_staticText[STATICTEXT_SCALE]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_UPPERLEFT);
		_staticText[STATICTEXT_SEARCH] = mainGame->env->addStaticText(dataManager.GetSysString(1325), rect<s32>(205, 74, 280, 94), false, false, wFilter, STATICTEXT_SEARCH);
		_staticText[STATICTEXT_TYPE] = mainGame->env->addStaticText(dataManager.GetSysString(1321), rect<s32>(10, 51, 70, 71), false, false, wFilter, STATICTEXT_TYPE);
		_staticText[STATICTEXT_SET] = mainGame->env->addStaticText(dataManager.GetSysString(1393), rect<s32>(10, 100, 70, 122), false, false, wFilter, STATICTEXT_SET);
		
		_ComboBox[COMBOBOX_DBLFLIST] = mainGame->env->addComboBox(rect<s32>(80, 5, 220, 30), wDeckEdit, COMBOBOX_DBLFLIST);
		for (unsigned int i = 0; i < deckManager._lfList.size(); ++i)
			_ComboBox[COMBOBOX_DBLFLIST]->addItem(deckManager._lfList[i].listName, deckManager._lfList[i].hash);
		
		_ComboBox[COMBOBOX_DBDECKS] = mainGame->env->addComboBox(rect<s32>(80, 35, 220, 60), wDeckEdit, COMBOBOX_DBDECKS);
		for (unsigned int i = 0; i < deckManager._lfList.size(); ++i)
			_ComboBox[COMBOBOX_DBDECKS]->addItem(deckManager._lfList[i].listName);

		_buttons[BUTTON_SAVE_DECK] = mainGame->env->addButton(rect<s32>(225, 35, 290, 60), wDeckEdit, BUTTON_SAVE_DECK, dataManager.GetSysString(1302));
		_editBox[EDITBOX_DECKNAME] = mainGame->env->addEditBox(L"", rect<s32>(80, 65, 220, 90), true, wDeckEdit, EDITBOX_DECKNAME);
		_editBox[EDITBOX_DECKNAME]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		_buttons[BUTTON_SAVE_DECK_AS] = mainGame->env->addButton(rect<s32>(225, 65, 290, 90), wDeckEdit, BUTTON_SAVE_DECK_AS, dataManager.GetSysString(1303));
		_buttons[BUTTON_CLEAR_DECK] = mainGame->env->addButton(rect<s32>(115, 99, 165, 120), wDeckEdit, BUTTON_CLEAR_DECK, dataManager.GetSysString(1304));
		_buttons[BUTTON_SORT_DECK] = mainGame->env->addButton(rect<s32>(60, 99, 110, 120), wDeckEdit, BUTTON_SORT_DECK, dataManager.GetSysString(1305));
		_buttons[BUTTON_SHUFFLE_DECK] = mainGame->env->addButton(rect<s32>(5, 99, 55, 120), wDeckEdit, BUTTON_SHUFFLE_DECK, dataManager.GetSysString(1307));
		_buttons[BUTTON_DBEXIT] = mainGame->env->addButton(rect<s32>(5, 68, 75, 89), wDeckEdit, BUTTON_DBEXIT, dataManager.GetSysString(1306));
		_buttons[BUTTON_SIDE_OK] = mainGame->env->addButton(rect<s32>(510, 40, 820, 80), 0, BUTTON_SIDE_OK, dataManager.GetSysString(1334));
		_buttons[BUTTON_DELETE_DECK] = mainGame->env->addButton(rect<s32>(225, 95, 290, 120), wDeckEdit, BUTTON_DELETE_DECK, dataManager.GetSysString(2027));

		scrFilter = mainGame->env->addScrollBar(false, recti(999, 161, 1019, 629), 0, SCROLL_FILTER);
		scrFilter->setLargeStep(10);
		scrFilter->setSmallStep(1);
		scrFilter->setVisible(false);
		////sort type
		
		_ComboBox[COMBOBOX_SORTTYPE] = mainGame->env->addComboBox(rect<s32>(10, 2, 85, 22), wSort, COMBOBOX_SORTTYPE);
		_ComboBox[COMBOBOX_SORTTYPE]->setMaxSelectionRows(10);
		for (int i = 1370; i <= 1373; i++)
			_ComboBox[COMBOBOX_SORTTYPE]->addItem(dataManager.GetSysString(i));

		_ComboBox[COMBOBOX_MAINTYPE] = mainGame->env->addComboBox(rect<s32>(60, 3, 120, 23), wFilter, COMBOBOX_MAINTYPE);
		_ComboBox[COMBOBOX_MAINTYPE]->addItem(dataManager.GetSysString(1310));
		_ComboBox[COMBOBOX_MAINTYPE]->addItem(dataManager.GetSysString(1312));
		_ComboBox[COMBOBOX_MAINTYPE]->addItem(dataManager.GetSysString(1313));
		_ComboBox[COMBOBOX_MAINTYPE]->addItem(dataManager.GetSysString(1314));
		_ComboBox[COMBOBOX_MAINTYPE2] = mainGame->env->addComboBox(rect<s32>(130, 3, 190, 23), wFilter, -1);
		_ComboBox[COMBOBOX_MAINTYPE2]->addItem(dataManager.GetSysString(1310), 0);
		_ComboBox[COMBOBOX_LIMIT] = mainGame->env->addComboBox(rect<s32>(260, 3, 390, 23), wFilter, COMBOBOX_LIMIT);
		_ComboBox[COMBOBOX_LIMIT]->addItem(dataManager.GetSysString(1310));
		_ComboBox[COMBOBOX_LIMIT]->addItem(dataManager.GetSysString(1316));
		_ComboBox[COMBOBOX_LIMIT]->addItem(dataManager.GetSysString(1317));
		_ComboBox[COMBOBOX_LIMIT]->addItem(dataManager.GetSysString(1318));
		_ComboBox[COMBOBOX_LIMIT]->addItem(dataManager.GetSysString(1240));
		_ComboBox[COMBOBOX_LIMIT]->addItem(dataManager.GetSysString(1241));
		_ComboBox[COMBOBOX_LIMIT]->addItem(dataManager.GetSysString(2018));
		_ComboBox[COMBOBOX_LIMIT]->addItem(dataManager.GetSysString(2019));
		_ComboBox[COMBOBOX_LIMIT]->addItem(dataManager.GetSysString(2020));
		_ComboBox[COMBOBOX_ATTRIBUTE] = mainGame->env->addComboBox(rect<s32>(60, 26, 190, 46), wFilter, COMBOBOX_ATTRIBUTE);
		_ComboBox[COMBOBOX_ATTRIBUTE]->addItem(dataManager.GetSysString(1310), 0);
		for (int filter = 0x1; filter != 0x80; filter <<= 1)
			_ComboBox[COMBOBOX_ATTRIBUTE]->addItem(dataManager.FormatAttribute(filter), filter);
		
		_ComboBox[COMBOBOX_RACE] = mainGame->env->addComboBox(rect<s32>(60, 49, 190, 69), wFilter, COMBOBOX_RACE);
		_ComboBox[COMBOBOX_RACE]->addItem(dataManager.GetSysString(1310), 0);
		for (int filter = 0x1; filter != 0x1000000; filter <<= 1)
			_ComboBox[COMBOBOX_RACE]->addItem(dataManager.FormatRace(filter), filter);
		
		_editBox[EDITBOX_ATTACK] = mainGame->env->addEditBox(L"", rect<s32>(260, 26, 340, 46), true, wFilter, EDITBOX_ATTACK);
		_editBox[EDITBOX_ATTACK]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		
		_editBox[EDITBOX_DEFENCE] = mainGame->env->addEditBox(L"", rect<s32>(260, 49, 340, 69), true, wFilter, EDITBOX_DEFENCE);
		_editBox[EDITBOX_DEFENCE]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);

		_editBox[EDITBOX_LEVEL] = mainGame->env->addEditBox(L"", rect<s32>(60, 72, 100, 92), true, wFilter, EDITBOX_LEVEL);
		_editBox[EDITBOX_LEVEL]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		
		_editBox[EDITBOX_SCALE] = mainGame->env->addEditBox(L"", rect<s32>(150, 72, 190, 92), true, wFilter, EDITBOX_SCALE);
		_editBox[EDITBOX_SCALE]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
		
		_editBox[EDITBOX_KEYWORD] = mainGame->env->addEditBox(L"", rect<s32>(260, 72, 390, 92), true, wFilter, EDITBOX_KEYWORD);
		_editBox[EDITBOX_KEYWORD]->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);

		_buttons[BUTTON_EFFECT_FILTER] = mainGame->env->addButton(rect<s32>(345, 28, 390, 69), wFilter, BUTTON_EFFECT_FILTER, dataManager.GetSysString(1326));
		_buttons[BUTTON_START_FILTER] = mainGame->env->addButton(rect<s32>(260, 96, 390, 118), wFilter, BUTTON_START_FILTER, dataManager.GetSysString(1327));
		_buttons[BUTTON_CLEAR_FILTER] = mainGame->env->addButton(rect<s32>(205, 96, 255, 118), wFilter, BUTTON_CLEAR_FILTER, dataManager.GetSysString(1304));

		_ComboBox[COMBOBOX_SETCODE] = mainGame->env->addComboBox(rect<s32>(60, 96, 190, 118), wFilter, COMBOBOX_SETCODE);
		_ComboBox[COMBOBOX_SETCODE]->addItem(L"None", 0);
		std::vector<wchar_t*> setcodes = dataManager.GetSetcodeList();
		std::sort(setcodes.begin(), setcodes.end(), DataManager::sort_name);
		for (int i = 0; i < (int)setcodes.size(); ++i)
			_ComboBox[COMBOBOX_SETCODE]->addItem(setcodes[i], dataManager.GetSetcode(setcodes[i]));
		
		wCategories = mainGame->env->addWindow(rect<s32>(450, 60, 1000, 270), false, dataManager.strBuffer);
		wCategories->getCloseButton()->setVisible(false);
		wCategories->setDrawTitlebar(false);
		wCategories->setDraggable(false);
		wCategories->setVisible(false);
		_buttons[BUTTON_CATEGORY_OK] = mainGame->env->addButton(rect<s32>(200, 175, 300, 200), wCategories, BUTTON_CATEGORY_OK, dataManager.GetSysString(1211));
		for (int i = 0; i < 32; ++i)
			chkCategory[i] = mainGame->env->addCheckBox(false, recti(10 + (i % 4) * 130, 10 + (i / 4) * 20, 140 + (i % 4) * 130, 30 + (i / 4) * 20), wCategories, -1, dataManager.GetSysString(1100 + i));
		Hide();
	}

	void GUIDeckEdit::ClearFilter() {
		_ComboBox[COMBOBOX_MAINTYPE]->setSelected(0);
		_ComboBox[COMBOBOX_MAINTYPE2]->setSelected(0);
		_ComboBox[COMBOBOX_ATTRIBUTE]->setSelected(0);
		_ComboBox[COMBOBOX_RACE]->setSelected(0);
		_ComboBox[COMBOBOX_LIMIT]->setSelected(0);
		_ComboBox[COMBOBOX_SETCODE]->setSelected(0);
		_editBox[EDITBOX_ATTACK]->setText(L"");
		_editBox[EDITBOX_DEFENCE]->setText(L"");
		_editBox[EDITBOX_LEVEL]->setText(L"");
		_editBox[EDITBOX_SCALE]->setText(L"");
		filter_effect = 0;
		_editBox[EDITBOX_KEYWORD]->setText(L"");
		for (int i = 0; i < 32; ++i)
			chkCategory[i]->setChecked(false);
		results.clear();
		myswprintf(result_string, L"0");
		_ComboBox[COMBOBOX_MAINTYPE2]->setEnabled(false);
		_ComboBox[COMBOBOX_RACE]->setEnabled(false);
		_ComboBox[COMBOBOX_ATTRIBUTE]->setEnabled(false);
		_editBox[EDITBOX_ATTACK]->setEnabled(false);
		_editBox[EDITBOX_DEFENCE]->setEnabled(false);
		_editBox[EDITBOX_LEVEL]->setEnabled(false);
		_editBox[EDITBOX_SCALE]->setEnabled(false);
	}

	void GUIDeckEdit::OnResize() {

		wDeckEdit->setRelativePosition(mainGame->Resize(309, 8, 605, 130));
		_ComboBox[COMBOBOX_DBLFLIST]->setRelativePosition(mainGame->Resize(80, 5, 220, 30));
		_ComboBox[COMBOBOX_DBDECKS]->setRelativePosition(mainGame->Resize(80, 35, 220, 60));
		_buttons[BUTTON_CLEAR_DECK]->setRelativePosition(mainGame->Resize(115, 99, 165, 120));
		_buttons[BUTTON_SORT_DECK]->setRelativePosition(mainGame->Resize(60, 99, 110, 120));
		_buttons[BUTTON_SHUFFLE_DECK]->setRelativePosition(mainGame->Resize(5, 99, 55, 120));
		_buttons[BUTTON_SAVE_DECK]->setRelativePosition(mainGame->Resize(225, 35, 290, 60));
		_buttons[BUTTON_SAVE_DECK_AS]->setRelativePosition(mainGame->Resize(225, 65, 290, 90));
		_buttons[BUTTON_DBEXIT]->setRelativePosition(mainGame->Resize(5, 68, 75, 89));
		_editBox[EDITBOX_DECKNAME]->setRelativePosition(mainGame->Resize(80, 65, 220, 90));

		wSort->setRelativePosition(mainGame->Resize(930, 132, 1020, 156));
		_ComboBox[COMBOBOX_SORTTYPE]->setRelativePosition(mainGame->Resize(10, 2, 85, 22));
		wFilter->setRelativePosition(mainGame->Resize(610, 8, 1020, 130));
		scrFilter->setRelativePosition(mainGame->Resize(999, 161, 1019, 629));
		_ComboBox[COMBOBOX_MAINTYPE]->setRelativePosition(mainGame->Resize(60, 3, 120, 23));
		_ComboBox[COMBOBOX_MAINTYPE2]->setRelativePosition(mainGame->Resize(130, 3, 190, 23));
		_ComboBox[COMBOBOX_RACE]->setRelativePosition(mainGame->Resize(60, 49, 190, 69));
		_ComboBox[COMBOBOX_ATTRIBUTE]->setRelativePosition(mainGame->Resize(60, 26, 190, 46));
		_ComboBox[COMBOBOX_LIMIT]->setRelativePosition(mainGame->Resize(260, 3, 390, 23));
		_editBox[EDITBOX_LEVEL]->setRelativePosition(mainGame->Resize(60, 72, 100, 92));
		_editBox[EDITBOX_SCALE]->setRelativePosition(mainGame->Resize(150, 72, 190, 92));
		_editBox[EDITBOX_ATTACK]->setRelativePosition(mainGame->Resize(260, 26, 340, 46));
		_editBox[EDITBOX_DEFENCE]->setRelativePosition(mainGame->Resize(260, 49, 340, 69));
		_editBox[EDITBOX_KEYWORD]->setRelativePosition(mainGame->Resize(260, 72, 390, 92));
		_buttons[BUTTON_EFFECT_FILTER]->setRelativePosition(mainGame->Resize(345, 28, 390, 69));
		_buttons[BUTTON_START_FILTER]->setRelativePosition(mainGame->Resize(260, 96, 390, 118));
		_buttons[BUTTON_CLEAR_FILTER]->setRelativePosition(mainGame->Resize(205, 96, 255, 118));
		_ComboBox[COMBOBOX_SETCODE]->setRelativePosition(mainGame->Resize(60, 96, 190, 118));

		_staticText[STATICTEXT_BANLIST]->setRelativePosition(mainGame->ResizeWin(10, 9, 100, 29));
		_staticText[STATICTEXT_DECK]->setRelativePosition(mainGame->ResizeWin(10, 39, 100, 59));
		_staticText[STATICTEXT_CATEGORY]->setRelativePosition(mainGame->ResizeWin(10, 5, 70, 25));
		_staticText[STATICTEXT_LIMIT]->setRelativePosition(mainGame->ResizeWin(205, 5, 280, 25));
		_staticText[STATICTEXT_ATTRIBUTE]->setRelativePosition(mainGame->ResizeWin(10, 28, 70, 48));
		_staticText[STATICTEXT_TYPE]->setRelativePosition(mainGame->ResizeWin(10, 51, 70, 71));
		_staticText[STATICTEXT_ATTACK]->setRelativePosition(mainGame->ResizeWin(205, 28, 280, 48));
		_staticText[STATICTEXT_DEFENCE]->setRelativePosition(mainGame->ResizeWin(205, 51, 280, 71));
		_staticText[STATICTEXT_LEVEL]->setRelativePosition(mainGame->ResizeWin(10, 74, 80, 94));
		_staticText[STATICTEXT_SEARCH]->setRelativePosition(mainGame->ResizeWin(205, 74, 280, 94));
		_staticText[STATICTEXT_SET]->setRelativePosition(mainGame->ResizeWin(10, 100, 70, 122));
		_staticText[STATICTEXT_SCALE]->setRelativePosition(mainGame->ResizeWin(101, 74, 150, 94));

		_buttons[BUTTON_SIDE_OK]->setRelativePosition(mainGame->Resize(510, 40, 820, 80));
		_buttons[BUTTON_DELETE_DECK]->setRelativePosition(mainGame->Resize(225, 95, 290, 120));
	}

	void GUIDeckEdit::ShowDeckEdit() {
		mainGame->RefreshDeck(_ComboBox[COMBOBOX_DBDECKS]);
		if (_ComboBox[COMBOBOX_DBDECKS]->getSelected() != -1)
			deckManager.LoadDeck(_ComboBox[COMBOBOX_DBDECKS]->getItem(_ComboBox[COMBOBOX_DBDECKS]->getSelected()));
		mainGame->is_building = true;
		mainGame->is_siding = false;
		mainGame->wInfoTab.Show();
		mainGame->wInfoTab.ShowImage(true);
		_buttons[BUTTON_SIDE_OK]->setVisible(false);
		filterList = deckManager._lfList[0].content;
		_ComboBox[COMBOBOX_DBLFLIST]->setSelected(0);
		_ComboBox[COMBOBOX_MAINTYPE]->setSelected(0);
		_ComboBox[COMBOBOX_MAINTYPE2]->setSelected(0);
		_ComboBox[COMBOBOX_ATTRIBUTE]->setSelected(0);
		_ComboBox[COMBOBOX_RACE]->setSelected(0);
		_editBox[EDITBOX_ATTACK]->setText(L"");
		_editBox[EDITBOX_DEFENCE]->setText(L"");
		_editBox[EDITBOX_LEVEL]->setText(L"");
		_editBox[EDITBOX_SCALE]->setText(L"");
		_ComboBox[COMBOBOX_MAINTYPE2]->setEnabled(false);
		_ComboBox[COMBOBOX_ATTRIBUTE]->setEnabled(false);
		_ComboBox[COMBOBOX_RACE]->setEnabled(false);
		_editBox[EDITBOX_ATTACK]->setEnabled(false);
		_editBox[EDITBOX_DEFENCE]->setEnabled(false);
		_editBox[EDITBOX_LEVEL]->setEnabled(false);
		_editBox[EDITBOX_SCALE]->setEnabled(false);
		filter_effect = 0;
		result_string[0] = L'0';
		result_string[1] = 0;
		results.clear();
		is_draging = false;
		mainGame->device->setEventReceiver(&mainGame->wEdit);
		for (int i = 0; i < 32; ++i)
			chkCategory[i]->setChecked(false);
		mainGame->PopupElement(wDeckEdit);
		mainGame->PopupElement(wFilter);
		mainGame->PopupElement(wSort);
	}

	void GUIDeckEdit::ShowSiding() {
		mainGame->is_building = false;
		mainGame->is_siding = true;
		wDeckEdit->setVisible(false);
		wFilter->setVisible(false);
		wSort->setVisible(false);
		_buttons[BUTTON_SIDE_OK]->setVisible(true);
	}

	void GUIDeckEdit::Hide() {
		_buttons[BUTTON_SIDE_OK]->setVisible(false);
		mainGame->is_building = false;
		mainGame->is_siding = false;
		wDeckEdit->setVisible(false);
		wCategories->setVisible(false);
		wFilter->setVisible(false);
		mainGame->wInfoTab.ShowImage(false);
		mainGame->wInfoTab.Hide();
		mainGame->wACMessage->setVisible(false);
		imageManager.ClearTexture();
		scrFilter->setVisible(false);
		wSort->setVisible(false);
		if (_ComboBox[COMBOBOX_DBDECKS]->getSelected() != -1) {
			BufferIO::CopyWStr(_ComboBox[COMBOBOX_DBDECKS]->getItem(_ComboBox[COMBOBOX_DBDECKS]->getSelected()), mainGame->gameConf.lastdeck, 64);
		}
	}

}
