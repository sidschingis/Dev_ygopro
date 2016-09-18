#include "GUISingleList.h"
#include "game.h"
#include "deck_manager.h"
#include "duelclient.h"
#include "netserver.h"
#include "image_manager.h"
#include "single_mode.h"

namespace ygo {

	void GUISingleList::Load() {
		IGUIEnvironment* env = mainGame->env;
		wSingleList = env->addWindow(rect<s32>(220, 100, 800, 520), false, dataManager.GetSysString(1201));
		wSingleList->getCloseButton()->setVisible(false);
		wSingleList->setVisible(false);
		_list = env->addListBox(rect<s32>(10, 30, 350, 400), wSingleList, LISTBOX_SINGLEPLAY_LIST, true);
		_list->setItemHeight(18);
		_buttons[BUTTON_LOAD_SINGLEPLAY] = env->addButton(rect<s32>(460, 355, 570, 380), wSingleList, BUTTON_LOAD_SINGLEPLAY, dataManager.GetSysString(1211));
		_buttons[BUTTON_CANCEL_SINGLEPLAY] = env->addButton(rect<s32>(460, 385, 570, 410), wSingleList, BUTTON_CANCEL_SINGLEPLAY, dataManager.GetSysString(1210));
		env->addStaticText(dataManager.GetSysString(1352), rect<s32>(360, 30, 570, 50), false, true, wSingleList);
		_staticText[STATICTEXT_SINGLEINFO] = env->addStaticText(L"", rect<s32>(360, 60, 570, 350), false, true, wSingleList, STATICTEXT_SINGLEINFO);
		
	}

	irr::gui::IGUIElement* GUISingleList::GetBtnElement(unsigned int item) {
		auto cdit = _buttons.find(item);
		if (cdit == _buttons.end())
			return NULL;
		return _buttons[item];
	}

	const wchar_t* GUISingleList::GetText(unsigned int item) {
		auto cdit = _editBox.find(item);
		if (cdit == _editBox.end())
			return L"Value Not Found";
		return _editBox[item]->getText();
	}

	void GUISingleList::SetText(unsigned int item, const wchar_t* text) {
		auto cdit = _editBox.find(item);
		if (cdit != _editBox.end())
			_editBox[item]->setText(text);
	}

	void GUISingleList::SetStaticText(unsigned int item, const wchar_t* text) {
		auto cdit = _staticText.find(item);
		if (cdit != _staticText.end())
			mainGame->SetStaticText(_staticText[item], 180, mainGame->guiFont, text);
	}

	const wchar_t* GUISingleList::GetListData() {
		return _list->getListItem(_list->getSelected());
	}

	int GUISingleList::GetSelected() {
		return _list->getSelected();
	}

	void GUISingleList::SetSelected(const wchar_t* item) {
		_list->setSelected(item);
	}

	void GUISingleList::ClearList() {
		_list->clear();
	}

	void GUISingleList::AddListItem(wchar_t* item) {
		_list->addItem(item);
	}

	void GUISingleList::OnResize() {
		wSingleList->setRelativePosition(mainGame->ResizeWin(220, 100, 800, 520));
	}
	void GUISingleList::Show() {
		mainGame->PopupElement(wSingleList);
		mainGame->device->setEventReceiver(&mainGame->wSingleList);
	}

	void GUISingleList::Hide() {
		mainGame->HideElement(wSingleList);
	}

	bool GUISingleList::OnEvent(const irr::SEvent& event) {
		//handle events here
		switch (event.EventType) {
		case irr::EET_GUI_EVENT: {
			s32 id = event.GUIEvent.Caller->getID();
			switch (event.GUIEvent.EventType) {
			case irr::gui::EGET_BUTTON_CLICKED: {
				switch (id) {
				case BUTTON_LOAD_SINGLEPLAY: {
					if (GetSelected() == -1)
						break;
					mainGame->singleSignal.SetNoWait(false);
					SingleMode::StartPlay();
					break;
				}
				case BUTTON_CANCEL_SINGLEPLAY: {
					Hide();
					mainGame->wMenu.Show();
					break;
				}
				break;
				}
			}
		}
		break;
		}
		break;
		}
		return false;
	}
}
