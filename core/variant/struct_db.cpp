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
#include "core/variant/struct.h"
#include "core/io/marshalls.h"

uint8_t *StructDB::get_data(Struct &p_struct) {
	switch (p_struct.preamble.bucket) {
		case STRUCT_MINIMAL:
			return StructMinimal::get_data(*((StructMinimal*)&p_struct));
		case STRUCT_SMALL:
			return StructSmall::get_data(*((StructSmall*)&p_struct));
		case STRUCT_MEDIUM:
			return StructMedium::get_data(*((StructMedium*)&p_struct));
		case STRUCT_LARGE:
			return StructLarge::get_data(*((StructLarge*)&p_struct));
		default:
			return nullptr;
	}
}

const uint8_t *StructDB::get_data_const(const Struct &p_struct) {
	switch (p_struct.preamble.bucket) {
		case STRUCT_MINIMAL:
			return StructMinimal::get_data(*((StructMinimal*)&p_struct));
		case STRUCT_SMALL:
			return StructSmall::get_data(*((StructSmall*)&p_struct));
		case STRUCT_MEDIUM:
			return StructMedium::get_data(*((StructMedium*)&p_struct));
		case STRUCT_LARGE:
			return StructLarge::get_data(*((StructLarge*)&p_struct));
		default:
			return nullptr;
	}
}

template<class TParser>
void StructDB::init_struct(Struct* p_struct) {
	StructTypeInfo *type = nullptr;
	uint8_t *data = nullptr;
	if (!_validate_struct(p_struct, type, data)) {
		return;
	}
	TParser parser;
	parser.init(*t, p_struct->preamble, data);
}

template<class TParser>
void StructDB::clear_struct(Struct* p_struct) {
	StructTypeInfo *type = nullptr;
	uint8_t *data = nullptr;
	if (!_validate_struct(p_struct, type, data)) {
		return;
	}
	TParser parser;
	parser.clear(*t, p_struct->preamble, data);
}

template<class TParser>
void StructDB::set_struct_property(Struct* p_struct, struct_property_t p_id, Variant p_value, Error &r_error) {
	StructTypeInfo *type = nullptr;
	uint8_t *data = nullptr;
	if (!_validate_struct(p_struct, type, data)) {
		return;
	}
	TParser parser;
	parser.set_property(*type, p_struct->preamble, data, p_id, p_value, r_error);
}

template<class TParser>
Variant StructDB::get_struct_property(Struct* p_struct, struct_property_t p_id) {
	StructTypeInfo *type = nullptr;
	uint8_t *data = nullptr;
	if (!_validate_struct(p_struct, type, data)) {
		return;
	}
	TParser parser;
	return parser.get_property(*type, p_struct->preamble, data, p_id);
}

bool StructDB::_validate_struct(Struct *p_struct, StructTypeInfo *r_type, uint8_t *r_data) {
	r_type = nullptr;
	r_data = nullptr;
	ERR_FAIL_COND_V_MSG(!p_struct, false, "Struct validation failed: null struct.");
	struct_type_t id = p_struct->preamble.type_id;
	r_type = _get_struct_type_ptr(id);
	ERR_FAIL_COND_V_MSG(!r_type, false, vformat("Struct validation failed: unknown type id '%d'.", id));
	r_data = get_data(*p_struct);
	ERR_FAIL_COND_V_MSG(!r_data, false, "Struct validation failed: null data.");
	return true;
}

void StructDB::get_struct_type_names(Vector<StringName> &r_names) {
	RWLockRead _r(_lock);

	r_names.resize(_types.size());
	int i = 0;
	for (KeyValue<struct_type_t, StructTypeInfo> pair : _types) {
		r_names.write[i++] = pair.value.name;
	}
}

void StructDB::add_struct_type(const StructTypeInfo &p_info) {
	RWLockWrite _w(_lock);

	ERR_FAIL_COND_MSG(_name_map.has(p_info.name), vformat("Cannot add duplicate struct type with name '%s'.", p_info.name));
	_name_map[p_info.name] = _types.size();
	struct_type_t id = _next_id();
	_types.insert(id, p_info);
}

void StructDB::remove_struct_type(struct_type_t p_id) {
	RWLockWrite _w(_lock);

	StructTypeInfo *info = _types.getptr(p_id);
	_remove_struct_type(info, "id", uitos(p_id));
}

void StructDB::remove_struct_type(const StringName &p_name) {
	RWLockWrite _w(_lock);

	struct_type_t *id = _name_map.getptr(p_name);
	if (!id) {
		return;
	}
	StructTypeInfo *info = _types.getptr(*id);
	_remove_struct_type(info, "name", p_name);
}

void StructDB::_remove_struct_type(const StructTypeInfo *p_info, const char *p_lookup, String p_identifier) {
	ERR_FAIL_COND_MSG(!p_info, vformat("Cannot remove struct type with %s '%s' because it does not exist.", p_lookup, p_identifier));
	_name_map.erase(p_info->name);
	_types.erase(p_info->id);
}

struct_type_t StructDB::_next_id() {
	// TODO: identify a safer way of calculating this so it doesn't take arbitrarily long as slots become occupied.
	ERR_FAIL_COND_MSG(_types.size() < sizeof(struct_type_t), "Unable to create any more struct types.");
	struct_type_t value = _rand.rand();
	while (_types.has(value)) {
		value = _rand.rand();
	}
	return value;
}

StructBucket StructDB::_evaluate_bucket_size(const StructTypeInfo& info) {
	int bytes = sizeof(StructPreamble);
	for (KeyValue<struct_property_t, StructPropertyInfo> pair : info.properties) {
		bytes += pair.value.bytes;
	}
	if (bytes <= sizeof(int64_t)) {
		return STRUCT_MINIMAL;
	} else if (bytes <= sizeof(AABB)) {
		return STRUCT_SMALL;
	} else if (bytes <= sizeof(Transform3D)) {
		return STRUCT_MEDIUM;
	} else if (bytes <= sizeof(Projection)) {
		return STRUCT_LARGE;
	}
	return STRUCT_MINIMAL;
}

const char StructDB::_fallback_property_keys[] = {
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
};

struct_property_t StructDB::_evaluate_property_key(const StructTypeInfo& p_type, const StructPropertyInfo& p_property) {
	ERR_FAIL_COND_V_MSG(p_property.name.is_empty(), 0, "Cannot evaluate key for struct property with an empty name.");
	// TODO: identify how to account for 16-bit unicode characters for RTL languages, etc. TextServer not accessible in core. Alternative?
	for (int i = 0; i < p_property.name.length(); i++) {
		char c = p_property.name.get(i);
		if (!p_type.properties.has(c)) {
			return c;
		}
	}
	// TODO: Devise a more inclusive fallback algorithm.
	for (char c : _fallback_property_keys) {
		if (!p_type.properties.has(c)) {
			return c;
		}
	}
	return 0;
}

void StructDefaultParser::init(const StructTypeInfo &p_type, const StructPreamble &p_preamble, uint8_t *p_data) {
	uint8_t *end = p_data + p_type.get_capacity();
	uint8_t *ptr = p_data;
	for (struct_property_t pid : p_preamble.property_ids) {
		const StructPropertyInfo &pi = p_type.properties[pid];
		ptr += pi.bytes;
	}
	for (KeyValue<struct_property_t, StructPropertyInfo> pair : p_type.properties) {
		ERR_FAIL_COND_MSG(ptr > end, "Cannot complete struct initialization: values require more memory than capacity. Some data loss may occur.");
		*ptr = pair.key;
		*ptr = pair.value.default_value;
		ptr += pair.value.bytes;
	}
}

void StructDefaultParser::clear(const StructTypeInfo &p_type, const StructPreamble &p_preamble, uint8_t *p_data) {
	uint8_t *end = p_data + p_type.get_capacity();
	uint8_t *ptr = p_data;
	for (KeyValue<struct_property_t, StructPropertyInfo> pair : p_type.properties) {
		ERR_FAIL_COND_MSG(ptr > end, "Cannot completely clear struct: values require more memory than capacity. Some data may remain.");
		*ptr = pair.key;
		*ptr = 0;
		ptr += pair.value.bytes;
	}
}

void StructDefaultParser::set_property(const StructTypeInfo &p_type, const StructPreamble &p_preamble, uint8_t *p_data, struct_property_t p_id, Variant p_value, Error &r_error) {
	uint8_t *ptr = p_data;
	int idx = 0;

	const StructPropertyInfo *info = p_type.properties.getptr(p_id);
	if (!info) {
		r_error = ERR_DOES_NOT_EXIST;
		return;
	}
	for (KeyValue<struct_property_t, StructPropertyInfo> pair : p_type.properties) {
		if (pair.key == p_id) {
			break;
		}
		ptr += pair.value.bytes;
	}

	uint8_t *end = p_data + p_type.get_capacity();
	int len;
	// TODO: This is not reliable. For example: Callable does nothing and strings are deep copied.
	// Need some way of directly copying the references and/or values stored in Variant, but without
	// having to also copy the type information since we are already storing that in the StructPropertyInfo.
	// We also need to be able to copy it into a buffer which we artificially truncate/compress to just the
	// bytes configured to be allotted for that property.
	r_error = encode_variant_value(p_value, nullptr, len, false);
	if (r_error != OK) {
		return;
	}
	if (ptr + len > end) {
		r_error = ERR_OUT_OF_MEMORY;
		ERR_FAIL_MSG(vformat("Cannot set struct property '%s': would write %d byte(s) beyond capacity of %d byte(s).", p_type.name, ptr + len - end, p_type.get_capacity()));
	}

	r_error = encode_variant_value(p_value, ptr, len, false);
}

Variant StructDefaultParser::get_property(const StructTypeInfo &p_info, const StructPreamble &p_preamble, uint8_t *p_data, struct_property_t p_id) {

}

