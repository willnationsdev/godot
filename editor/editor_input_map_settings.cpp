/*************************************************************************/
/*  editor_input_map_settings.cpp                                        */
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

#include "editor/editor_input_map_settings.h"

#include "core/input_map.h"
#include "core/os/keyboard.h"
#include "editor/editor_node.h"
#include "editor/editor_scale.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"

static const char *_button_names[JOY_BUTTON_MAX] = {
	"DualShock Cross, Xbox A, Nintendo B",
	"DualShock Circle, Xbox B, Nintendo A",
	"DualShock Square, Xbox X, Nintendo Y",
	"DualShock Triangle, Xbox Y, Nintendo X",
	"L, L1",
	"R, R1",
	"L2",
	"R2",
	"L3",
	"R3",
	"Select, Nintendo -",
	"Start, Nintendo +",
	"D-Pad Up",
	"D-Pad Down",
	"D-Pad Left",
	"D-Pad Right"
};

static const char *_axis_names[JOY_AXIS_MAX * 2] = {
	" (Left Stick Left)",
	" (Left Stick Right)",
	" (Left Stick Up)",
	" (Left Stick Down)",
	" (Right Stick Left)",
	" (Right Stick Right)",
	" (Right Stick Up)",
	" (Right Stick Down)",
	"", "", "", "",
	"", " (L2)",
	"", " (R2)"
};

static bool _validate_action_name(const String &p_name) {
	const CharType *cstr = p_name.c_str();
	for (int i = 0; cstr[i]; i++)
		if (cstr[i] == '/' || cstr[i] == ':' || cstr[i] == '"' ||
				cstr[i] == '=' || cstr[i] == '\\' || cstr[i] < 32)
			return false;
	return true;
}

void EditorInputMapSettings::_action_selected() {

	TreeItem *ti = input_editor->get_selected();
	if (!ti || !ti->is_editable(0))
		return;

	add_at = "input/" + itos(InputMap::get_singleton()->get_map_index()) + "/" + ti->get_text(0);
	edit_idx = -1;
}

void EditorInputMapSettings::_action_edited() {

	TreeItem *ti = input_editor->get_selected();
	if (!ti)
		return;

	if (input_editor->get_selected_column() == 0) {

		String new_name = ti->get_text(0);
		String old_name = add_at.substr(add_at.find("/") + 1, add_at.length());

		if (new_name == old_name)
			return;

		if (new_name == "" || !_validate_action_name(new_name)) {

			ti->set_text(0, old_name);
			add_at = "input/" + old_name;

			message->set_text(TTR("Invalid action name. it cannot be empty nor contain '/', ':', '=', '\\' or '\"'"));
			message->popup_centered(Size2(300, 100) * EDSCALE);
			return;
		}

		String action_prop = "input/" + new_name;

		if (ProjectSettings::get_singleton()->has_setting(action_prop)) {

			ti->set_text(0, old_name);
			add_at = "input/" + old_name;

			message->set_text(vformat(TTR("An action with the name '%s' already exists."), new_name));
			message->popup_centered(Size2(300, 100) * EDSCALE);
			return;
		}

		int order = ProjectSettings::get_singleton()->get_order(add_at);
		Dictionary action = ProjectSettings::get_singleton()->get(add_at);

		UndoRedo *undo_redo = EditorNode::get_singleton()->get_undo_redo();

		ERR_FAIL_COND(!settings_lock);

		*settings_lock = true;
		undo_redo->create_action(TTR("Rename Input Action Event"));
		undo_redo->add_do_method(ProjectSettings::get_singleton(), "clear", add_at);
		undo_redo->add_do_method(ProjectSettings::get_singleton(), "set", action_prop, action);
		undo_redo->add_do_method(ProjectSettings::get_singleton(), "set_order", action_prop, order);
		undo_redo->add_undo_method(ProjectSettings::get_singleton(), "clear", action_prop);
		undo_redo->add_undo_method(ProjectSettings::get_singleton(), "set", add_at, action);
		undo_redo->add_undo_method(ProjectSettings::get_singleton(), "set_order", add_at, order);
		undo_redo->add_do_method(this, "_update_actions");
		undo_redo->add_undo_method(this, "_update_actions");
		undo_redo->add_do_method(this, "_settings_changed");
		undo_redo->add_undo_method(this, "_settings_changed");
		undo_redo->commit_action();
		*settings_lock = false;

		add_at = action_prop;
	} else if (input_editor->get_selected_column() == 1) {

		String name = "input/" + ti->get_text(0);
		Dictionary old_action = ProjectSettings::get_singleton()->get(name);
		Dictionary new_action = old_action.duplicate();
		new_action["deadzone"] = ti->get_range(1);

		UndoRedo *undo_redo = EditorNode::get_singleton()->get_undo_redo();

		ERR_FAIL_COND(!settings_lock);

		*settings_lock = true;
		undo_redo->create_action(TTR("Change Action deadzone"));
		undo_redo->add_do_method(ProjectSettings::get_singleton(), "set", name, new_action);
		undo_redo->add_do_method(this, "_settings_changed");
		undo_redo->add_undo_method(ProjectSettings::get_singleton(), "set", name, old_action);
		undo_redo->add_undo_method(this, "_settings_changed");
		undo_redo->commit_action();
		*settings_lock = false; // TODO: note that these setting assignments were added
	}
}

void EditorInputMapSettings::_device_input_add() {

	Ref<InputEvent> ie;
	String name = add_at;
	int idx = edit_idx;
	Dictionary old_val = ProjectSettings::get_singleton()->get(name);
	Dictionary action = old_val.duplicate();
	Array events = action["events"];

	switch (add_type) {

		case INPUT_MOUSE_BUTTON: {

			Ref<InputEventMouseButton> mb;
			mb.instance();
			mb->set_button_index(device_index->get_selected() + 1);
			mb->set_device(_get_current_device());

			for (int i = 0; i < events.size(); i++) {

				Ref<InputEventMouseButton> aie = events[i];
				if (aie.is_null())
					continue;
				if (aie->get_device() == mb->get_device() && aie->get_button_index() == mb->get_button_index()) {
					return;
				}
			}

			ie = mb;

		} break;
		case INPUT_JOY_MOTION: {

			Ref<InputEventJoypadMotion> jm;
			jm.instance();
			jm->set_axis(device_index->get_selected() >> 1);
			jm->set_axis_value((device_index->get_selected() & 1) ? 1 : -1);
			jm->set_device(_get_current_device());

			for (int i = 0; i < events.size(); i++) {

				Ref<InputEventJoypadMotion> aie = events[i];
				if (aie.is_null())
					continue;

				if (aie->get_device() == jm->get_device() && aie->get_axis() == jm->get_axis() && aie->get_axis_value() == jm->get_axis_value()) {
					return;
				}
			}

			ie = jm;

		} break;
		case INPUT_JOY_BUTTON: {

			Ref<InputEventJoypadButton> jb;
			jb.instance();

			jb->set_button_index(device_index->get_selected());
			jb->set_device(_get_current_device());

			for (int i = 0; i < events.size(); i++) {

				Ref<InputEventJoypadButton> aie = events[i];
				if (aie.is_null())
					continue;
				if (aie->get_device() == jb->get_device() && aie->get_button_index() == jb->get_button_index()) {
					return;
				}
			}
			ie = jb;

		} break;
		default: {
		}
	}

	if (idx < 0 || idx >= events.size()) {
		events.push_back(ie);
	} else {
		events[idx] = ie;
	}
	action["events"] = events;

	UndoRedo *undo_redo = EditorNode::get_singleton()->get_undo_redo();

	ERR_FAIL_COND(!settings_lock);

	*settings_lock = true;
	undo_redo->create_action(TTR("Add Input Action Event"));
	undo_redo->add_do_method(ProjectSettings::get_singleton(), "set", name, action);
	undo_redo->add_undo_method(ProjectSettings::get_singleton(), "set", name, old_val);
	undo_redo->add_do_method(this, "_update_actions");
	undo_redo->add_undo_method(this, "_update_actions");
	undo_redo->add_do_method(this, "_settings_changed");
	undo_redo->add_undo_method(this, "_settings_changed");
	undo_redo->commit_action();
	*settings_lock = false;

	_show_last_added(ie, name);
}

void EditorInputMapSettings::_set_current_device(int i_device) {
	device_id->select(i_device + 1);
}

int EditorInputMapSettings::_get_current_device() {
	return device_id->get_selected() - 1;
}

String EditorInputMapSettings::_get_device_string(int i_device) {
	if (i_device == InputMap::ALL_DEVICES)
		return TTR("All Devices");
	return TTR("Device") + " " + itos(i_device);
}

void EditorInputMapSettings::_press_a_key_confirm() {

	if (last_wait_for_key.is_null())
		return;

	Ref<InputEventKey> ie;
	ie.instance();
	ie->set_scancode(last_wait_for_key->get_scancode());
	ie->set_shift(last_wait_for_key->get_shift());
	ie->set_alt(last_wait_for_key->get_alt());
	ie->set_control(last_wait_for_key->get_control());
	ie->set_metakey(last_wait_for_key->get_metakey());

	String name = add_at;
	int idx = edit_idx;

	Dictionary old_val = ProjectSettings::get_singleton()->get(name);
	Dictionary action = old_val.duplicate();
	Array events = action["events"];

	for (int i = 0; i < events.size(); i++) {

		Ref<InputEventKey> aie = events[i];
		if (aie.is_null())
			continue;
		if (aie->get_scancode_with_modifiers() == ie->get_scancode_with_modifiers()) {
			return;
		}
	}

	if (idx < 0 || idx >= events.size()) {
		events.push_back(ie);
	} else {
		events[idx] = ie;
	}
	action["events"] = events;

	UndoRedo *undo_redo = EditorNode::get_singleton()->get_undo_redo();

	ERR_FAIL_COND(!settings_lock);

	*settings_lock = true;
	undo_redo->create_action(TTR("Add Input Action Event"));
	undo_redo->add_do_method(ProjectSettings::get_singleton(), "set", name, action);
	undo_redo->add_undo_method(ProjectSettings::get_singleton(), "set", name, old_val);
	undo_redo->add_do_method(this, "_update_actions");
	undo_redo->add_undo_method(this, "_update_actions");
	undo_redo->add_do_method(this, "_settings_changed");
	undo_redo->add_undo_method(this, "_settings_changed");
	undo_redo->commit_action();
	*settings_lock = false;

	_show_last_added(ie, name);
}

void EditorInputMapSettings::_show_last_added(const Ref<InputEvent> &p_event, const String &p_name) {
	TreeItem *r = input_editor->get_root();

	String name = p_name;
	name.erase(0, 6);
	if (!r)
		return;
	r = r->get_children();
	if (!r)
		return;
	bool found = false;
	while (r) {
		if (r->get_text(0) != name) {
			r = r->get_next();
			continue;
		}
		TreeItem *child = r->get_children();
		while (child) {
			Variant input = child->get_meta("__input");
			if (p_event == input) {
				r->set_collapsed(false);
				child->select(0);
				found = true;
				break;
			}
			child = child->get_next();
		}
		if (found) break;
		r = r->get_next();
	}

	if (found) input_editor->ensure_cursor_is_visible();
}

void EditorInputMapSettings::_update_actions() {

	ERR_FAIL_COND(!settings_lock);

	if (*settings_lock)
		return;

	Map<String, bool> collapsed;

	if (input_editor->get_root() && input_editor->get_root()->get_children()) {
		for (TreeItem *item = input_editor->get_root()->get_children(); item; item = item->get_next()) {
			collapsed[item->get_text(0)] = item->is_collapsed();
		}
	}

	input_editor->clear();
	TreeItem *root = input_editor->create_item();
	input_editor->set_hide_root(true);

	List<PropertyInfo> props;
	ProjectSettings::get_singleton()->get_property_list(&props);

	for (List<PropertyInfo>::Element *E = props.front(); E; E = E->next()) {

		const PropertyInfo &pi = E->get();
		if (!pi.name.begins_with("input/"))
			continue;

		String name = pi.name.get_slice("/", 1);
		if (name == "")
			continue;

		Dictionary action = ProjectSettings::get_singleton()->get(pi.name);
		Array events = action["events"];

		TreeItem *item = input_editor->create_item(root);
		item->set_text(0, name);
		item->set_custom_bg_color(0, get_color("prop_subsection", "Editor"));
		if (collapsed.has(name))
			item->set_collapsed(collapsed[name]);

		item->set_editable(1, true);
		item->set_cell_mode(1, TreeItem::CELL_MODE_RANGE);
		item->set_range_config(1, 0.0, 1.0, 0.01);
		item->set_range(1, action["deadzone"]);
		item->set_custom_bg_color(1, get_color("prop_subsection", "Editor"));

		item->add_button(2, get_icon("Add", "EditorIcons"), 1, false, TTR("Add Event"));
		if (!ProjectSettings::get_singleton()->get_input_presets().find(pi.name)) {
			item->add_button(2, get_icon("Remove", "EditorIcons"), 2, false, TTR("Remove"));
			item->set_editable(0, true);
		}

		for (int i = 0; i < events.size(); i++) {

			Ref<InputEvent> event = events[i];
			if (event.is_null())
				continue;

			TreeItem *action2 = input_editor->create_item(item);

			Ref<InputEventKey> k = event;
			if (k.is_valid()) {

				String str = keycode_get_string(k->get_scancode()).capitalize();
				if (k->get_metakey())
					str = vformat("%s+", find_keycode_name(KEY_META)) + str;
				if (k->get_shift())
					str = TTR("Shift+") + str;
				if (k->get_alt())
					str = TTR("Alt+") + str;
				if (k->get_control())
					str = TTR("Control+") + str;

				action2->set_text(0, str);
				action2->set_icon(0, get_icon("Keyboard", "EditorIcons"));
			}

			Ref<InputEventJoypadButton> jb = event;

			if (jb.is_valid()) {

				String str = _get_device_string(jb->get_device()) + ", " + TTR("Button") + " " + itos(jb->get_button_index());
				if (jb->get_button_index() >= 0 && jb->get_button_index() < JOY_BUTTON_MAX)
					str += String() + " (" + _button_names[jb->get_button_index()] + ").";
				else
					str += ".";

				action2->set_text(0, str);
				action2->set_icon(0, get_icon("JoyButton", "EditorIcons"));
			}

			Ref<InputEventMouseButton> mb = event;

			if (mb.is_valid()) {
				String str = _get_device_string(mb->get_device()) + ", ";
				switch (mb->get_button_index()) {
					case BUTTON_LEFT: str += TTR("Left Button."); break;
					case BUTTON_RIGHT: str += TTR("Right Button."); break;
					case BUTTON_MIDDLE: str += TTR("Middle Button."); break;
					case BUTTON_WHEEL_UP: str += TTR("Wheel Up."); break;
					case BUTTON_WHEEL_DOWN: str += TTR("Wheel Down."); break;
					default: str += TTR("Button") + " " + itos(mb->get_button_index()) + ".";
				}

				action2->set_text(0, str);
				action2->set_icon(0, get_icon("Mouse", "EditorIcons"));
			}

			Ref<InputEventJoypadMotion> jm = event;

			if (jm.is_valid()) {

				int ax = jm->get_axis();
				int n = 2 * ax + (jm->get_axis_value() < 0 ? 0 : 1);
				String desc = _axis_names[n];
				String str = _get_device_string(jm->get_device()) + ", " + TTR("Axis") + " " + itos(ax) + " " + (jm->get_axis_value() < 0 ? "-" : "+") + desc + ".";
				action2->set_text(0, str);
				action2->set_icon(0, get_icon("JoyAxis", "EditorIcons"));
			}
			action2->set_metadata(0, i);
			action2->set_meta("__input", event);

			action2->add_button(2, get_icon("Edit", "EditorIcons"), 3, false, TTR("Edit"));
			action2->add_button(2, get_icon("Remove", "EditorIcons"), 2, false, TTR("Remove"));
		}
	}

	_action_check(action_name->get_text());
}

void EditorInputMapSettings::_notification(int p_what) {

	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			action_add_error->add_color_override("font_color", get_color("error_color", "Editor"));

			_update_actions();
			popup_add->add_icon_item(get_icon("Keyboard", "EditorIcons"), TTR("Key "), INPUT_KEY); //"Key " - because the word 'key' has already been used as a key animation
			popup_add->add_icon_item(get_icon("JoyButton", "EditorIcons"), TTR("Joy Button"), INPUT_JOY_BUTTON);
			popup_add->add_icon_item(get_icon("JoyAxis", "EditorIcons"), TTR("Joy Axis"), INPUT_JOY_MOTION);
			popup_add->add_icon_item(get_icon("Mouse", "EditorIcons"), TTR("Mouse Button"), INPUT_MOUSE_BUTTON);
		}
	}
}

EditorInputMapSettings::EditorInputMapSettings() {
	set_name(TTR("Input Map"));

	set_anchor_and_margin(MARGIN_TOP, ANCHOR_BEGIN, 0);
	set_anchor_and_margin(MARGIN_BOTTOM, ANCHOR_END, 0);
	set_anchor_and_margin(MARGIN_LEFT, ANCHOR_BEGIN, 0);
	set_anchor_and_margin(MARGIN_RIGHT, ANCHOR_END, 0);

	message = memnew(AcceptDialog);
	add_child(message);

	toolbar = memnew(HBoxContainer);
	add_child(toolbar);

	Label *l = memnew(Label);
	toolbar->add_child(l);
	l->set_text(TTR("Action:"));

	action_name = memnew(LineEdit);
	action_name->set_h_size_flags(SIZE_EXPAND_FILL);
	toolbar->add_child(action_name);
	action_name->connect("text_entered", this, "_action_adds");
	action_name->connect("text_changed", this, "_action_check");

	action_add_error = memnew(Label);
	toolbar->add_child(action_add_error);
	action_add_error->hide();

	Button *b = memnew(Button);
	toolbar->add_child(b);
	b->set_text(TTR("Add"));
	b->set_disabled(true);
	b->connect("pressed", this, "_action_b");
	action_add = b;

	input_editor = memnew(Tree);
	add_child(input_editor);
	input_editor->set_v_size_flags(SIZE_EXPAND_FILL);
	input_editor->set_columns(3);
	input_editor->set_column_titles_visible(true);
	input_editor->set_column_title(0, TTR("Action"));
	input_editor->set_column_title(1, TTR("Deadzone"));
	input_editor->set_column_expand(1, false);
	input_editor->set_column_min_width(1, 80 * EDSCALE);
	input_editor->set_column_expand(2, false);
	input_editor->set_column_min_width(2, 50 * EDSCALE);
	input_editor->connect("item_edited", this, "_action_edited");
	input_editor->connect("item_activated", this, "_action_activated");
	input_editor->connect("cell_selected", this, "_action_selected");
	input_editor->connect("button_pressed", this, "_action_button_pressed");
	popup_add = memnew(PopupMenu);
	add_child(popup_add);
	popup_add->connect("id_pressed", this, "_add_item");

	press_a_key = memnew(ConfirmationDialog);
	press_a_key->set_focus_mode(FOCUS_ALL);
	add_child(press_a_key);

	l = memnew(Label);
	l->set_text(TTR("Press a Key..."));
	l->set_anchors_and_margins_preset(Control::PRESET_WIDE);
	l->set_align(Label::ALIGN_CENTER);
	l->set_margin(MARGIN_TOP, 20);
	l->set_anchor_and_margin(MARGIN_BOTTOM, ANCHOR_BEGIN, 30);
	press_a_key_label = l;
	press_a_key->add_child(l);
	press_a_key->connect("gui_input", this, "_wait_for_key");
	press_a_key->connect("confirmed", this, "_press_a_key_confirm");

	device_input = memnew(ConfirmationDialog);
	add_child(device_input);
	device_input->get_ok()->set_text(TTR("Add"));
	device_input->connect("confirmed", this, "_device_input_add");

	HBoxContainer *hbc = memnew(HBoxContainer);
	device_input->add_child(hbc);

	VBoxContainer *vbc_left = memnew(VBoxContainer);
	hbc->add_child(vbc_left);

	l = memnew(Label);
	l->set_text(TTR("Device:"));
	vbc_left->add_child(l);

	device_id = memnew(OptionButton);
	for (int i = -1; i < 8; i++)
		device_id->add_item(_get_device_string(i));
	_set_current_device(0);
	vbc_left->add_child(device_id);

	VBoxContainer *vbc_right = memnew(VBoxContainer);
	hbc->add_child(vbc_right);
	vbc_right->set_h_size_flags(SIZE_EXPAND_FILL);

	l = memnew(Label);
	l->set_text(TTR("Index:"));
	vbc_right->add_child(l);
	device_index_label = l;

	device_index = memnew(OptionButton);
	vbc_right->add_child(device_index);
}
