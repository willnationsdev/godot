/*************************************************************************/
/*  action_map_editor.h                                                  */
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

#ifndef ACTION_MAP_EDITOR_H
#define ACTION_MAP_EDITOR_H

#include "editor/editor_data.h"
#include "editordeps/input_event_configuration_dialog.h"

class ActionMapEditor : public Control {
	GDCLASS(ActionMapEditor, Control);

public:
	struct ActionInfo {
		String name = String();
		Dictionary action = Dictionary();

		Ref<Texture2D> icon = Ref<Texture2D>();
		bool editable = true;
	};

private:
	enum ItemButton {
		BUTTON_ADD_EVENT,
		BUTTON_EDIT_EVENT,
		BUTTON_REMOVE_ACTION,
		BUTTON_REMOVE_EVENT,
	};

	Vector<ActionInfo> actions_cache;
	Tree *action_tree;

	// Storing which action/event is currently being edited in the InputEventConfigurationDialog.

	Dictionary current_action = Dictionary();
	String current_action_name = String();
	int current_action_event_index = -1;

	// Popups

	InputEventConfigurationDialog *event_config_dialog;
	AcceptDialog *message;

	// Filtering and Adding actions

	bool show_builtin_actions;
	CheckButton *show_builtin_actions_checkbutton;
	LineEdit *action_list_search;

	HBoxContainer *add_hbox;
	LineEdit *add_edit;

	void _event_config_confirmed();

	void _add_action_pressed();
	void _add_action(const String &p_name);
	void _action_edited();

	void _tree_button_pressed(Object *p_item, int p_column, int p_id);
	void _tree_item_activated();
	void _search_term_updated(const String &p_search_term);

	Variant get_drag_data_fw(const Point2 &p_point, Control *p_from);
	bool can_drop_data_fw(const Point2 &p_point, const Variant &p_data, Control *p_from) const;
	void drop_data_fw(const Point2 &p_point, const Variant &p_data, Control *p_from);

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	LineEdit *get_search_box() const;
	InputEventConfigurationDialog *get_configuration_dialog();

	// Dictionary represents an Action with "events" (Array) and "deadzone" (float) items. Pass with no param to update list from cached action map.
	void update_action_list(const Vector<ActionInfo> &p_action_infos = Vector<ActionInfo>());
	void show_message(const String &p_message);

	void set_show_builtin_actions(bool p_show);

	void use_external_search_box(LineEdit *p_searchbox);

	ActionMapEditor();
};

#endif
