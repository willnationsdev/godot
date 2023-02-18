/**************************************************************************/
/*  struct.cpp                                                            */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "core/variant/struct_db.h"

void StructDB::get_struct_type_names(Vector<StringName> &r_names) {
	RWLockRead _r(_lock);

	r_names.resize(_types.size());
	int i = 0;
	for (KeyValue<StructID, StructDB::StructTypeInfo> pair : _types) {
		r_names.write[i++] = pair.value.name;
	}
}

bool StructDB::try_get_struct_type(StructID p_id, StructTypeInfo *r_info) {
	RWLockRead _r(_lock);

	if (!r_info) {
		return _types.has(p_id);
	}
	r_info = _types.getptr(p_id);
	return r_info != nullptr;
}

bool StructDB::try_get_struct_type(const StringName &p_name, StructTypeInfo *r_info) {
	RWLockRead _r(_lock);

	StructID *id = _name_map.getptr(p_name);
	return id && try_get_struct_type(*id, r_info); // is this safe with the shared mutex read lock?
}

void StructDB::add_struct_type(const StructTypeInfo &p_info) {
	RWLockWrite _w(_lock);

	ERR_FAIL_COND_MSG(_name_map.has(p_info.name), "Cannot add duplicate struct type: " + p_info.name);
	_name_map[p_info.name] = _types.size();
	StructID id = _next_id();
	_types.insert(id, p_info);
}

void StructDB::remove_struct_type(StructID p_id) {
	RWLockWrite _w(_lock);

	StructTypeInfo *info = _types.getptr(p_id);
	ERR_FAIL_COND_MSG(!info, "Failed to remove struct type. Struct type with id '" + itos(p_id) + "' does not exist.");
	_name_map.erase(info->name);
	_types.erase(p_id);
}

StructID StructDB::_next_id() {
	// TODO: identify a safer way of calculating this so it doesn't take arbitrarily long as slots become occupied.
	ERR_FAIL_COND_MSG(_types.size() < sizeof(StructID), "Unable to create any more struct types.");
	StructID value = _rand.rand();
	while (_types.has(value)) {
		value = _rand.rand();
	}
	return value;
}

StructPropertyID StructDB::_next_property_id(StructID p_type_id) {
	// TODO: identify a safer way of calculating this so it doesn't take arbitrarily long as slots become occupied.
	StructTypeInfo *info = nullptr;
	ERR_FAIL_COND_V_MSG(!try_get_struct_type(p_type_id, info), 0, "Unable to generate struct property ID. Given struct ID '" + itos(p_type_id) + "' does not exist.");
	StructPropertyID value = _rand.rand();
	while (info->properties.has(value)) {
		value = _rand.rand();
	}
	return value;
}

uint8_t StructDB::_evaluate_bucket_size(StructTypeInfo& info) {
	int bytes = 0;
	for (KeyValue<StructPropertyID, StructPropertyInfo> pair : info.properties) {
		bytes += pair.value.bytes;
	}
	if (bytes <= sizeof(int64_t)) {
		return 0;
	} else if (bytes <= sizeof(AABB)) {
		return 1;
	} else if (bytes <= sizeof(Transform3D)) {
		return 2;
	} else if (bytes <= sizeof(Projection)) {
		return 3;
	}
	return -1;
}

