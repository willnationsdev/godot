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

void StructDB::StructTypeInfo::_add_constant(StructTypeId p_type, StringName p_constant_name, int64_t p_constant_value) {
	constant_data.value[p_constant_name] = p_constant_value;
#ifdef DEBUG_ENABLED
	constant_data.value_ordered.push_back(p_constant_name);
#endif
}

void StructDB::StructTypeInfo::_add_variant_constant(StructTypeId p_type, StringName p_constant_name, const Variant &p_constant_value) {
	constant_data.variant_value[p_constant_name] = p_constant_value;
#ifdef DEBUG_ENABLED
	constant_data.variant_value_ordered.push_back(p_constant_name);
#endif
}

void StructDB::StructTypeInfo::_add_enum_constant(StructTypeId p_type, StringName p_enum_type_name, StringName p_enumeration_name, int p_enum_value) {
	enum_data.value[p_enum_type_name][p_enumeration_name] = p_enum_value;
}

int StructDB::StructTypeInfo::get_length() const {
	int len = 0;
	for (StructPropertyInfo prop : property_list) {
		len += prop.bytes;
	}
	return len;
}

int StructDB::StructTypeInfo::get_variant_length() const {
	return sizeof(Struct::StructPreamble) + get_length();
}

int StructDB::StructTypeInfo::get_capacity() const {
	int capacity = Struct::get_capacity(bucket);
	ERR_FAIL_COND_V_MSG(!capacity, capacity, vformat("Struct type '%s' has invalid bucket size '%d'.", name, bucket));
}

const StructPropertyInfo &StructDB::StructTypeInfo::get_property_info(StructPropertyId p_id) const {
	const StructPropertyInfo *p = property_id_map.getptr(p_id);
	return p ? *p : StructPropertyInfo();
}

const StructPropertyInfo &StructDB::StructTypeInfo::get_property_info(const StringName& p_name) const {
	const StructPropertyInfo *p = property_name_map.getptr(p_name);
	return p ? *p : StructPropertyInfo();
}

void StructDB::StructTypeInfo::assign(Struct *p_self, const Struct *p_value) {
	//Callable *assign = methods.getptr(SNAME("_op_assign"));
	//if (assign) {
	//	assign->callp(p_self, )
	//}
}

bool StructDB::StructTypeInfo::has_method(const StringName &p_name) const {
	return method_map.has(p_name);
}

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

void StructDB::init_struct(Struct* p_struct) {
	StructInstanceInfo instance = _get_validated_struct_info(p_struct);
	if (instance.is_valid()) {
		return;
	}

	int cap = instance.type->get_capacity();
	uint8_t *ptr = instance.data;
	uint8_t *end = ptr + cap;
	List<StructPropertyInfo> props;
	p_struct->get_property_list(&props);
	for (StructPropertyInfo &pi : props) {
		ERR_FAIL_COND_MSG(ptr > end, "Cannot complete struct initialization: values require more memory than capacity. Some data loss may occur.");
		*ptr = pi.id;
		*ptr = pi.default_value;
		ptr += pi.bytes;
	}
}

void StructDB::struct_assign(Struct* p_struct, const Struct* p_other, Error &r_error) {
	StructInstanceInfo instance1 = _get_validated_struct_info(p_struct);
	if (instance1.is_valid()) {
		return;
	}
	StructInstanceInfo instance2 = _get_validated_struct_info(p_struct);
	if (instance2.is_valid()) {
		return;
	}
	//get_struct_parser()->assign(instance1, instance2, r_error);
}

void StructDB::struct_set_property(Struct* p_struct, StructPropertyId p_id, Variant p_value, Error &r_error) {
	StructInstanceInfo instance = _get_validated_struct_info(p_struct);
	if (instance.is_valid()) {
		return;
	}
	//get_struct_parser()->set_property(instance, p_id, p_value, r_error);
}

Variant StructDB::struct_get_property(Struct* p_struct, StructPropertyId p_id) {
	StructInstanceInfo instance = _get_validated_struct_info(p_struct);
	if (instance.is_valid()) {
		return Variant();
	}
	//return get_struct_parser()->get_property(instance, p_id);
}

bool StructDB::struct_has_method(const Struct *p_struct, const StringName &p_name) {
	RWLockRead _r(_lock);

	ERR_FAIL_COND_V_MSG(!p_struct, false, vformat("Struct null while checking for method '%s'.", p_name));
	const StructTypeInfo *type = StructDB::get_struct_type(p_struct->get_type_id());
	ERR_FAIL_COND_V_MSG(!type, false, vformat("Type with id '%d' null while checking for method '%s'.", p_struct->preamble.type_id, p_name));
	return type->has_method(p_name);
}

StructDB::StructInstanceInfo &&StructDB::_get_validated_struct_info(Struct *p_struct) {
	ERR_FAIL_COND_V_MSG(!p_struct, StructInstanceInfo(p_struct), "Struct validation failed: null struct.");
	Struct::StructPreamble *sp = &p_struct->preamble;
	StructTypeId id = p_struct->preamble.type_id;
	StructTypeInfo *type = _types.getptr(id);
	ERR_FAIL_COND_V_MSG(!type, StructInstanceInfo(p_struct, type, sp), vformat("Struct validation failed: unknown type id '%d'.", id));
	uint8_t *data = get_data(*p_struct);
	ERR_FAIL_COND_V_MSG(!data, StructInstanceInfo(p_struct, type, sp), "Struct validation failed: null data.");
	return StructInstanceInfo(p_struct, type, sp, data);
}

void StructDB::get_struct_type_names(Vector<StringName> &r_names) {
	RWLockRead _r(_lock);

	r_names.resize(_types.size());
	int i = 0;
	for (KeyValue<StructTypeId, StructTypeInfo> &pair : _types) {
		r_names.write[i++] = pair.value.name;
	}
}

const StringName& StructDB::get_struct_type_name(StructTypeId p_id) {
	RWLockRead _r(_lock);
	StructTypeInfo *t = _types.getptr(p_id);
	ERR_FAIL_COND_V_MSG(!t, StringName(), vformat("Cannot find struct type with %s '%s'.", "id", p_id));
	return t->name;
}

StructTypeId StructDB::get_struct_type_id(const StringName& p_name) {
	RWLockRead _r(_lock);
	StructTypeId *id = _types_by_name.getptr(p_name);
	return id ? *id : 0;
}

void StructDB::add_struct_type(const StructTypeInfo &p_info) {
	RWLockWrite _w(_lock);
	ERR_FAIL_COND_MSG(_types_by_name.has(p_info.name), vformat("Cannot add duplicate struct type with name '%s'.", p_info.name));
	_types_by_name[p_info.name] = _types.size();
	StructTypeId id = _next_id();
	_types.insert(id, p_info);
}

void StructDB::remove_struct_type(StructTypeId p_id) {
	RWLockWrite _w(_lock);

	StructTypeInfo *info = _types.getptr(p_id);
	_remove_struct_type(info, "id", uitos(p_id));
}

void StructDB::remove_struct_type(const StringName &p_name) {
	RWLockWrite _w(_lock);

	StructTypeId *id = _types_by_name.getptr(p_name);
	if (!id) {
		return;
	}
	StructTypeInfo *info = _types.getptr(*id);
	_remove_struct_type(info, "name", p_name);
}

StructBucket StructDB::get_struct_type_bucket(StructTypeId p_id) {
	RWLockRead _r(_lock);
	StructTypeInfo *t = _types.getptr(p_id);
	return t ? t->bucket : STRUCT_MINIMAL;
}

void StructDB::initialize() {
}

void StructDB::finalize() {
}

void StructDB::_remove_struct_type(const StructTypeInfo *p_info, const char *p_lookup, String p_identifier) {
	ERR_FAIL_COND_MSG(!p_info, vformat("Cannot remove struct type with %s '%s' because it does not exist.", p_lookup, p_identifier));
	_types_by_name.erase(p_info->name);
	_types.erase(p_info->id);
}

const StructDB::StructTypeInfo* StructDB::get_struct_type(StructTypeId p_id) {
	return _types.getptr(p_id);
}

StructTypeId StructDB::_next_id() {
	// TODO: identify a safer way of calculating this so it doesn't take arbitrarily long as slots become occupied.
	ERR_FAIL_COND_MSG(_types.size() < sizeof(StructTypeId), "Unable to create any more struct types.");
	StructTypeId value = _rand.rand();
	while (_types.has(value)) {
		value = _rand.rand();
	}
	return value;
}

StructBucket StructDB::_evaluate_bucket_size(const StructTypeInfo& info) {
	int bytes = sizeof(Struct::StructPreamble);
	for (const StructPropertyInfo &prop : info.property_list) {
		bytes += prop.bytes;
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

StructPropertyId StructDB::_evaluate_property_key(const StructTypeInfo& p_type, const StructPropertyInfo& p_property) {
	ERR_FAIL_COND_V_MSG(p_property.name.is_empty(), 0, "Cannot evaluate key for struct property with an empty name.");
	// TODO: identify how to account for 16-bit unicode characters for RTL languages, etc. TextServer not accessible in core. Alternative?
	for (int i = 0; i < p_property.name.length(); i++) {
		char c = p_property.name.get(i);
		if (!p_type.property_id_map.has(c)) {
			return c;
		}
	}
	// TODO: Devise a more inclusive fallback algorithm.
	for (StructPropertyId c = 'a'; c <= 'z'; c++) {
		if (!p_type.property_id_map.has(c)) {
			return c;
		}
	}
	return 0;
}

/*
void DefaultStructParser::init(StructInstanceInfo &p_info) {
	int cap = p_info.type->get_capacity();
	uint8_t *ptr = p_info.data;
	uint8_t *end = ptr + cap;
	for (StructPropertyId pid : p_info.preamble->property_ids) {
		const StructPropertyInfo &pi = p_info.type->properties[pid];
		ptr += pi.bytes;
	}
	for (KeyValue<StructPropertyId, StructPropertyInfo> pair : p_info.type->properties) {
		ERR_FAIL_COND_MSG(ptr > end, "Cannot complete struct initialization: values require more memory than capacity. Some data loss may occur.");
		*ptr = pair.key;
		*ptr = pair.value.default_value;
		ptr += pair.value.bytes;
	}
}

void DefaultStructParser::clear(StructInstanceInfo &p_info) {
	int cap = p_info.type->get_capacity();
	uint8_t *ptr = p_info.data;
	uint8_t *end = ptr + cap;
	for (KeyValue<StructPropertyId, StructPropertyInfo> pair : p_info.type->properties) {
		ERR_FAIL_COND_MSG(ptr > end, "Cannot completely clear struct: values require more memory than capacity. Some data may remain.");
		*ptr = pair.key;
		*ptr = 0;
		ptr += pair.value.bytes;
	}
}

int DefaultStructParser::try_cast(StructInstanceInfo &p_info, StructInstanceInfo &p_other, Error &r_error) {
	const char *prefix = "_op_cast_";
	Callable *cast_ptr = p_info.type->methods.getptr(prefix + p_other.type->name);
	if (cast_ptr) {
		Variant selfv(*p_info.ref);
		Variant otherv(*p_other.ref);
		const Variant *args[2]{ &selfv, &otherv };
		Variant ret;
		VariantInternal::initialize(&ret, Variant::STRUCT, p_other.type->id);
		Callable::CallError ce;
		cast_ptr->callp(args, 2, ret, ce);
		if (ce.error != Callable::CallError::CALL_OK) {
			r_error = ERR_BUG;
			return -1;
		}
		StructDB::assign_struct(p_info.ref, &ret.operator Struct(), r_error);
		return r_error == OK ? 1 : -1;
	}
	return 0;
}
 
void DefaultStructParser::assign(StructInstanceInfo &p_info, StructInstanceInfo &p_other, Error &r_error) {
	if (p_info.ref == p_other.ref) {
		r_error = OK;
		return;
	}
	StructTypeInfo &type = *p_info.type;
	StructTypeInfo &other_type = *p_other.type;
	ERR_FAIL_COND_MSG(!type.id, "Destination struct missing type during assignment.");
	ERR_FAIL_COND_MSG(!other_type.id, "Source struct missing type during assignment.");
	if (type.id == other_type.id) {
		int cap = type.get_capacity();
		memcpy(p_info.data, p_other.data, cap);
		return;
	}

	int cast_attempted = try_cast(p_info, p_other, r_error);
	if (cast_attempted) {
		return;
	}
	cast_attempted = try_cast(p_other, p_info, r_error);
	if (cast_attempted) {
		return;
	}

	for (KeyValue<StructPropertyId, StructPropertyInfo> pair : type.properties) {
		StructPropertyInfo **other_ptr = other_type.property_name_map.getptr(pair.value.name);
		if (!other_ptr) {
			continue;
		}
		StructPropertyInfo &other = **other_ptr;
		if (pair.value.name != other.name || pair.value.type != other.type || (pair.value.type == Variant::STRUCT && pair.value.struct_type_id != other.struct_type_id)) {
			continue;
		}
		Variant value = get_property(p_other, other.id);
		set_property(p_info, pair.key, value, r_error);
		if (r_error != OK) {
			return;
		}
	}
}

void DefaultStructParser::set_property(StructInstanceInfo &p_instance, StructPropertyId p_id, Variant p_value, Error &r_error) {
	uint8_t *ptr = p_instance.data;
	StructTypeInfo &type = *p_instance.type;
	int idx = 0;

	const StructPropertyInfo *info = type.properties.getptr(p_id);
	if (!info) {
		r_error = ERR_DOES_NOT_EXIST;
		return;
	}
	for (StructPropertyId pid : p_instance.preamble->property_ids) {
		ptr += type.properties[pid].bytes;
	}

	int cap = type.get_capacity() - sizeof(StructPreamble);
	uint8_t *end = p_instance.data + cap;
	int len;
	// TODO: This is not reliable. For example: Callable does nothing and strings are deep copied.
	// Need some way of directly copying the references and/or values stored in Variant, but without
	// having to also copy the type information since we are already storing that in the StructPropertyInfo.
	// We also need to be able to copy it into a buffer which we artificially truncate/compress to just the
	// bytes configured to be allotted for that property.
	Error err = encode_variant_value(p_value, nullptr, len, false);
	if (err != OK) {
		r_error = err;
		return;
	}
	if (ptr + len > end) {
		r_error = ERR_OUT_OF_MEMORY;
		ERR_FAIL_MSG(vformat("Cannot set struct property '%s': would write %d byte(s) beyond capacity of %d byte(s).", type.name, ptr + len - end, cap));
	}

	r_error = encode_variant_value(p_value, ptr, len, false);
}

Variant DefaultStructParser::get_property(const StructInstanceInfo &p_instance, StructPropertyId p_id) {

}*/

