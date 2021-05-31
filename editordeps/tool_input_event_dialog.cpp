/*************************************************************************/
/*  input_event_configuration_dialog.cpp                                 */
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

#include "tool_input_event_dialog.h"

#include "core/input/input_map.h"
#include "core/os/keyboard.h"
#include "scene/gui/center_container.h"
#include "scene/gui/separator.h"
#include "tool_scale.h"

/////////////////////////////////////////

// Maps to 2*axis if value is neg, or + 1 if value is pos.
static const char *_joy_axis_descriptions[JOY_AXIS_MAX * 2] = {
	TTRC("Left Stick Left, Joystick 0 Left"),
	TTRC("Left Stick Right, Joystick 0 Right"),
	TTRC("Left Stick Up, Joystick 0 Up"),
	TTRC("Left Stick Down, Joystick 0 Down"),
	TTRC("Right Stick Left, Joystick 1 Left"),
	TTRC("Right Stick Right, Joystick 1 Right"),
	TTRC("Right Stick Up, Joystick 1 Up"),
	TTRC("Right Stick Down, Joystick 1 Down"),
	TTRC("Joystick 2 Left"),
	TTRC("Left Trigger, Sony L2, Xbox LT, Joystick 2 Right"),
	TTRC("Joystick 2 Up"),
	TTRC("Right Trigger, Sony R2, Xbox RT, Joystick 2 Down"),
	TTRC("Joystick 3 Left"),
	TTRC("Joystick 3 Right"),
	TTRC("Joystick 3 Up"),
	TTRC("Joystick 3 Down"),
	TTRC("Joystick 4 Left"),
	TTRC("Joystick 4 Right"),
	TTRC("Joystick 4 Up"),
	TTRC("Joystick 4 Down"),
};

String ToolInputEventDialog::get_event_text(const Ref<InputEvent> &p_event) {
	ERR_FAIL_COND_V_MSG(p_event.is_null(), String(), "Provided event is not a valid instance of InputEvent");

	// Joypad motion events will display slightly differently than what the event->as_text() provides. See #43660.
	Ref<InputEventJoypadMotion> jpmotion = p_event;
	if (jpmotion.is_valid()) {
		String desc = TTR("Unknown Joypad Axis");
		if (jpmotion->get_axis() < JOY_AXIS_MAX) {
			desc = RTR(_joy_axis_descriptions[2 * jpmotion->get_axis() + (jpmotion->get_axis_value() < 0 ? 0 : 1)]);
		}

		return vformat("Joypad Axis %s %s (%s)", itos(jpmotion->get_axis()), jpmotion->get_axis_value() < 0 ? "-" : "+", desc);
	} else {
		return p_event->as_text();
	}
}

void ToolInputEventDialog::_set_event(const Ref<InputEvent> &p_event) {
	if (p_event.is_valid()) {
		event = p_event;

		emit_signal("event_updated", varray(Variant(p_event)));

		// Update Label
		event_as_text->set_text(get_event_text(event));

		Ref<InputEventKey> k = p_event;
		Ref<InputEventMouseButton> mb = p_event;
		Ref<InputEventJoypadButton> joyb = p_event;
		Ref<InputEventJoypadMotion> joym = p_event;
		Ref<InputEventWithModifiers> mod = p_event;

		// Update option values and visibility
		bool show_mods = false;
		bool show_device = false;
		bool show_phys_key = false;

		if (mod.is_valid()) {
			show_mods = true;
			mod_checkboxes[MOD_ALT]->set_pressed(mod->is_alt_pressed());
			mod_checkboxes[MOD_SHIFT]->set_pressed(mod->is_shift_pressed());
			mod_checkboxes[MOD_COMMAND]->set_pressed(mod->is_command_pressed());
			mod_checkboxes[MOD_CTRL]->set_pressed(mod->is_ctrl_pressed());
			mod_checkboxes[MOD_META]->set_pressed(mod->is_meta_pressed());

			store_command_checkbox->set_pressed(mod->is_storing_command());
		}

		if (k.is_valid()) {
			show_phys_key = true;
			physical_key_checkbox->set_pressed(k->get_physical_keycode() != 0 && k->get_keycode() == 0);

		} else if (joyb.is_valid() || joym.is_valid() || mb.is_valid()) {
			show_device = true;
			_set_current_device(event->get_device());
		}

		mod_container->set_visible(show_mods);
		device_container->set_visible(show_device);
		physical_key_checkbox->set_visible(show_phys_key);
		additional_options_container->show();

		// Update selected item in input list for keys, joybuttons and joyaxis only (since the mouse cannot be "listened" for).
		if (k.is_valid() || joyb.is_valid() || joym.is_valid()) {
			TreeItem *category = input_list_tree->get_root()->get_first_child();
			while (category) {
				TreeItem *input_item = category->get_first_child();

				// has_type this should be always true, unless the tree structure has been misconfigured.
				bool has_type = input_item->get_parent()->has_meta("__type");
				int input_type = input_item->get_parent()->get_meta("__type");
				if (!has_type) {
					return;
				}

				// If event type matches input types of this category.
				if ((k.is_valid() && input_type == INPUT_KEY) || (joyb.is_valid() && input_type == INPUT_JOY_BUTTON) || (joym.is_valid() && input_type == INPUT_JOY_MOTION)) {
					// Loop through all items of this category until one matches.
					while (input_item) {
						bool key_match = k.is_valid() && (Variant(k->get_keycode()) == input_item->get_meta("__keycode") || Variant(k->get_physical_keycode()) == input_item->get_meta("__keycode"));
						bool joyb_match = joyb.is_valid() && Variant(joyb->get_button_index()) == input_item->get_meta("__index");
						bool joym_match = joym.is_valid() && Variant(joym->get_axis()) == input_item->get_meta("__axis") && joym->get_axis_value() == (float)input_item->get_meta("__value");
						if (key_match || joyb_match || joym_match) {
							category->set_collapsed(false);
							input_item->select(0);
							input_list_tree->ensure_cursor_is_visible();
							return;
						}
						input_item = input_item->get_next();
					}
				}

				category->set_collapsed(true); // Event not in this category, so collapse;
				category = category->get_next();
			}
		}
	} else {
		// Event is not valid, reset dialog
		event = p_event;
		Vector<String> strings;

		emit_signal("event_cleared");

		// Reset message, prompt for input according to which input types are allowed.
		String text = TTR("Perform an Input (%s).");

		if (allowed_input_types & INPUT_KEY) {
			strings.append(TTR("Key"));
		}
		// We don't check for INPUT_MOUSE_BUTTON since it is ignored in the "Listen Window Input" method.

		if (allowed_input_types & INPUT_JOY_BUTTON) {
			strings.append(TTR("Joypad Button"));
		}
		if (allowed_input_types & INPUT_JOY_MOTION) {
			strings.append(TTR("Joypad Axis"));
		}

		if (strings.size() == 0) {
			text = TTR("Input Event dialog has been misconfigured: No input types are allowed.");
			event_as_text->set_text(text);
		} else {
			String insert_text = String(", ").join(strings);
			event_as_text->set_text(vformat(text, insert_text));
		}

		additional_options_container->hide();
		input_list_tree->deselect_all();
		_update_input_list();
	}
}

void ToolInputEventDialog::_tab_selected(int p_tab) {
	Callable signal_method = callable_mp(this, &ToolInputEventDialog::_listen_window_input);
	if (p_tab == 0) {
		// Start Listening.
		if (!is_connected("window_input", signal_method)) {
			connect("window_input", signal_method);
		}
	} else {
		// Stop Listening.
		if (is_connected("window_input", signal_method)) {
			disconnect("window_input", signal_method);
		}
		input_list_tree->call_deferred("ensure_cursor_is_visible");
		if (input_list_tree->get_selected() == nullptr) {
			// If nothing selected, scroll to top.
			input_list_tree->scroll_to_item(input_list_tree->get_root());
		}
	}
}

void ToolInputEventDialog::_listen_window_input(const Ref<InputEvent> &p_event) {
	// Ignore if echo or not pressed
	if (p_event->is_echo() || !p_event->is_pressed()) {
		return;
	}

	// Ignore mouse
	Ref<InputEventMouse> m = p_event;
	if (m.is_valid()) {
		return;
	}

	// Check what the type is and if it is allowed.
	Ref<InputEventKey> k = p_event;
	Ref<InputEventJoypadButton> joyb = p_event;
	Ref<InputEventJoypadMotion> joym = p_event;

	int type = k.is_valid() ? INPUT_KEY : joyb.is_valid() ? INPUT_JOY_BUTTON :
								  joym.is_valid()		  ? INPUT_JOY_MOTION :
															  0;

	if (!(allowed_input_types & type)) {
		return;
	}

	if (joym.is_valid()) {
		float axis_value = joym->get_axis_value();
		if (ABS(axis_value) < 0.9) {
			// Ignore motion below 0.9 magnitude to avoid accidental touches
			return;
		} else {
			// Always make the value 1 or -1 for display consistency
			joym->set_axis_value(SGN(axis_value));
		}
	}

	if (k.is_valid()) {
		k->set_pressed(false); // to avoid serialisation of 'pressed' property - doesn't matter for actions anyway.
		// Maintain physical keycode option state
		if (physical_key_checkbox->is_pressed()) {
			k->set_physical_keycode(k->get_keycode());
			k->set_keycode(0);
		} else {
			k->set_keycode(k->get_physical_keycode());
			k->set_physical_keycode(0);
		}
	}

	Ref<InputEventWithModifiers> mod = p_event;
	if (mod.is_valid()) {
		// Maintain store command option state
		mod->set_store_command(store_command_checkbox->is_pressed());

		mod->set_window_id(0);
	}

	_set_event(p_event);
	set_input_as_handled();
}

void ToolInputEventDialog::_search_term_updated(const String &) {
	_update_input_list();
}

void ToolInputEventDialog::_update_input_list() {
	input_list_tree->clear();

	TreeItem *root = input_list_tree->create_item();
	String search_term = input_list_search->get_text();

	bool collapse = input_list_search->get_text().is_empty();

	if (allowed_input_types & INPUT_KEY) {
		TreeItem *kb_root = input_list_tree->create_item(root);
		kb_root->set_text(0, TTR("Keyboard Keys"));
		kb_root->set_icon(0, icon_cache.keyboard);
		kb_root->set_collapsed(collapse);
		kb_root->set_meta("__type", INPUT_KEY);

		for (int i = 0; i < keycode_get_count(); i++) {
			String name = keycode_get_name_by_index(i);

			if (!search_term.is_empty() && name.findn(search_term) == -1) {
				continue;
			}

			TreeItem *item = input_list_tree->create_item(kb_root);
			item->set_text(0, name);
			item->set_meta("__keycode", keycode_get_value_by_index(i));
		}
	}

	if (allowed_input_types & INPUT_MOUSE_BUTTON) {
		TreeItem *mouse_root = input_list_tree->create_item(root);
		mouse_root->set_text(0, TTR("Mouse Buttons"));
		mouse_root->set_icon(0, icon_cache.mouse);
		mouse_root->set_collapsed(collapse);
		mouse_root->set_meta("__type", INPUT_MOUSE_BUTTON);

		MouseButton mouse_buttons[9] = { MOUSE_BUTTON_LEFT, MOUSE_BUTTON_RIGHT, MOUSE_BUTTON_MIDDLE, MOUSE_BUTTON_WHEEL_UP, MOUSE_BUTTON_WHEEL_DOWN, MOUSE_BUTTON_WHEEL_LEFT, MOUSE_BUTTON_WHEEL_RIGHT, MOUSE_BUTTON_XBUTTON1, MOUSE_BUTTON_XBUTTON2 };
		for (int i = 0; i < 9; i++) {
			Ref<InputEventMouseButton> mb;
			mb.instance();
			mb->set_button_index(mouse_buttons[i]);
			String desc = get_event_text(mb);

			if (!search_term.is_empty() && desc.findn(search_term) == -1) {
				continue;
			}

			TreeItem *item = input_list_tree->create_item(mouse_root);
			item->set_text(0, desc);
			item->set_meta("__index", mouse_buttons[i]);
		}
	}

	if (allowed_input_types & INPUT_JOY_BUTTON) {
		TreeItem *joyb_root = input_list_tree->create_item(root);
		joyb_root->set_text(0, TTR("Joypad Buttons"));
		joyb_root->set_icon(0, icon_cache.joypad_button);
		joyb_root->set_collapsed(collapse);
		joyb_root->set_meta("__type", INPUT_JOY_BUTTON);

		for (int i = 0; i < JOY_BUTTON_MAX; i++) {
			Ref<InputEventJoypadButton> joyb;
			joyb.instance();
			joyb->set_button_index(i);
			String desc = get_event_text(joyb);

			if (!search_term.is_empty() && desc.findn(search_term) == -1) {
				continue;
			}

			TreeItem *item = input_list_tree->create_item(joyb_root);
			item->set_text(0, desc);
			item->set_meta("__index", i);
		}
	}

	if (allowed_input_types & INPUT_JOY_MOTION) {
		TreeItem *joya_root = input_list_tree->create_item(root);
		joya_root->set_text(0, TTR("Joypad Axes"));
		joya_root->set_icon(0, icon_cache.joypad_axis);
		joya_root->set_collapsed(collapse);
		joya_root->set_meta("__type", INPUT_JOY_MOTION);

		for (int i = 0; i < JOY_AXIS_MAX * 2; i++) {
			int axis = i / 2;
			int direction = (i & 1) ? 1 : -1;
			Ref<InputEventJoypadMotion> joym;
			joym.instance();
			joym->set_axis(axis);
			joym->set_axis_value(direction);
			String desc = get_event_text(joym);

			if (!search_term.is_empty() && desc.findn(search_term) == -1) {
				continue;
			}

			TreeItem *item = input_list_tree->create_item(joya_root);
			item->set_text(0, desc);
			item->set_meta("__axis", i >> 1);
			item->set_meta("__value", (i & 1) ? 1 : -1);
		}
	}
}

void ToolInputEventDialog::_mod_toggled(bool p_checked, int p_index) {
	Ref<InputEventWithModifiers> ie = event;

	// Not event with modifiers
	if (ie.is_null()) {
		return;
	}

	if (p_index == 0) {
		ie->set_alt_pressed(p_checked);
	} else if (p_index == 1) {
		ie->set_shift_pressed(p_checked);
	} else if (p_index == 2) {
		ie->set_command_pressed(p_checked);
	} else if (p_index == 3) {
		ie->set_ctrl_pressed(p_checked);
	} else if (p_index == 4) {
		ie->set_meta_pressed(p_checked);
	}

	_set_event(ie);
}

void ToolInputEventDialog::_store_command_toggled(bool p_checked) {
	Ref<InputEventWithModifiers> ie = event;
	if (ie.is_valid()) {
		ie->set_store_command(p_checked);
		_set_event(ie);
	}

	if (p_checked) {
		// If storing Command, show it's checkbox and hide Control (Win/Lin) or Meta (Mac)
#ifdef APPLE_STYLE_KEYS
		mod_checkboxes[MOD_META]->hide();

		mod_checkboxes[MOD_COMMAND]->show();
		mod_checkboxes[MOD_COMMAND]->set_text("Meta (Command)");
#else
		mod_checkboxes[MOD_CTRL]->hide();

		mod_checkboxes[MOD_COMMAND]->show();
		mod_checkboxes[MOD_COMMAND]->set_text("Control (Command)");
#endif
	} else {
		// If not, hide Command, show Control and Meta.
		mod_checkboxes[MOD_COMMAND]->hide();
		mod_checkboxes[MOD_CTRL]->show();
		mod_checkboxes[MOD_META]->show();
	}
}

void ToolInputEventDialog::_physical_keycode_toggled(bool p_checked) {
	Ref<InputEventKey> k = event;

	if (k.is_null()) {
		return;
	}

	if (p_checked) {
		k->set_physical_keycode(k->get_keycode());
		k->set_keycode(0);
	} else {
		k->set_keycode(k->get_physical_keycode());
		k->set_physical_keycode(0);
	}

	_set_event(k);
}

void ToolInputEventDialog::_input_list_item_selected() {
	TreeItem *selected = input_list_tree->get_selected();

	// Invalid tree selection - type only exists on the "category" items, which are not a valid selection.
	if (selected->has_meta("__type")) {
		return;
	}

	int input_type = selected->get_parent()->get_meta("__type");

	switch (input_type) {
		case ToolInputEventDialog::INPUT_KEY: {
			int kc = selected->get_meta("__keycode");
			Ref<InputEventKey> k;
			k.instance();

			if (physical_key_checkbox->is_pressed()) {
				k->set_physical_keycode(kc);
				k->set_keycode(0);
			} else {
				k->set_physical_keycode(0);
				k->set_keycode(kc);
			}

			// Maintain modifier state from checkboxes
			k->set_alt_pressed(mod_checkboxes[MOD_ALT]->is_pressed());
			k->set_shift_pressed(mod_checkboxes[MOD_SHIFT]->is_pressed());
			k->set_command_pressed(mod_checkboxes[MOD_COMMAND]->is_pressed());
			k->set_ctrl_pressed(mod_checkboxes[MOD_CTRL]->is_pressed());
			k->set_meta_pressed(mod_checkboxes[MOD_META]->is_pressed());
			k->set_store_command(store_command_checkbox->is_pressed());

			_set_event(k);
		} break;
		case ToolInputEventDialog::INPUT_MOUSE_BUTTON: {
			int idx = selected->get_meta("__index");
			Ref<InputEventMouseButton> mb;
			mb.instance();
			mb->set_button_index(idx);
			// Maintain modifier state from checkboxes
			mb->set_alt_pressed(mod_checkboxes[MOD_ALT]->is_pressed());
			mb->set_shift_pressed(mod_checkboxes[MOD_SHIFT]->is_pressed());
			mb->set_command_pressed(mod_checkboxes[MOD_COMMAND]->is_pressed());
			mb->set_ctrl_pressed(mod_checkboxes[MOD_CTRL]->is_pressed());
			mb->set_meta_pressed(mod_checkboxes[MOD_META]->is_pressed());
			mb->set_store_command(store_command_checkbox->is_pressed());

			_set_event(mb);
		} break;
		case ToolInputEventDialog::INPUT_JOY_BUTTON: {
			int idx = selected->get_meta("__index");
			Ref<InputEventJoypadButton> jb = InputEventJoypadButton::create_reference(idx);
			_set_event(jb);
		} break;
		case ToolInputEventDialog::INPUT_JOY_MOTION: {
			int axis = selected->get_meta("__axis");
			int value = selected->get_meta("__value");

			Ref<InputEventJoypadMotion> jm;
			jm.instance();
			jm->set_axis(axis);
			jm->set_axis_value(value);
			_set_event(jm);
		} break;
		default:
			break;
	}
}

void ToolInputEventDialog::_set_current_device(int i_device) {
	device_id_option->select(i_device + 1);
}

int ToolInputEventDialog::_get_current_device() const {
	return device_id_option->get_selected() - 1;
}

String ToolInputEventDialog::_get_device_string(int i_device) const {
	if (i_device == InputMap::ALL_DEVICES) {
		return TTR("All Devices");
	}
	return TTR("Device") + " " + itos(i_device);
}

void ToolInputEventDialog::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE:
		case NOTIFICATION_THEME_CHANGED: {
			input_list_search->set_right_icon(input_list_search->get_theme_icon("Search", "EditorIcons"));

			physical_key_checkbox->set_icon(get_theme_icon("KeyboardPhysical", "EditorIcons"));

			icon_cache.keyboard = get_theme_icon("Keyboard", "EditorIcons");
			icon_cache.mouse = get_theme_icon("Mouse", "EditorIcons");
			icon_cache.joypad_button = get_theme_icon("JoyButton", "EditorIcons");
			icon_cache.joypad_axis = get_theme_icon("JoyAxis", "EditorIcons");

			_update_input_list();
		} break;
		default:
			break;
	}
}

void ToolInputEventDialog::popup_and_configure(const Ref<InputEvent> &p_event) {
	if (p_event.is_valid()) {
		_set_event(p_event);
	} else {
		// Clear Event
		_set_event(p_event);

		// Clear Checkbox Values
		for (int i = 0; i < MOD_MAX; i++) {
			mod_checkboxes[i]->set_pressed(false);
		}
		physical_key_checkbox->set_pressed(false);
		store_command_checkbox->set_pressed(true);
		_set_current_device(0);

		// Switch to "Listen" tab
		tab_container->set_current_tab(0);
	}

	popup_centered();
}

Ref<InputEvent> ToolInputEventDialog::get_event() const {
	return event;
}

int ToolInputEventDialog::get_allowed_input_types() {
	return allowed_input_types;
}

void ToolInputEventDialog::set_allowed_input_types(int p_type_masks) {
	allowed_input_types = p_type_masks;
}

void ToolInputEventDialog::_bind_methods() {
	ClassDB::bind_method(D_METHOD("popup_and_configure", "event"), &ToolInputEventDialog::popup_and_configure);
	ClassDB::bind_method(D_METHOD("get_event"), &ToolInputEventDialog::get_event);
	ClassDB::bind_method(D_METHOD("get_event_text", "event"), &ToolInputEventDialog::get_event_text);

	ClassDB::bind_method(D_METHOD("get_allowed_input_types"), &ToolInputEventDialog::get_allowed_input_types);
	ClassDB::bind_method(D_METHOD("set_allowed_input_types", "flags"), &ToolInputEventDialog::set_allowed_input_types);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "allowed_input_types", PROPERTY_HINT_FLAGS, "Key, Mouse (Ignored), Joypad Button, Joypad Axis", PROPERTY_USAGE_DEFAULT_INTL), "set_allowed_input_types", "get_allowed_input_types");

	ADD_SIGNAL(MethodInfo("event_updated", PropertyInfo(Variant::OBJECT, "event")));
	ADD_SIGNAL(MethodInfo("event_cleared"));
}

ToolInputEventDialog::ToolInputEventDialog() {
	allowed_input_types = INPUT_KEY | INPUT_MOUSE_BUTTON | INPUT_JOY_BUTTON | INPUT_JOY_MOTION;

	set_title("Event Configuration");
	set_min_size(Size2i(550 * EDSCALE, 0)); // Min width

	VBoxContainer *main_vbox = memnew(VBoxContainer);
	add_child(main_vbox);

	tab_container = memnew(TabContainer);
	tab_container->set_tab_align(TabContainer::TabAlign::ALIGN_LEFT);
	tab_container->set_use_hidden_tabs_for_min_size(true);
	tab_container->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	tab_container->connect("tab_selected", callable_mp(this, &ToolInputEventDialog::_tab_selected));
	main_vbox->add_child(tab_container);

	CenterContainer *cc = memnew(CenterContainer);
	cc->set_name("Listen for Input");
	event_as_text = memnew(Label);
	event_as_text->set_align(Label::ALIGN_CENTER);
	cc->add_child(event_as_text);
	tab_container->add_child(cc);

	// List of all input options to manually select from.

	VBoxContainer *manual_vbox = memnew(VBoxContainer);
	manual_vbox->set_name("Manual Selection");
	manual_vbox->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	tab_container->add_child(manual_vbox);

	input_list_search = memnew(LineEdit);
	input_list_search->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	input_list_search->set_placeholder(TTR("Filter Inputs"));
	input_list_search->set_clear_button_enabled(true);
	input_list_search->connect("text_changed", callable_mp(this, &ToolInputEventDialog::_search_term_updated));
	manual_vbox->add_child(input_list_search);

	input_list_tree = memnew(Tree);
	input_list_tree->set_custom_minimum_size(Size2(0, 100 * EDSCALE)); // Min height for tree
	input_list_tree->connect("item_selected", callable_mp(this, &ToolInputEventDialog::_input_list_item_selected));
	input_list_tree->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	manual_vbox->add_child(input_list_tree);

	input_list_tree->set_hide_root(true);
	input_list_tree->set_columns(1);

	_update_input_list();

	// Additional Options
	additional_options_container = memnew(VBoxContainer);
	additional_options_container->hide();

	Label *opts_label = memnew(Label);
	opts_label->set_text("Additional Options");
	additional_options_container->add_child(opts_label);

	// Device Selection
	device_container = memnew(HBoxContainer);
	device_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);

	Label *device_label = memnew(Label);
	device_label->set_text("Device:");
	device_container->add_child(device_label);

	device_id_option = memnew(OptionButton);
	device_id_option->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	device_container->add_child(device_id_option);

	for (int i = -1; i < 8; i++) {
		device_id_option->add_item(_get_device_string(i));
	}
	_set_current_device(0);
	device_container->hide();
	additional_options_container->add_child(device_container);

	// Modifier Selection
	mod_container = memnew(HBoxContainer);
	for (int i = 0; i < MOD_MAX; i++) {
		String name = mods[i];
		mod_checkboxes[i] = memnew(CheckBox);
		mod_checkboxes[i]->connect("toggled", callable_mp(this, &ToolInputEventDialog::_mod_toggled), varray(i));
		mod_checkboxes[i]->set_text(name);
		mod_container->add_child(mod_checkboxes[i]);
	}

	mod_container->add_child(memnew(VSeparator));

	store_command_checkbox = memnew(CheckBox);
	store_command_checkbox->connect("toggled", callable_mp(this, &ToolInputEventDialog::_store_command_toggled));
	store_command_checkbox->set_pressed(true);
	store_command_checkbox->set_text(TTR("Store Command"));
#ifdef APPLE_STYLE_KEYS
	store_command_checkbox->set_tooltip(TTR("Toggles between serializing 'command' and 'meta'. Used for compatibility with Windows/Linux style keyboard."));
#else
	store_command_checkbox->set_tooltip(TTR("Toggles between serializing 'command' and 'control'. Used for compatibility with Apple Style keyboards."));
#endif
	mod_container->add_child(store_command_checkbox);

	mod_container->hide();
	additional_options_container->add_child(mod_container);

	// Physical Key Checkbox

	physical_key_checkbox = memnew(CheckBox);
	physical_key_checkbox->set_text(TTR("Use Physical Keycode"));
	physical_key_checkbox->set_tooltip(TTR("Stores the physical position of the key on the keyboard rather than the keys value. Used for compatibility with non-latin layouts."));
	physical_key_checkbox->connect("toggled", callable_mp(this, &ToolInputEventDialog::_physical_keycode_toggled));
	physical_key_checkbox->hide();
	additional_options_container->add_child(physical_key_checkbox);

	main_vbox->add_child(additional_options_container);

	// Default to first tab
	tab_container->set_current_tab(0);
}

/////////////////////////////////////////

static bool _is_action_name_valid(const String &p_name) {
	const char32_t *cstr = p_name.get_data();
	for (int i = 0; cstr[i]; i++) {
		if (cstr[i] == '/' || cstr[i] == ':' || cstr[i] == '"' ||
				cstr[i] == '=' || cstr[i] == '\\' || cstr[i] < 32) {
			return false;
		}
	}
	return true;
}
