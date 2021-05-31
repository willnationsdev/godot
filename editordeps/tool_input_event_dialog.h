/*************************************************************************/
/*  input_event_configuration_dialog.h                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2021 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2021 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef INPUT_EVENT_CONFIGURATION_DIALOG_H
#define INPUT_EVENT_CONFIGURATION_DIALOG_H

#include "scene/gui/check_box.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/option_button.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/tree.h"

// Confirmation Dialog used when configuring an input event.
// Separate from ActionMapEditor for code cleanliness and separation of responsibilities.
class ToolInputEventDialog : public ConfirmationDialog {
	GDCLASS(ToolInputEventDialog, ConfirmationDialog);

public:
	enum InputType {
		INPUT_KEY = 1,
		INPUT_MOUSE_BUTTON = 2,
		INPUT_JOY_BUTTON = 4,
		INPUT_JOY_MOTION = 8
	};

private:
	struct IconCache {
		Ref<Texture2D> keyboard;
		Ref<Texture2D> mouse;
		Ref<Texture2D> joypad_button;
		Ref<Texture2D> joypad_axis;
	} icon_cache;

	Ref<InputEvent> event = Ref<InputEvent>();

	TabContainer *tab_container;

	// Listening for input
	Label *event_as_text;

	// List of All Key/Mouse/Joypad input options.
	int allowed_input_types;
	Tree *input_list_tree;
	LineEdit *input_list_search;

	// Additional Options, shown depending on event selected
	VBoxContainer *additional_options_container;

	HBoxContainer *device_container;
	OptionButton *device_id_option;

	HBoxContainer *mod_container; // Contains the subcontainer and the store command checkbox.

	enum ModCheckbox {
		MOD_ALT,
		MOD_SHIFT,
		MOD_COMMAND,
		MOD_CTRL,
		MOD_META,
		MOD_MAX
	};
	String mods[MOD_MAX] = { "Alt", "Shift", "Command", "Ctrl", "Metakey" };

	CheckBox *mod_checkboxes[MOD_MAX];
	CheckBox *store_command_checkbox;

	CheckBox *physical_key_checkbox;

	void _set_event(const Ref<InputEvent> &p_event);

	void _tab_selected(int p_tab);
	void _listen_window_input(const Ref<InputEvent> &p_event);

	void _search_term_updated(const String &p_term);
	void _update_input_list();
	void _input_list_item_selected();

	void _mod_toggled(bool p_checked, int p_index);
	void _store_command_toggled(bool p_checked);
	void _physical_keycode_toggled(bool p_checked);

	void _set_current_device(int i_device);
	int _get_current_device() const;
	String _get_device_string(int i_device) const;

protected:
	void _notification(int p_what);

	static void _bind_methods();

public:
	// Pass an existing event to configure it. Alternatively, pass no event to start with a blank configuration.
	void popup_and_configure(const Ref<InputEvent> &p_event = Ref<InputEvent>());
	Ref<InputEvent> get_event() const;
	String get_event_text(const Ref<InputEvent> &p_event);

	int get_allowed_input_types();
	void set_allowed_input_types(int p_type_masks);

	ToolInputEventDialog();
};

#endif
