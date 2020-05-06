/*************************************************************************/
/*  editor_actions.cpp                                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2020 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2020 Godot Engine contributors (cf. AUTHORS.md).   */
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

#include "editor/editor_actions.h"


void EditorActions::add_action(StringName p_name, const Callable& p_callable) {
	ERR_FAIL_COND_MSG(callables.has(p_name), "The EditorAction '" + String(p_name) + "' already exists. Unable to add it.");
	callables[p_name] = p_callable;
}

void EditorActions::add_action_obj(StringName p_name, const Object *p_object, const StringName &p_method) {
	Callable c(p_object, p_method);
	ERR_FAIL_COND_MSG(callables.has(p_name), "The EditorAction '" + String(p_name) + "' already exists. Unable to add it.");
	callables[p_name] = c;
}

void EditorActions::remove_action(StringName p_name) {
	ERR_FAIL_COND_MSG(!callables.has(p_name), "The EditorAction '" + String(p_name) + "' does not exist. Unable to remove it.");
	callables.erase(p_name);
}

void EditorActions::get_action_list(List<StringName>* p_list) {
	callables.get_key_list(p_list);
}

Callable EditorActions::get_action(StringName p_name) {
	ERR_FAIL_COND_V_MSG(!callables.has(p_name), Callable(), "The EditorAction '" + String(p_name) + "' does not exist. Unable to get it.");
	return callables[p_name];
}

Array EditorActions::_get_action_list() {
	Array ret;
	List<StringName> lst;
	get_action_list(&lst);
	for (List<StringName>::Element *E = lst.front(); E; E = E->next()) {
		ret.append(E->get());
	}
	return ret;
}

void EditorActions::_bind_methods() {

	ClassDB::bind_method(D_METHOD("add_action", "name", "callable"), &EditorActions::add_action);
	ClassDB::bind_method(D_METHOD("add_action_obj", "name", "object", "method"), &EditorActions::add_action_obj);
	ClassDB::bind_method(D_METHOD("remove_action", "name"), &EditorActions::remove_action);
	ClassDB::bind_method("get_action_list", &EditorActions::_get_action_list);
	ClassDB::bind_method(D_METHOD("get_action", "name"), &EditorActions::get_action);
}

EditorActions::EditorActions() {}

