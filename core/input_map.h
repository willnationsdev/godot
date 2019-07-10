/*************************************************************************/
/*  input_map.h                                                          */
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

#ifndef INPUT_MAP_H
#define INPUT_MAP_H

#include "core/object.h"
#include "core/os/input_event.h"

class InputMap : public Object {

	GDCLASS(InputMap, Object);

public:
	/**
	* A special value used to signify that a given Action can be triggered by any device
	*/
	static int ALL_DEVICES;

	struct Action {
		int id;
		float deadzone;
		Set<Ref<InputEvent> > inputs;
	};

private:
	static InputMap *singleton;

public:
	typedef Map<StringName, Action> ActionMap;

private:
	struct ActionMapData {
		ActionMap action_map;
		StringName name;
		bool active = true;
	};

	mutable Vector<ActionMapData> action_maps;
	mutable Map<StringName, ActionMapData *> map_names;
	mutable uint32_t map_idx;
	mutable ActionMap composite_map;
	mutable bool composite_map_dirty;

	static int last_id;

	ActionMapData *get_am_data(int p_map_idx) const;
	const ActionMapData *get_am_data_const(int p_map_idx) const;

	Set<Ref<InputEvent> >::Element *_find_event(Action &p_action, const Ref<InputEvent> &p_event, bool *p_pressed = NULL, float *p_strength = NULL) const;

	Array _get_action_list(const StringName &p_action, int p_map_idx = ACTION_MAP_INDEX_CURRENT);
	Array _get_actions(int p_map_idx = ACTION_MAP_INDEX_CURRENT);

protected:
	static void _bind_methods();

public:
	static _FORCE_INLINE_ InputMap *get_singleton() { return singleton; }
	static const int ACTION_MAP_INDEX_CURRENT = -2;
	static const int ACTION_MAP_INDEX_ALL = -1;

	bool has_action(const StringName &p_action, int p_map_idx = ACTION_MAP_INDEX_CURRENT) const;
	List<StringName> get_actions(int p_map_idx = ACTION_MAP_INDEX_CURRENT) const;
	void add_action(const StringName &p_action, float p_deadzone = 0.5, int p_map_idx = ACTION_MAP_INDEX_CURRENT);
	void erase_action(const StringName &p_action, int p_map_idx = ACTION_MAP_INDEX_CURRENT);

	void action_set_deadzone(const StringName &p_action, float p_deadzone, int p_map_idx = ACTION_MAP_INDEX_CURRENT);
	void action_add_event(const StringName &p_action, const Ref<InputEvent> &p_event, int p_map_idx = ACTION_MAP_INDEX_CURRENT);
	bool action_has_event(const StringName &p_action, const Ref<InputEvent> &p_event, int p_map_idx = ACTION_MAP_INDEX_CURRENT);
	void action_erase_event(const StringName &p_action, const Ref<InputEvent> &p_event, int p_map_idx = ACTION_MAP_INDEX_CURRENT);
	void action_erase_events(const StringName &p_action, int p_map_idx = ACTION_MAP_INDEX_CURRENT);

	const Set<Ref<InputEvent> > *get_action_list(const StringName &p_action, int p_map_idx = ACTION_MAP_INDEX_CURRENT);
	bool event_is_action(const Ref<InputEvent> &p_event, const StringName &p_action, int p_map_idx = ACTION_MAP_INDEX_CURRENT) const;
	bool event_get_action_status(const Ref<InputEvent> &p_event, const StringName &p_action, bool *p_pressed = NULL, float *p_strength = NULL, int p_map_idx = ACTION_MAP_INDEX_CURRENT) const;

	const ActionMap *get_action_map_idx(int p_map_idx) const;
	void add_empty_action_map();
	void add_duplicate_action_map(int p_map_idx = ACTION_MAP_INDEX_CURRENT);
	void remove_action_map(int p_map_idx = ACTION_MAP_INDEX_CURRENT);
	void swap_action_maps(int p_first_idx, int p_second_idx);
	void set_current_action_map(int p_map_idx) const;
	void set_action_map_name(const StringName &p_name, int p_map_idx = ACTION_MAP_INDEX_CURRENT);

	const ActionMap &get_action_map() const;
	void load_from_globals();
	void load_default();

	InputMap();
};

#endif // INPUT_MAP_H
