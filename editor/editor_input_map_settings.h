/*************************************************************************/
/*  editor_input_map_settings.h                                          */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
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

#ifndef EDITOR_INPUT_MAP_SETTINGS_H
#define EDITOR_INPUT_MAP_SETTINGS_H

#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/option_button.h"
#include "scene/gui/tree.h"

class EditorInputMapSettings : public VBoxContainer {

	GDCLASS(EditorInputMapSettings, VBoxContainer);

	enum InputType {
		INPUT_KEY,
		INPUT_JOY_BUTTON,
		INPUT_JOY_MOTION,
		INPUT_MOUSE_BUTTON
	};

	friend class ProjectSettingsEditor;

	InputType add_type;
	String add_at;
	int edit_idx;
	bool *settings_lock; // set externally

	HBoxContainer *toolbar;
	AcceptDialog *message;

	LineEdit *action_name;
	Label *action_add_error;
	Button *action_add;

	Tree *input_editor;
	PopupMenu *popup_add;

	ConfirmationDialog *press_a_key;
	Label *press_a_key_label;
	Ref<InputEventKey> last_wait_for_key;

	ConfirmationDialog *device_input;
	OptionButton *device_id;
	Label *device_index_label;
	OptionButton *device_index;

	void _press_a_key_confirm();
	void _show_last_added(const Ref<InputEvent> &p_event, const String &p_name);

	void _set_current_device(int i_device);
	int _get_current_device();
	String _get_device_string(int i_device);
	void _device_input_add();

	void _action_check(String p_action);
	void _action_adds(String);
	void _action_add();
	void _action_selected();
	void _action_edited();
	void _action_activated();
	void _action_button_pressed(Object *p_obj, int p_column, int p_id);
	void _wait_for_key(const Ref<InputEvent> &p_event);
	void _press_a_key_confirm();
	void _show_last_added(const Ref<InputEvent> &p_event, const String &p_name);
	void _update_actions();

	void _notification(int p_what);

public:
	EditorInputMapSettings();
};

#endif // EDITOR_INPUT_MAP_SETTINGS_H
