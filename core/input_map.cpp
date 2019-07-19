/*************************************************************************/
/*  input_map.cpp                                                        */
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

#include "input_map.h"

#include "core/os/keyboard.h"
#include "core/project_settings.h"

InputMap *InputMap::singleton = NULL;

int InputMap::ALL_DEVICES = -1;

int InputMap::last_id = 1;

void InputMap::_bind_methods() {

	ClassDB::bind_method(D_METHOD("has_action", "action"), &InputMap::has_action);
	ClassDB::bind_method(D_METHOD("get_actions"), &InputMap::_get_actions);
	ClassDB::bind_method(D_METHOD("add_action", "action", "deadzone"), &InputMap::add_action, DEFVAL(0.5f));
	ClassDB::bind_method(D_METHOD("erase_action", "action"), &InputMap::erase_action);

	ClassDB::bind_method(D_METHOD("action_set_deadzone", "action", "deadzone"), &InputMap::action_set_deadzone);
	ClassDB::bind_method(D_METHOD("action_add_event", "action", "event"), &InputMap::action_add_event);
	ClassDB::bind_method(D_METHOD("action_has_event", "action", "event"), &InputMap::action_has_event);
	ClassDB::bind_method(D_METHOD("action_erase_event", "action", "event"), &InputMap::action_erase_event);
	ClassDB::bind_method(D_METHOD("action_erase_events", "action"), &InputMap::action_erase_events);
	ClassDB::bind_method(D_METHOD("get_action_list", "action"), &InputMap::_get_action_list);
	ClassDB::bind_method(D_METHOD("event_is_action", "event", "action"), &InputMap::event_is_action);
	ClassDB::bind_method(D_METHOD("load_from_globals"), &InputMap::load_from_globals);
}

InputMap::ActionMapData *InputMap::get_am_data(int p_map_idx) const {

	ActionMapData *data = NULL;

	if (p_map_idx == ACTION_MAP_INDEX_CURRENT) {
		ERR_FAIL_INDEX_V(map_idx, action_maps.size(), NULL);
		data = &action_maps.write[map_idx];
	} else if (p_map_idx != ACTION_MAP_INDEX_ALL) {
		ERR_FAIL_INDEX_V(p_map_idx, action_maps.size(), NULL);
		data = &action_maps.write[p_map_idx];
	}

	return data;
}

const InputMap::ActionMapData *InputMap::get_am_data_const(int p_map_idx) const {

	const ActionMapData *data = NULL;

	if (p_map_idx == ACTION_MAP_INDEX_CURRENT) {
		ERR_FAIL_INDEX_V(map_idx, action_maps.size(), NULL);
		data = &action_maps[map_idx];
	} else if (p_map_idx != ACTION_MAP_INDEX_ALL) {
		ERR_FAIL_INDEX_V(p_map_idx, action_maps.size(), NULL);
		data = &action_maps[p_map_idx];
	}

	return data;
}

void InputMap::add_action(const StringName &p_action, float p_deadzone, int p_map_idx) {

	ActionMapData *data = get_am_data(p_map_idx);
	ActionMap *map = data ? &data->action_map : NULL;
	if (p_map_idx != ACTION_MAP_INDEX_ALL) {
		ERR_FAIL_COND(!map);
		ERR_FAIL_COND(!map->has(p_action));

		(*map)[p_action] = Action();
		(*map)[p_action].id = last_id;
		(*map)[p_action].deadzone = p_deadzone;
		last_id++;

	} else {
		for (int i = 0; i < action_maps.size(); ++i) {
			data = get_am_data(i);
			map = data ? &data->action_map : NULL;

			if (map && !map->has(p_action)) {
				(*map)[p_action] = Action();
				(*map)[p_action].id = last_id;
				(*map)[p_action].deadzone = p_deadzone;
				last_id++;
			}
		}
	}
	composite_map_dirty = true;
}

void InputMap::erase_action(const StringName &p_action, int p_map_idx) {

	ERR_FAIL_COND(p_map_idx != ACTION_MAP_INDEX_ALL && !action_maps[p_map_idx].action_map.has(p_action));
	if (p_map_idx == ACTION_MAP_INDEX_ALL) {
		for (int i = 0; i < action_maps.size(); ++i) {
			action_maps.write[i].action_map.erase(p_action);
		}
	} else {
		action_maps.write[p_map_idx].action_map.erase(p_action);
	}

	composite_map_dirty = true;
}

Array InputMap::_get_actions(int p_map_idx) {

	Array ret;
	List<StringName> actions = get_actions(p_map_idx);
	if (actions.empty())
		return ret;

	for (const List<StringName>::Element *E = actions.front(); E; E = E->next()) {

		ret.push_back(E->get());
	}

	return ret;
}

List<StringName> InputMap::get_actions(int p_map_idx) const {

	List<StringName> actions = List<StringName>();
	const ActionMapData *data = get_am_data_const(p_map_idx);
	const ActionMap *map = data ? &data->action_map : NULL;

	if (map) {
		if (map->empty()) {
			return actions;
		}

		for (Map<StringName, Action>::Element *E = map->front(); E; E = E->next()) {
			actions.push_back(E->key());
		}
	} else {
		if (action_maps.empty()) {
			return actions;
		}

		Set<StringName> action_set = Set<StringName>();
		for (int i = 0; i < action_maps.size(); ++i) {
			for (const Map<StringName, Action>::Element *E = action_maps[i].action_map.front(); E; E = E->next()) {
				action_set.insert(E->key());
			}
		}
		for (const Set<StringName>::Element *E = action_set.front(); E; E = E->next()) {
			actions.push_back(E->get());
		}
	}

	return actions;
}

Set<Ref<InputEvent> >::Element *InputMap::_find_event(Action &p_action, const Ref<InputEvent> &p_event, bool *p_pressed, float *p_strength) const {

	for (Set<Ref<InputEvent> >::Element *E = p_action.inputs.front(); E; E = E->next()) {

		const Ref<InputEvent> e = E->get();

		//if (e.type != Ref<InputEvent>::KEY && e.device != p_event.device) -- unsure about the KEY comparison, why is this here?
		//	continue;

		int device = e->get_device();
		if (device == ALL_DEVICES || device == p_event->get_device()) {
			if (e->action_match(p_event, p_pressed, p_strength, p_action.deadzone)) {
				return E;
			}
		}
	}

	return NULL;
}

bool InputMap::has_action(const StringName &p_action, int p_map_idx) const {

	if (p_map_idx == ACTION_MAP_INDEX_CURRENT) {
		return action_maps[map_idx].action_map.has(p_action);
	} else if (p_map_idx != ACTION_MAP_INDEX_ALL) {
		return action_maps[p_map_idx].action_map.has(p_action);
	}

	for (int i = 0; i < action_maps.size(); ++i) {
		if (action_maps[i].action_map.has(p_action)) {
			return true;
		}
	}

	return false;
}

void InputMap::action_set_deadzone(const StringName &p_action, float p_deadzone, int p_map_idx) {

	ActionMapData *data = get_am_data(p_map_idx);
	ActionMap *map = data ? &data->action_map : NULL;

	if (map) {
		(*map)[p_action].deadzone = p_deadzone;
	} else {
		for (int i = 0; i < action_maps.size(); ++i) {
			action_maps.write[i].action_map[p_action].deadzone = p_deadzone;
		}
	}

	composite_map_dirty = true;
}

void InputMap::action_add_event(const StringName &p_action, const Ref<InputEvent> &p_event, int p_map_idx) {

	ERR_FAIL_COND(p_event.is_null());

	ActionMapData *data = get_am_data(p_map_idx);
	ActionMap *map = data ? &data->action_map : NULL;

	if (map) {
		ERR_FAIL_COND(!map->has(p_action));
		if (_find_event((*map)[p_action], p_event)) {
			return; //already gots
		}

		(*map)[p_action].inputs.insert(p_event);
	} else {
		for (int i = 0; i < action_maps.size(); ++i) {
			action_maps.write[i].action_map[p_action].inputs.insert(p_event);
		}
	}

	composite_map_dirty = true;
}

bool InputMap::action_has_event(const StringName &p_action, const Ref<InputEvent> &p_event, int p_map_idx) {

	ERR_FAIL_COND_V(p_event.is_null(), false);

	ActionMapData *data = get_am_data(p_map_idx);
	ActionMap *map = data ? &data->action_map : NULL;

	if (p_map_idx == ACTION_MAP_INDEX_CURRENT) {
		map = &action_maps.write[map_idx].action_map;
	} else if (p_map_idx != ACTION_MAP_INDEX_ALL) {
		map = &action_maps.write[p_map_idx].action_map;
	}

	if (map) {
		ERR_FAIL_COND_V(!map->has(p_action), false);

		return (_find_event((*map)[p_action], p_event) != NULL);
	} else {
		for (int i = 0; i < action_maps.size(); ++i) {
			if (_find_event(action_maps.write[i].action_map[p_action], p_event) != NULL)
				return true;
		}
		return false;
	}
}

void InputMap::action_erase_event(const StringName &p_action, const Ref<InputEvent> &p_event, int p_map_idx) {

	ERR_FAIL_COND(p_event.is_null());

	ActionMapData *data = get_am_data(p_map_idx);
	ActionMap *map = data ? &data->action_map : NULL;

	if (map) {
		ERR_FAIL_COND(!map->has(p_action));

		Set<Ref<InputEvent> >::Element *E = _find_event((*map)[p_action], p_event);
		if (E)
			(*map)[p_action].inputs.erase(E);
	} else {
		for (int i = 0; i < action_maps.size(); ++i) {
			Set<Ref<InputEvent> >::Element *E = _find_event(action_maps.write[i].action_map[p_action], p_event);
			if (E)
				action_maps.write[i].action_map[p_action].inputs.erase(E);
		}
	}

	composite_map_dirty = true;
}

void InputMap::action_erase_events(const StringName &p_action, int p_map_idx) {

	ActionMapData *data = get_am_data(p_map_idx);
	ActionMap *map = data ? &data->action_map : NULL;

	if (map) {
		ERR_FAIL_COND(!map->has(p_action));

		(*map)[p_action].inputs.clear();
	} else {
		for (int i = 0; i < action_maps.size(); ++i) {
			action_maps.write[i].action_map[p_action].inputs.clear();
		}
	}

	composite_map_dirty = true;
}

Array InputMap::_get_action_list(const StringName &p_action, int p_map_idx) {

	Array ret;
	const Set<Ref<InputEvent> > *al = get_action_list(p_action, p_map_idx);
	if (al) {
		for (const Set<Ref<InputEvent> >::Element *E = al->front(); E; E = E->next()) {

			ret.push_back(E->get());
		}
	}

	return ret;
}

const Set<Ref<InputEvent> > *InputMap::get_action_list(const StringName &p_action, int p_map_idx) {

	const ActionMapData *data = get_am_data_const(p_map_idx);
	const ActionMap *map = data ? &data->action_map : NULL;

	if (map) {
		const Map<StringName, Action>::Element *E = map->find(p_action);
		if (!E)
			return NULL;

		return &E->get().inputs;
	} else {
		// Returns an existing list, so we won't allocate a new one to return the full list.
		return NULL;
	}
}

bool InputMap::event_is_action(const Ref<InputEvent> &p_event, const StringName &p_action, int p_map_idx) const {
	return event_get_action_status(p_event, p_action, NULL, NULL, p_map_idx);
}

bool InputMap::event_get_action_status(const Ref<InputEvent> &p_event, const StringName &p_action, bool *p_pressed, float *p_strength, int p_map_idx) const {

	ActionMapData *data = get_am_data(p_map_idx);

	if (data) {
		ActionMap::Element *E = data->action_map.find(p_action);
		if (!E) {
			ERR_EXPLAIN("Request for nonexistent InputMap action: " + String(p_action) + " - in map '" + String(data->name) + "'");
			ERR_FAIL_V(false);
		}

		Ref<InputEventAction> input_event_action = p_event;
		if (input_event_action.is_valid()) {
			if (p_pressed != NULL)
				*p_pressed = input_event_action->is_pressed();
			if (p_strength != NULL)
				*p_strength = (*p_pressed) ? input_event_action->get_strength() : 0.0f;
			return input_event_action->get_action() == p_action;
		}

		bool pressed;
		float strength;
		Set<Ref<InputEvent> >::Element *event = _find_event(E->get(), p_event, &pressed, &strength);
		if (event != NULL) {
			if (p_pressed != NULL)
				*p_pressed = pressed;
			if (p_strength != NULL)
				*p_strength = strength;
			return true;
		} else {
			return false;
		}
	} else {
		for (int i = 0; i < action_maps.size(); ++i) {
			if (event_is_action(p_event, p_action, NULL))
				return true;
		}
		return false;
	}
}

const InputMap::ActionMap *InputMap::get_action_map_idx(int p_map_idx) const {
	ERR_FAIL_COND_V(p_map_idx != ACTION_MAP_INDEX_ALL, NULL);
	if (p_map_idx == ACTION_MAP_INDEX_CURRENT) {
		ERR_FAIL_INDEX_V(map_idx, action_maps.size(), NULL);
		return &action_maps[map_idx].action_map;
	} else if (p_map_idx > 0) {
		ERR_FAIL_INDEX_V(p_map_idx, action_maps.size(), NULL);
		return &action_maps[p_map_idx].action_map;
	}
	ERR_FAIL_COND_V(p_map_idx < 0, NULL);
	return NULL;
}

void InputMap::add_empty_action_map() {
	String name = "ActionMap";
	ActionMapData data;

	data.active = true;
	data.name = name;
	int id = 0;
	while (map_names.has(data.name)) {
		data.name = name + itos(id++);
	}

	action_maps.push_back(data);
	map_names[data.name] = &action_maps.write[action_maps.size() - 1];
}

void InputMap::add_duplicate_action_map(int p_map_idx) {
	if (p_map_idx == ACTION_MAP_INDEX_ALL) {
		ERR_EXPLAIN("Using ACTION_MAP_INDEX_ALL to duplicate all action maps is not supported.");
		ERR_FAIL();
	}

	const ActionMapData *data = get_am_data_const(p_map_idx);
	ActionMapData new_data;

	new_data.active = data->active;
	int id = 0;
	do {
		new_data.name = String(data->name) + itos(id++);
	} while (map_names.has(new_data.name));
	new_data.action_map = data->action_map;

	action_maps.push_back(new_data);
	map_names[new_data.name] = &action_maps.write[action_maps.size() - 1];
}

void InputMap::remove_action_map(int p_map_idx) {

	ActionMapData *data = get_am_data(p_map_idx);

	if (!data) {
		action_maps.clear();
		return;
	}

	ERR_FAIL_INDEX(p_map_idx, action_maps.size());
	map_names.erase(data->name);
	action_maps.remove(p_map_idx);

	map_idx--;
	if (map_idx < 0)
		map_idx = 0;

	composite_map_dirty = true;
}

void InputMap::swap_action_maps(int p_first_idx, int p_second_idx) {
	ERR_FAIL_INDEX(p_first_idx, action_maps.size());
	ERR_FAIL_INDEX(p_second_idx, action_maps.size());
	ActionMapData temp = action_maps[p_first_idx];
	action_maps.write[p_first_idx] = action_maps[p_second_idx];
	action_maps.write[p_second_idx] = temp;
}

void InputMap::set_action_map_name(const StringName &p_name, int p_map_idx) {
	ERR_FAIL_COND(map_names.has(p_name));

	ActionMapData *data = get_am_data(p_map_idx);

	if (data) {
		map_names.erase(data->name);
		map_names[p_name] = data;
		data->name = p_name;
	} else {
		ERR_EXPLAIN("Using ACTION_MAP_INDEX_ALL to rename all action maps is not supported.");
		ERR_FAIL();
	}
}

const Map<StringName, InputMap::Action> &InputMap::get_action_map() const {
	// TODO: build final input map
	if (!composite_map_dirty)
		return composite_map;

	composite_map.clear();

	for (int i = 0; i < action_maps.size(); ++i) {
		const ActionMapData &data = action_maps[i];

		if (!data.active)
			continue;

		for (ActionMap::Element *E = data.action_map.front(); E; E = E->next()) {
			bool exists = composite_map.has(E->key());
			if (!exists) {
				composite_map[E->key()] = Action();
			}

			Action &action = composite_map[E->key()];
			if (!exists) {
				action.id = last_id++;
				action.deadzone = E->get().deadzone;
			}

			for (Set<Ref<InputEvent> >::Element *F = E->get().inputs.front(); F; F = F->next()) {
				action.inputs.insert(F->get());
			}
		}
	}

	composite_map_dirty = false;

	return composite_map;
}

void InputMap::load_from_globals() {
	// TODO

	action_maps.clear();
	add_empty_action_map();
	set_map_index(0);

	List<PropertyInfo> pinfo;
	ProjectSettings::get_singleton()->get_property_list(&pinfo);

	for (List<PropertyInfo>::Element *E = pinfo.front(); E; E = E->next()) {
		const PropertyInfo &pi = E->get();

		if (!pi.name.begins_with("input/"))
			continue;

		String name = pi.name.substr(pi.name.find("/") + 1, pi.name.length());

		Dictionary action = ProjectSettings::get_singleton()->get(pi.name);
		float deadzone = action.has("deadzone") ? (float)action["deadzone"] : 0.5f;
		Array events = action["events"];

		add_action(name, deadzone);
		for (int i = 0; i < events.size(); i++) {
			Ref<InputEvent> event = events[i];
			if (event.is_null())
				continue;
			action_add_event(name, event);
		}
	}

	composite_map_dirty = true;
}

void InputMap::load_default() {

	action_maps.clear();
	add_empty_action_map();

	Ref<InputEventKey> key;

	add_action("ui_accept");
	key.instance();
	key->set_scancode(KEY_ENTER);
	action_add_event("ui_accept", key);

	key.instance();
	key->set_scancode(KEY_KP_ENTER);
	action_add_event("ui_accept", key);

	key.instance();
	key->set_scancode(KEY_SPACE);
	action_add_event("ui_accept", key);

	add_action("ui_select");
	key.instance();
	key->set_scancode(KEY_SPACE);
	action_add_event("ui_select", key);

	add_action("ui_cancel");
	key.instance();
	key->set_scancode(KEY_ESCAPE);
	action_add_event("ui_cancel", key);

	add_action("ui_focus_next");
	key.instance();
	key->set_scancode(KEY_TAB);
	action_add_event("ui_focus_next", key);

	add_action("ui_focus_prev");
	key.instance();
	key->set_scancode(KEY_TAB);
	key->set_shift(true);
	action_add_event("ui_focus_prev", key);

	add_action("ui_left");
	key.instance();
	key->set_scancode(KEY_LEFT);
	action_add_event("ui_left", key);

	add_action("ui_right");
	key.instance();
	key->set_scancode(KEY_RIGHT);
	action_add_event("ui_right", key);

	add_action("ui_up");
	key.instance();
	key->set_scancode(KEY_UP);
	action_add_event("ui_up", key);

	add_action("ui_down");
	key.instance();
	key->set_scancode(KEY_DOWN);
	action_add_event("ui_down", key);

	add_action("ui_page_up");
	key.instance();
	key->set_scancode(KEY_PAGEUP);
	action_add_event("ui_page_up", key);

	add_action("ui_page_down");
	key.instance();
	key->set_scancode(KEY_PAGEDOWN);
	action_add_event("ui_page_down", key);

	add_action("ui_home");
	key.instance();
	key->set_scancode(KEY_HOME);
	action_add_event("ui_home", key);

	add_action("ui_end");
	key.instance();
	key->set_scancode(KEY_END);
	action_add_event("ui_end", key);

	//set("display/window/handheld/orientation", "landscape");
	composite_map_dirty = true;
}

InputMap::InputMap() {

	ERR_FAIL_COND(singleton);
	singleton = this;
}
