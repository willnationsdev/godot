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

#include "core/io/marshalls.h"
#include "core/object/script_language.h"
#include "core/variant/struct_db.h"
#include "core/variant/struct.h"

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
	return Struct::get_capacity(bucket);
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
	RWLockRead _r(_lock);

	StructInstanceInfo self = _get_validated_struct_info(p_struct);
	if (self.is_valid()) {
		return;
	}
	StructInstanceInfo other = _get_validated_struct_info(p_struct);
	if (other.is_valid()) {
		return;
	}
	if (self.ref == other.ref) {
		r_error = OK;
		return;
	}
	StructTypeInfo &type = *self.type;
	StructTypeInfo &other_type = *other.type;
	ERR_FAIL_COND_MSG(!type.id, "Destination struct missing type during assignment.");
	ERR_FAIL_COND_MSG(!other_type.id, "Source struct missing type during assignment.");
	if (type.id == other_type.id) {
		int cap = type.get_capacity();
		memcpy(self.data, other.data, cap);
		return;
	}

	r_error = try_cast(self, other);
	if (r_error != OK) {
		return;
	}
	r_error = try_cast(other, self);
	if (r_error != OK) {
		return;
	}

	for (StructPropertyInfo &info : type.property_list) {
		StructPropertyInfo *prop_other = other_type.property_name_map.getptr(info.name);
		if (!prop_other) {
			continue;
		}
		StructPropertyInfo &po = *prop_other;
		if (info.name != po.name || info.type != po.type || (info.type == Variant::STRUCT && info.struct_type_id != po.struct_type_id)) {
			continue;
		}
		Variant value = struct_get_property(p_other, po.id);
		struct_set_property(p_info, info.id, value, r_error);
		if (r_error != OK) {
			return;
		}
	}
}

Error StructDB::try_cast(StructInstanceInfo &p_info, StructInstanceInfo &p_other) {
	Error err;
	const char *prefix = "_op_cast_";
	MethodBind **cast_ptr = p_info.type->method_map.getptr(prefix + p_other.type->name);
	Ref<Script> scr = p_info.type->script;
	if (cast_ptr && scr.is_valid()) {
		Variant selfv(*p_info.ref);
		Variant otherv(*p_other.ref);
		const Variant *args[2]{ &selfv, &otherv };
		Variant ret;
		VariantInternal::initialize(&ret, Variant::STRUCT, p_other.type->id);
		Callable::CallError ce;
		ret = (*cast_ptr)->call(*scr, args, 2, ce);
		if (ce.error != Callable::CallError::CALL_OK) {
			return ERR_BUG;
		}
		StructDB::struct_assign(p_info.ref, &ret.operator Struct(), err);
		return err;
	}
	return ERR_DOES_NOT_EXIST;
}

void StructDB::struct_set_property(Struct* p_struct, StructPropertyId p_id, Variant p_value, Error &r_error) {
	RWLockRead _r(_lock);

	StructInstanceInfo instance = _get_validated_struct_info(p_struct);
	if (instance.is_valid()) {
		return;
	}
	//get_struct_parser()->set_property(instance, p_id, p_value, r_error);
}

Variant StructDB::struct_get_property(Struct* p_struct, StructPropertyId p_id, Error &r_error) {
	RWLockRead _r(_lock);

	ERR_FAIL_COND_V(!p_struct, Variant());
	StructInstanceInfo instance = _get_validated_struct_info(p_struct);
	if (instance.is_valid()) {
		return Variant();
	}

	const uint8_t *ptr = p_struct->get_data_const();
	StructTypeInfo &type = *get_struct_type(p_struct->get_type_id());
	int cap = type.get_capacity() - sizeof(Struct::StructPreamble);
	const uint8_t *end = ptr + cap;

	int idx = 0;

	const StructPropertyInfo *info = type.property_id_map.getptr(p_id);
	if (!info) {
		r_error = ERR_DOES_NOT_EXIST;
		return;
	}
	for (StructPropertyId pid : p_struct->preamble.property_ids) {
		if (info->id == pid) {
			break;
		}
		ptr += type.property_id_map[pid].bytes;
	}

	int len;
	// TODO: This is not reliable. For example: Callable does nothing and strings are deep copied.
	// Need some way of directly copying the references and/or values stored in Variant, but without
	// having to also copy the type information since we are already storing that in the StructPropertyInfo.
	// We also need to be able to copy it into a buffer which we artificially truncate/compress to just the
	// bytes configured to be allotted for that property.
	StructBuffer buf(p_struct);
	buf.put
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

Variant StructDB::struct_get_property_const(const Struct* p_struct, StructPropertyId p_id) {
	RWLockRead _r(_lock);

	const StructInstanceInfo instance = _get_validated_struct_info(p_struct);
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
	StructTypeInfo *type = get_struct_type(id);
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
	StructTypeInfo *info = get_struct_type(p_id);
	_remove_struct_type(info, "id", uitos(p_id));
}

void StructDB::remove_struct_type(const StringName &p_name) {
	StructTypeId id = get_struct_type_id(p_name);
	if (!id) {
		return;
	}
	StructTypeInfo *info = get_struct_type(id);
	_remove_struct_type(info, "name", p_name);
}

void StructDB::_remove_struct_type(const StructTypeInfo *p_info, const char *p_lookup, String p_identifier) {
	RWLockWrite _w(_lock);
	ERR_FAIL_COND_MSG(!p_info, vformat("Cannot remove struct type with %s '%s' because it does not exist.", p_lookup, p_identifier));
	_types_by_name.erase(p_info->name);
	_types.erase(p_info->id);
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

StructDB::StructTypeInfo* StructDB::get_struct_type(StructTypeId p_id) {
	RWLockRead _r(_lock);
	return _types.getptr(p_id);
}

const StructDB::StructTypeInfo* StructDB::get_struct_type_const(const StructTypeId p_id) {
	RWLockRead _r(_lock);
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

uint8_t *StructBuffer::_get_data() {
	return StructDB::get_data(*_struct);
}

void StructBuffer::set_big_endian(bool p_big_endian) {
	big_endian = p_big_endian;
}

bool StructBuffer::is_big_endian_enabled() const {
	return big_endian;
}

void StructBuffer::put_u8(uint8_t p_val) {
	put_data((const uint8_t *)&p_val, 1);
}

void StructBuffer::put_8(int8_t p_val) {
	put_data((const uint8_t *)&p_val, 1);
}

void StructBuffer::put_u16(uint16_t p_val) {
	if (big_endian) {
		p_val = BSWAP16(p_val);
	}
	uint8_t buf[2];
	encode_uint16(p_val, buf);
	put_data(buf, 2);
}

void StructBuffer::put_16(int16_t p_val) {
	if (big_endian) {
		p_val = BSWAP16(p_val);
	}
	uint8_t buf[2];
	encode_uint16(p_val, buf);
	put_data(buf, 2);
}

void StructBuffer::put_u32(uint32_t p_val) {
	if (big_endian) {
		p_val = BSWAP32(p_val);
	}
	uint8_t buf[4];
	encode_uint32(p_val, buf);
	put_data(buf, 4);
}

void StructBuffer::put_32(int32_t p_val) {
	if (big_endian) {
		p_val = BSWAP32(p_val);
	}
	uint8_t buf[4];
	encode_uint32(p_val, buf);
	put_data(buf, 4);
}

void StructBuffer::put_u64(uint64_t p_val) {
	if (big_endian) {
		p_val = BSWAP64(p_val);
	}
	uint8_t buf[8];
	encode_uint64(p_val, buf);
	put_data(buf, 8);
}

void StructBuffer::put_64(int64_t p_val) {
	if (big_endian) {
		p_val = BSWAP64(p_val);
	}
	uint8_t buf[8];
	encode_uint64(p_val, buf);
	put_data(buf, 8);
}

void StructBuffer::put_float(float p_val) {
	uint8_t buf[4];

	encode_float(p_val, buf);
	if (big_endian) {
		uint32_t *p32 = (uint32_t *)buf;
		*p32 = BSWAP32(*p32);
	}

	put_data(buf, 4);
}

void StructBuffer::put_double(double p_val) {
	uint8_t buf[8];
	encode_double(p_val, buf);
	if (big_endian) {
		uint64_t *p64 = (uint64_t *)buf;
		*p64 = BSWAP64(*p64);
	}
	put_data(buf, 8);
}

template <class T>
void StructBuffer::_put_ptr(const T *p_ptr) {
	uint8_t buf[sizeof(T *)];
	if (sizeof(T *) == 4) {
		put_u32(p_ptr);
	} else if (sizeof(T *) == 8) {
		put_u64(p_ptr);
	}
	put_data(buf, sizeof(T *));
}

void StructBuffer::put_string(const String &p_string) {
	_put_ptr<String>(&p_string);
}

void StructBuffer::put_var(const Variant &p_variant, bool p_full_objects) {
	int len = 0;
	Vector<uint8_t> buf;
	encode_variant(p_variant, nullptr, len, p_full_objects);
	buf.resize(len);
	put_32(len);
	encode_variant(p_variant, buf.ptrw(), len, p_full_objects);
	put_data(buf.ptr(), buf.size());
}

#define ENCODE_MASK 0xFF
Variant StructBuffer::get_value(Variant::Type p_type, Error& r_error) {
	const uint8_t *buf = _get_data();
	int len = p_len;

	ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);

	uint32_t type = p_type;

	ERR_FAIL_COND_V((type & ENCODE_MASK) >= Variant::VARIANT_MAX, ERR_INVALID_DATA);

	buf += 4;
	len -= 4;
	if (r_len) {
		*r_len = 4;
	}

	// Note: We cannot use sizeof(real_t) for decoding, in case a different size is encoded.
	// Decoding math types always checks for the encoded size, while encoding always uses compilation setting.
	// This does lead to some code duplication for decoding, but compatibility is the priority.
	switch (type & ENCODE_MASK) {
		case Variant::NIL: {
			r_variant = Variant();
		} break;
		case Variant::BOOL: {
			ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);
			bool val = decode_uint32(buf);
			r_variant = val;
			if (r_len) {
				(*r_len) += 4;
			}
		} break;
		case Variant::INT: {
			if (type & ENCODE_FLAG_64) {
				ERR_FAIL_COND_V(len < 8, ERR_INVALID_DATA);
				int64_t val = decode_uint64(buf);
				r_variant = val;
				if (r_len) {
					(*r_len) += 8;
				}

			} else {
				ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);
				int32_t val = decode_uint32(buf);
				r_variant = val;
				if (r_len) {
					(*r_len) += 4;
				}
			}

		} break;
		case Variant::FLOAT: {
			if (type & ENCODE_FLAG_64) {
				ERR_FAIL_COND_V((size_t)len < sizeof(double), ERR_INVALID_DATA);
				double val = decode_double(buf);
				r_variant = val;
				if (r_len) {
					(*r_len) += sizeof(double);
				}
			} else {
				ERR_FAIL_COND_V((size_t)len < sizeof(float), ERR_INVALID_DATA);
				float val = decode_float(buf);
				r_variant = val;
				if (r_len) {
					(*r_len) += sizeof(float);
				}
			}

		} break;
		case Variant::STRING: {
			String str;
			Error err = _decode_string(buf, len, r_len, str);
			if (err) {
				return err;
			}
			r_variant = str;

		} break;

		// math types
		case Variant::VECTOR2: {
			Vector2 val;
			if (type & ENCODE_FLAG_64) {
				ERR_FAIL_COND_V((size_t)len < sizeof(double) * 2, ERR_INVALID_DATA);
				val.x = decode_double(&buf[0]);
				val.y = decode_double(&buf[sizeof(double)]);

				if (r_len) {
					(*r_len) += sizeof(double) * 2;
				}
			} else {
				ERR_FAIL_COND_V((size_t)len < sizeof(float) * 2, ERR_INVALID_DATA);
				val.x = decode_float(&buf[0]);
				val.y = decode_float(&buf[sizeof(float)]);

				if (r_len) {
					(*r_len) += sizeof(float) * 2;
				}
			}
			r_variant = val;

		} break;
		case Variant::VECTOR2I: {
			ERR_FAIL_COND_V(len < 4 * 2, ERR_INVALID_DATA);
			Vector2i val;
			val.x = decode_uint32(&buf[0]);
			val.y = decode_uint32(&buf[4]);
			r_variant = val;

			if (r_len) {
				(*r_len) += 4 * 2;
			}

		} break;
		case Variant::RECT2: {
			Rect2 val;
			if (type & ENCODE_FLAG_64) {
				ERR_FAIL_COND_V((size_t)len < sizeof(double) * 4, ERR_INVALID_DATA);
				val.position.x = decode_double(&buf[0]);
				val.position.y = decode_double(&buf[sizeof(double)]);
				val.size.x = decode_double(&buf[sizeof(double) * 2]);
				val.size.y = decode_double(&buf[sizeof(double) * 3]);

				if (r_len) {
					(*r_len) += sizeof(double) * 4;
				}
			} else {
				ERR_FAIL_COND_V((size_t)len < sizeof(float) * 4, ERR_INVALID_DATA);
				val.position.x = decode_float(&buf[0]);
				val.position.y = decode_float(&buf[sizeof(float)]);
				val.size.x = decode_float(&buf[sizeof(float) * 2]);
				val.size.y = decode_float(&buf[sizeof(float) * 3]);

				if (r_len) {
					(*r_len) += sizeof(float) * 4;
				}
			}
			r_variant = val;

		} break;
		case Variant::RECT2I: {
			ERR_FAIL_COND_V(len < 4 * 4, ERR_INVALID_DATA);
			Rect2i val;
			val.position.x = decode_uint32(&buf[0]);
			val.position.y = decode_uint32(&buf[4]);
			val.size.x = decode_uint32(&buf[8]);
			val.size.y = decode_uint32(&buf[12]);
			r_variant = val;

			if (r_len) {
				(*r_len) += 4 * 4;
			}

		} break;
		case Variant::VECTOR3: {
			Vector3 val;
			if (type & ENCODE_FLAG_64) {
				ERR_FAIL_COND_V((size_t)len < sizeof(double) * 3, ERR_INVALID_DATA);
				val.x = decode_double(&buf[0]);
				val.y = decode_double(&buf[sizeof(double)]);
				val.z = decode_double(&buf[sizeof(double) * 2]);

				if (r_len) {
					(*r_len) += sizeof(double) * 3;
				}
			} else {
				ERR_FAIL_COND_V((size_t)len < sizeof(float) * 3, ERR_INVALID_DATA);
				val.x = decode_float(&buf[0]);
				val.y = decode_float(&buf[sizeof(float)]);
				val.z = decode_float(&buf[sizeof(float) * 2]);

				if (r_len) {
					(*r_len) += sizeof(float) * 3;
				}
			}
			r_variant = val;

		} break;
		case Variant::VECTOR3I: {
			ERR_FAIL_COND_V(len < 4 * 3, ERR_INVALID_DATA);
			Vector3i val;
			val.x = decode_uint32(&buf[0]);
			val.y = decode_uint32(&buf[4]);
			val.z = decode_uint32(&buf[8]);
			r_variant = val;

			if (r_len) {
				(*r_len) += 4 * 3;
			}

		} break;
		case Variant::VECTOR4: {
			Vector4 val;
			if (type & ENCODE_FLAG_64) {
				ERR_FAIL_COND_V((size_t)len < sizeof(double) * 4, ERR_INVALID_DATA);
				val.x = decode_double(&buf[0]);
				val.y = decode_double(&buf[sizeof(double)]);
				val.z = decode_double(&buf[sizeof(double) * 2]);
				val.w = decode_double(&buf[sizeof(double) * 3]);

				if (r_len) {
					(*r_len) += sizeof(double) * 4;
				}
			} else {
				ERR_FAIL_COND_V((size_t)len < sizeof(float) * 4, ERR_INVALID_DATA);
				val.x = decode_float(&buf[0]);
				val.y = decode_float(&buf[sizeof(float)]);
				val.z = decode_float(&buf[sizeof(float) * 2]);
				val.w = decode_float(&buf[sizeof(float) * 3]);

				if (r_len) {
					(*r_len) += sizeof(float) * 4;
				}
			}
			r_variant = val;

		} break;
		case Variant::VECTOR4I: {
			ERR_FAIL_COND_V(len < 4 * 4, ERR_INVALID_DATA);
			Vector4i val;
			val.x = decode_uint32(&buf[0]);
			val.y = decode_uint32(&buf[4]);
			val.z = decode_uint32(&buf[8]);
			val.w = decode_uint32(&buf[12]);
			r_variant = val;

			if (r_len) {
				(*r_len) += 4 * 4;
			}

		} break;
		case Variant::TRANSFORM2D: {
			Transform2D val;
			if (type & ENCODE_FLAG_64) {
				ERR_FAIL_COND_V((size_t)len < sizeof(double) * 6, ERR_INVALID_DATA);
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 2; j++) {
						val.columns[i][j] = decode_double(&buf[(i * 2 + j) * sizeof(double)]);
					}
				}

				if (r_len) {
					(*r_len) += sizeof(double) * 6;
				}
			} else {
				ERR_FAIL_COND_V((size_t)len < sizeof(float) * 6, ERR_INVALID_DATA);
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 2; j++) {
						val.columns[i][j] = decode_float(&buf[(i * 2 + j) * sizeof(float)]);
					}
				}

				if (r_len) {
					(*r_len) += sizeof(float) * 6;
				}
			}
			r_variant = val;

		} break;
		case Variant::PLANE: {
			Plane val;
			if (type & ENCODE_FLAG_64) {
				ERR_FAIL_COND_V((size_t)len < sizeof(double) * 4, ERR_INVALID_DATA);
				val.normal.x = decode_double(&buf[0]);
				val.normal.y = decode_double(&buf[sizeof(double)]);
				val.normal.z = decode_double(&buf[sizeof(double) * 2]);
				val.d = decode_double(&buf[sizeof(double) * 3]);

				if (r_len) {
					(*r_len) += sizeof(double) * 4;
				}
			} else {
				ERR_FAIL_COND_V((size_t)len < sizeof(float) * 4, ERR_INVALID_DATA);
				val.normal.x = decode_float(&buf[0]);
				val.normal.y = decode_float(&buf[sizeof(float)]);
				val.normal.z = decode_float(&buf[sizeof(float) * 2]);
				val.d = decode_float(&buf[sizeof(float) * 3]);

				if (r_len) {
					(*r_len) += sizeof(float) * 4;
				}
			}
			r_variant = val;

		} break;
		case Variant::QUATERNION: {
			Quaternion val;
			if (type & ENCODE_FLAG_64) {
				ERR_FAIL_COND_V((size_t)len < sizeof(double) * 4, ERR_INVALID_DATA);
				val.x = decode_double(&buf[0]);
				val.y = decode_double(&buf[sizeof(double)]);
				val.z = decode_double(&buf[sizeof(double) * 2]);
				val.w = decode_double(&buf[sizeof(double) * 3]);

				if (r_len) {
					(*r_len) += sizeof(double) * 4;
				}
			} else {
				ERR_FAIL_COND_V((size_t)len < sizeof(float) * 4, ERR_INVALID_DATA);
				val.x = decode_float(&buf[0]);
				val.y = decode_float(&buf[sizeof(float)]);
				val.z = decode_float(&buf[sizeof(float) * 2]);
				val.w = decode_float(&buf[sizeof(float) * 3]);

				if (r_len) {
					(*r_len) += sizeof(float) * 4;
				}
			}
			r_variant = val;

		} break;
		case Variant::AABB: {
			AABB val;
			if (type & ENCODE_FLAG_64) {
				ERR_FAIL_COND_V((size_t)len < sizeof(double) * 6, ERR_INVALID_DATA);
				val.position.x = decode_double(&buf[0]);
				val.position.y = decode_double(&buf[sizeof(double)]);
				val.position.z = decode_double(&buf[sizeof(double) * 2]);
				val.size.x = decode_double(&buf[sizeof(double) * 3]);
				val.size.y = decode_double(&buf[sizeof(double) * 4]);
				val.size.z = decode_double(&buf[sizeof(double) * 5]);

				if (r_len) {
					(*r_len) += sizeof(double) * 6;
				}
			} else {
				ERR_FAIL_COND_V((size_t)len < sizeof(float) * 6, ERR_INVALID_DATA);
				val.position.x = decode_float(&buf[0]);
				val.position.y = decode_float(&buf[sizeof(float)]);
				val.position.z = decode_float(&buf[sizeof(float) * 2]);
				val.size.x = decode_float(&buf[sizeof(float) * 3]);
				val.size.y = decode_float(&buf[sizeof(float) * 4]);
				val.size.z = decode_float(&buf[sizeof(float) * 5]);

				if (r_len) {
					(*r_len) += sizeof(float) * 6;
				}
			}
			r_variant = val;

		} break;
		case Variant::BASIS: {
			Basis val;
			if (type & ENCODE_FLAG_64) {
				ERR_FAIL_COND_V((size_t)len < sizeof(double) * 9, ERR_INVALID_DATA);
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						val.rows[i][j] = decode_double(&buf[(i * 3 + j) * sizeof(double)]);
					}
				}

				if (r_len) {
					(*r_len) += sizeof(double) * 9;
				}
			} else {
				ERR_FAIL_COND_V((size_t)len < sizeof(float) * 9, ERR_INVALID_DATA);
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						val.rows[i][j] = decode_float(&buf[(i * 3 + j) * sizeof(float)]);
					}
				}

				if (r_len) {
					(*r_len) += sizeof(float) * 9;
				}
			}
			r_variant = val;

		} break;
		case Variant::TRANSFORM3D: {
			Transform3D val;
			if (type & ENCODE_FLAG_64) {
				ERR_FAIL_COND_V((size_t)len < sizeof(double) * 12, ERR_INVALID_DATA);
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						val.basis.rows[i][j] = decode_double(&buf[(i * 3 + j) * sizeof(double)]);
					}
				}
				val.origin[0] = decode_double(&buf[sizeof(double) * 9]);
				val.origin[1] = decode_double(&buf[sizeof(double) * 10]);
				val.origin[2] = decode_double(&buf[sizeof(double) * 11]);

				if (r_len) {
					(*r_len) += sizeof(double) * 12;
				}
			} else {
				ERR_FAIL_COND_V((size_t)len < sizeof(float) * 12, ERR_INVALID_DATA);
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						val.basis.rows[i][j] = decode_float(&buf[(i * 3 + j) * sizeof(float)]);
					}
				}
				val.origin[0] = decode_float(&buf[sizeof(float) * 9]);
				val.origin[1] = decode_float(&buf[sizeof(float) * 10]);
				val.origin[2] = decode_float(&buf[sizeof(float) * 11]);

				if (r_len) {
					(*r_len) += sizeof(float) * 12;
				}
			}
			r_variant = val;

		} break;
		case Variant::PROJECTION: {
			Projection val;
			if (type & ENCODE_FLAG_64) {
				ERR_FAIL_COND_V((size_t)len < sizeof(double) * 16, ERR_INVALID_DATA);
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						val.columns[i][j] = decode_double(&buf[(i * 4 + j) * sizeof(double)]);
					}
				}
				if (r_len) {
					(*r_len) += sizeof(double) * 16;
				}
			} else {
				ERR_FAIL_COND_V((size_t)len < sizeof(float) * 16, ERR_INVALID_DATA);
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						val.columns[i][j] = decode_float(&buf[(i * 4 + j) * sizeof(float)]);
					}
				}

				if (r_len) {
					(*r_len) += sizeof(float) * 16;
				}
			}
			r_variant = val;

		} break;
		// misc types
		case Variant::COLOR: {
			ERR_FAIL_COND_V(len < 4 * 4, ERR_INVALID_DATA);
			Color val;
			val.r = decode_float(&buf[0]);
			val.g = decode_float(&buf[4]);
			val.b = decode_float(&buf[8]);
			val.a = decode_float(&buf[12]);
			r_variant = val;

			if (r_len) {
				(*r_len) += 4 * 4; // Colors should always be in single-precision.
			}
		} break;
		case Variant::STRING_NAME: {
			String str;
			Error err = _decode_string(buf, len, r_len, str);
			if (err) {
				return err;
			}
			r_variant = StringName(str);

		} break;

		case Variant::NODE_PATH: {
			ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);
			int32_t strlen = decode_uint32(buf);

			if (strlen & 0x80000000) {
				//new format
				ERR_FAIL_COND_V(len < 12, ERR_INVALID_DATA);
				Vector<StringName> names;
				Vector<StringName> subnames;

				uint32_t namecount = strlen &= 0x7FFFFFFF;
				uint32_t subnamecount = decode_uint32(buf + 4);
				uint32_t flags = decode_uint32(buf + 8);

				len -= 12;
				buf += 12;

				if (flags & 2) { // Obsolete format with property separate from subpath
					subnamecount++;
				}

				uint32_t total = namecount + subnamecount;

				if (r_len) {
					(*r_len) += 12;
				}

				for (uint32_t i = 0; i < total; i++) {
					String str;
					Error err = _decode_string(buf, len, r_len, str);
					if (err) {
						return err;
					}

					if (i < namecount) {
						names.push_back(str);
					} else {
						subnames.push_back(str);
					}
				}

				r_variant = NodePath(names, subnames, flags & 1);

			} else {
				//old format, just a string

				ERR_FAIL_V(ERR_INVALID_DATA);
			}

		} break;
		case Variant::RID: {
			ERR_FAIL_COND_V(len < 8, ERR_INVALID_DATA);
			uint64_t id = decode_uint64(buf);
			if (r_len) {
				(*r_len) += 8;
			}

			r_variant = RID::from_uint64(id);
		} break;
		case Variant::OBJECT: {
			if (type & ENCODE_FLAG_OBJECT_AS_ID) {
				//this _is_ allowed
				ERR_FAIL_COND_V(len < 8, ERR_INVALID_DATA);
				ObjectID val = ObjectID(decode_uint64(buf));
				if (r_len) {
					(*r_len) += 8;
				}

				if (val.is_null()) {
					r_variant = (Object *)nullptr;
				} else {
					Ref<EncodedObjectAsID> obj_as_id;
					obj_as_id.instantiate();
					obj_as_id->set_object_id(val);

					r_variant = obj_as_id;
				}

			} else {
				ERR_FAIL_COND_V(!p_allow_objects, ERR_UNAUTHORIZED);

				String str;
				Error err = _decode_string(buf, len, r_len, str);
				if (err) {
					return err;
				}

				if (str.is_empty()) {
					r_variant = (Object *)nullptr;
				} else {
					Object *obj = ClassDB::instantiate(str);

					ERR_FAIL_COND_V(!obj, ERR_UNAVAILABLE);
					ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);

					int32_t count = decode_uint32(buf);
					buf += 4;
					len -= 4;
					if (r_len) {
						(*r_len) += 4; // Size of count number.
					}

					for (int i = 0; i < count; i++) {
						str = String();
						err = _decode_string(buf, len, r_len, str);
						if (err) {
							return err;
						}

						Variant value;
						int used;
						err = decode_variant(value, buf, len, &used, p_allow_objects, p_depth + 1);
						if (err) {
							return err;
						}

						buf += used;
						len -= used;
						if (r_len) {
							(*r_len) += used;
						}

						obj->set(str, value);
					}

					if (Object::cast_to<RefCounted>(obj)) {
						Ref<RefCounted> ref = Ref<RefCounted>(Object::cast_to<RefCounted>(obj));
						r_variant = ref;
					} else {
						r_variant = obj;
					}
				}
			}

		} break;
		case Variant::CALLABLE: {
			r_variant = Callable();
		} break;
		case Variant::SIGNAL: {
			String name;
			Error err = _decode_string(buf, len, r_len, name);
			if (err) {
				return err;
			}

			ERR_FAIL_COND_V(len < 8, ERR_INVALID_DATA);
			ObjectID id = ObjectID(decode_uint64(buf));
			if (r_len) {
				(*r_len) += 8;
			}

			r_variant = Signal(id, StringName(name));
		} break;
		case Variant::DICTIONARY: {
			ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);
			int32_t count = decode_uint32(buf);
			//  bool shared = count&0x80000000;
			count &= 0x7FFFFFFF;

			buf += 4;
			len -= 4;

			if (r_len) {
				(*r_len) += 4; // Size of count number.
			}

			Dictionary d;

			for (int i = 0; i < count; i++) {
				Variant key, value;

				int used;
				Error err = decode_variant(key, buf, len, &used, p_allow_objects, p_depth + 1);
				ERR_FAIL_COND_V_MSG(err != OK, err, "Error when trying to decode Variant.");

				buf += used;
				len -= used;
				if (r_len) {
					(*r_len) += used;
				}

				err = decode_variant(value, buf, len, &used, p_allow_objects, p_depth + 1);
				ERR_FAIL_COND_V_MSG(err != OK, err, "Error when trying to decode Variant.");

				buf += used;
				len -= used;
				if (r_len) {
					(*r_len) += used;
				}

				d[key] = value;
			}

			r_variant = d;

		} break;
		case Variant::ARRAY: {
			ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);
			int32_t count = decode_uint32(buf);
			//  bool shared = count&0x80000000;
			count &= 0x7FFFFFFF;

			buf += 4;
			len -= 4;

			if (r_len) {
				(*r_len) += 4; // Size of count number.
			}

			Array varr;

			for (int i = 0; i < count; i++) {
				int used = 0;
				Variant v;
				Error err = decode_variant(v, buf, len, &used, p_allow_objects, p_depth + 1);
				ERR_FAIL_COND_V_MSG(err != OK, err, "Error when trying to decode Variant.");
				buf += used;
				len -= used;
				varr.push_back(v);
				if (r_len) {
					(*r_len) += used;
				}
			}

			r_variant = varr;

		} break;

		// arrays
		case Variant::PACKED_BYTE_ARRAY: {
			ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);
			int32_t count = decode_uint32(buf);
			buf += 4;
			len -= 4;
			ERR_FAIL_COND_V(count < 0 || count > len, ERR_INVALID_DATA);

			Vector<uint8_t> data;

			if (count) {
				data.resize(count);
				uint8_t *w = data.ptrw();
				for (int32_t i = 0; i < count; i++) {
					w[i] = buf[i];
				}
			}

			r_variant = data;

			if (r_len) {
				if (count % 4) {
					(*r_len) += 4 - count % 4;
				}
				(*r_len) += 4 + count;
			}

		} break;
		case Variant::PACKED_INT32_ARRAY: {
			ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);
			int32_t count = decode_uint32(buf);
			buf += 4;
			len -= 4;
			ERR_FAIL_MUL_OF(count, 4, ERR_INVALID_DATA);
			ERR_FAIL_COND_V(count < 0 || count * 4 > len, ERR_INVALID_DATA);

			Vector<int32_t> data;

			if (count) {
				//const int*rbuf=(const int*)buf;
				data.resize(count);
				int32_t *w = data.ptrw();
				for (int32_t i = 0; i < count; i++) {
					w[i] = decode_uint32(&buf[i * 4]);
				}
			}
			r_variant = Variant(data);
			if (r_len) {
				(*r_len) += 4 + count * sizeof(int32_t);
			}

		} break;
		case Variant::PACKED_INT64_ARRAY: {
			ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);
			int32_t count = decode_uint32(buf);
			buf += 4;
			len -= 4;
			ERR_FAIL_MUL_OF(count, 8, ERR_INVALID_DATA);
			ERR_FAIL_COND_V(count < 0 || count * 8 > len, ERR_INVALID_DATA);

			Vector<int64_t> data;

			if (count) {
				//const int*rbuf=(const int*)buf;
				data.resize(count);
				int64_t *w = data.ptrw();
				for (int64_t i = 0; i < count; i++) {
					w[i] = decode_uint64(&buf[i * 8]);
				}
			}
			r_variant = Variant(data);
			if (r_len) {
				(*r_len) += 4 + count * sizeof(int64_t);
			}

		} break;
		case Variant::PACKED_FLOAT32_ARRAY: {
			ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);
			int32_t count = decode_uint32(buf);
			buf += 4;
			len -= 4;
			ERR_FAIL_MUL_OF(count, 4, ERR_INVALID_DATA);
			ERR_FAIL_COND_V(count < 0 || count * 4 > len, ERR_INVALID_DATA);

			Vector<float> data;

			if (count) {
				//const float*rbuf=(const float*)buf;
				data.resize(count);
				float *w = data.ptrw();
				for (int32_t i = 0; i < count; i++) {
					w[i] = decode_float(&buf[i * 4]);
				}
			}
			r_variant = data;

			if (r_len) {
				(*r_len) += 4 + count * sizeof(float);
			}

		} break;
		case Variant::PACKED_FLOAT64_ARRAY: {
			ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);
			int32_t count = decode_uint32(buf);
			buf += 4;
			len -= 4;
			ERR_FAIL_MUL_OF(count, 8, ERR_INVALID_DATA);
			ERR_FAIL_COND_V(count < 0 || count * 8 > len, ERR_INVALID_DATA);

			Vector<double> data;

			if (count) {
				data.resize(count);
				double *w = data.ptrw();
				for (int64_t i = 0; i < count; i++) {
					w[i] = decode_double(&buf[i * 8]);
				}
			}
			r_variant = data;

			if (r_len) {
				(*r_len) += 4 + count * sizeof(double);
			}

		} break;
		case Variant::PACKED_STRING_ARRAY: {
			ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);
			int32_t count = decode_uint32(buf);

			Vector<String> strings;
			buf += 4;
			len -= 4;

			if (r_len) {
				(*r_len) += 4; // Size of count number.
			}

			for (int32_t i = 0; i < count; i++) {
				String str;
				Error err = _decode_string(buf, len, r_len, str);
				if (err) {
					return err;
				}

				strings.push_back(str);
			}

			r_variant = strings;

		} break;
		case Variant::PACKED_VECTOR2_ARRAY: {
			ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);
			int32_t count = decode_uint32(buf);
			buf += 4;
			len -= 4;

			Vector<Vector2> varray;

			if (type & ENCODE_FLAG_64) {
				ERR_FAIL_MUL_OF(count, sizeof(double) * 2, ERR_INVALID_DATA);
				ERR_FAIL_COND_V(count < 0 || count * sizeof(double) * 2 > (size_t)len, ERR_INVALID_DATA);

				if (r_len) {
					(*r_len) += 4; // Size of count number.
				}

				if (count) {
					varray.resize(count);
					Vector2 *w = varray.ptrw();

					for (int32_t i = 0; i < count; i++) {
						w[i].x = decode_double(buf + i * sizeof(double) * 2 + sizeof(double) * 0);
						w[i].y = decode_double(buf + i * sizeof(double) * 2 + sizeof(double) * 1);
					}

					int adv = sizeof(double) * 2 * count;

					if (r_len) {
						(*r_len) += adv;
					}
					len -= adv;
					buf += adv;
				}
			} else {
				ERR_FAIL_MUL_OF(count, sizeof(float) * 2, ERR_INVALID_DATA);
				ERR_FAIL_COND_V(count < 0 || count * sizeof(float) * 2 > (size_t)len, ERR_INVALID_DATA);

				if (r_len) {
					(*r_len) += 4; // Size of count number.
				}

				if (count) {
					varray.resize(count);
					Vector2 *w = varray.ptrw();

					for (int32_t i = 0; i < count; i++) {
						w[i].x = decode_float(buf + i * sizeof(float) * 2 + sizeof(float) * 0);
						w[i].y = decode_float(buf + i * sizeof(float) * 2 + sizeof(float) * 1);
					}

					int adv = sizeof(float) * 2 * count;

					if (r_len) {
						(*r_len) += adv;
					}
				}
			}
			r_variant = varray;

		} break;
		case Variant::PACKED_VECTOR3_ARRAY: {
			ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);
			int32_t count = decode_uint32(buf);
			buf += 4;
			len -= 4;

			Vector<Vector3> varray;

			if (type & ENCODE_FLAG_64) {
				ERR_FAIL_MUL_OF(count, sizeof(double) * 3, ERR_INVALID_DATA);
				ERR_FAIL_COND_V(count < 0 || count * sizeof(double) * 3 > (size_t)len, ERR_INVALID_DATA);

				if (r_len) {
					(*r_len) += 4; // Size of count number.
				}

				if (count) {
					varray.resize(count);
					Vector3 *w = varray.ptrw();

					for (int32_t i = 0; i < count; i++) {
						w[i].x = decode_double(buf + i * sizeof(double) * 3 + sizeof(double) * 0);
						w[i].y = decode_double(buf + i * sizeof(double) * 3 + sizeof(double) * 1);
						w[i].z = decode_double(buf + i * sizeof(double) * 3 + sizeof(double) * 2);
					}

					int adv = sizeof(double) * 3 * count;

					if (r_len) {
						(*r_len) += adv;
					}
					len -= adv;
					buf += adv;
				}
			} else {
				ERR_FAIL_MUL_OF(count, sizeof(float) * 3, ERR_INVALID_DATA);
				ERR_FAIL_COND_V(count < 0 || count * sizeof(float) * 3 > (size_t)len, ERR_INVALID_DATA);

				if (r_len) {
					(*r_len) += 4; // Size of count number.
				}

				if (count) {
					varray.resize(count);
					Vector3 *w = varray.ptrw();

					for (int32_t i = 0; i < count; i++) {
						w[i].x = decode_float(buf + i * sizeof(float) * 3 + sizeof(float) * 0);
						w[i].y = decode_float(buf + i * sizeof(float) * 3 + sizeof(float) * 1);
						w[i].z = decode_float(buf + i * sizeof(float) * 3 + sizeof(float) * 2);
					}

					int adv = sizeof(float) * 3 * count;

					if (r_len) {
						(*r_len) += adv;
					}
					len -= adv;
					buf += adv;
				}
			}
			r_variant = varray;

		} break;
		case Variant::PACKED_COLOR_ARRAY: {
			ERR_FAIL_COND_V(len < 4, ERR_INVALID_DATA);
			int32_t count = decode_uint32(buf);
			buf += 4;
			len -= 4;

			ERR_FAIL_MUL_OF(count, 4 * 4, ERR_INVALID_DATA);
			ERR_FAIL_COND_V(count < 0 || count * 4 * 4 > len, ERR_INVALID_DATA);

			Vector<Color> carray;

			if (r_len) {
				(*r_len) += 4; // Size of count number.
			}

			if (count) {
				carray.resize(count);
				Color *w = carray.ptrw();

				for (int32_t i = 0; i < count; i++) {
					// Colors should always be in single-precision.
					w[i].r = decode_float(buf + i * 4 * 4 + 4 * 0);
					w[i].g = decode_float(buf + i * 4 * 4 + 4 * 1);
					w[i].b = decode_float(buf + i * 4 * 4 + 4 * 2);
					w[i].a = decode_float(buf + i * 4 * 4 + 4 * 3);
				}

				int adv = 4 * 4 * count;

				if (r_len) {
					(*r_len) += adv;
				}
			}

			r_variant = carray;

		} break;
		default: {
			ERR_FAIL_V(ERR_BUG);
		}
	}

	return OK;
}

uint8_t StructBuffer::get_u8() {
	uint8_t buf[1];
	get_data(buf, 1);
	return buf[0];
}

int8_t StructBuffer::get_8() {
	uint8_t buf[1];
	get_data(buf, 1);
	return buf[0];
}

uint16_t StructBuffer::get_u16() {
	uint8_t buf[2];
	get_data(buf, 2);
	uint16_t r = decode_uint16(buf);
	if (big_endian) {
		r = BSWAP16(r);
	}
	return r;
}

int16_t StructBuffer::get_16() {
	uint8_t buf[2];
	get_data(buf, 2);
	uint16_t r = decode_uint16(buf);
	if (big_endian) {
		r = BSWAP16(r);
	}
	return r;
}

uint32_t StructBuffer::get_u32() {
	uint8_t buf[4];
	get_data(buf, 4);
	uint32_t r = decode_uint32(buf);
	if (big_endian) {
		r = BSWAP32(r);
	}
	return r;
}

int32_t StructBuffer::get_32() {
	uint8_t buf[4];
	get_data(buf, 4);
	uint32_t r = decode_uint32(buf);
	if (big_endian) {
		r = BSWAP32(r);
	}
	return r;
}

uint64_t StructBuffer::get_u64() {
	uint8_t buf[8];
	get_data(buf, 8);
	uint64_t r = decode_uint64(buf);
	if (big_endian) {
		r = BSWAP64(r);
	}
	return r;
}

int64_t StructBuffer::get_64() {
	uint8_t buf[8];
	get_data(buf, 8);
	uint64_t r = decode_uint64(buf);
	if (big_endian) {
		r = BSWAP64(r);
	}
	return r;
}

float StructBuffer::get_float() {
	uint8_t buf[4];
	get_data(buf, 4);

	if (big_endian) {
		uint32_t *p32 = (uint32_t *)buf;
		*p32 = BSWAP32(*p32);
	}

	return decode_float(buf);
}

double StructBuffer::get_double() {
	uint8_t buf[8];
	get_data(buf, 8);

	if (big_endian) {
		uint64_t *p64 = (uint64_t *)buf;
		*p64 = BSWAP64(*p64);
	}

	return decode_double(buf);
}

String StructBuffer::get_string() {
	String *s = get_ptr<String>();
	return s ? *s : String();
}

template <class T>
T *StructBuffer::get_ptr() {
	if (sizeof(T *) == 4) {
		T *v = reinterpret_cast<T *>(get_u32());
		return v;
	} else if (sizeof(T *) == 8) {
		T *v = reinterpret_cast<T *>(get_u64());
		return v;
	}
	ERR_FAIL_V_MSG(nullptr, "Indeterminate pointer extracted from struct.");
}

Variant StructBuffer::get_var(Variant::Type p_type, bool p_allow_objects) {
	int len = get_32();
	Vector<uint8_t> var;
	Error err = var.resize(len);
	ERR_FAIL_COND_V(err != OK, Variant());
	err = get_data(var.ptrw(), len);
	ERR_FAIL_COND_V(err != OK, Variant());

	Variant ret;
	err = decode_variant(ret, var.ptr(), len, nullptr, p_allow_objects);
	ERR_FAIL_COND_V_MSG(err != OK, Variant(), "Error when trying to decode Variant.");

	return ret;
}

Error StructBuffer::put_value(const Variant& p_variant) {
	uint8_t *buf = _get_data();

	bool use64 = false;

	switch (p_variant.get_type()) {
		case Variant::INT: {
			int64_t val = p_variant;
			if (val > (int64_t)INT_MAX || val < (int64_t)INT_MIN) {
				use64 = true;
			}
		} break;
		case Variant::FLOAT: {
			double d = p_variant;
			float f = d;
			if (double(f) != d) {
				use64 = true;
			}
		} break;
		case Variant::OBJECT: {
			// Test for potential wrong values sent by the debugger when it breaks.
			Object *obj = p_variant.get_validated_object();
			if (!obj) {
				// Object is invalid, send a nullptr instead.
				if (buf) {
					encode_uint32(Variant::NIL, buf);
				}
				pointer += 4;
				return OK;
			}
		} break;
#ifdef REAL_T_IS_DOUBLE
		case Variant::VECTOR2:
		case Variant::VECTOR3:
		case Variant::PACKED_VECTOR2_ARRAY:
		case Variant::PACKED_VECTOR3_ARRAY:
		case Variant::TRANSFORM2D:
		case Variant::TRANSFORM3D:
		case Variant::QUATERNION:
		case Variant::PLANE:
		case Variant::BASIS:
		case Variant::RECT2:
		case Variant::AABB: {
			use64 = true;
		} break;
#endif // REAL_T_IS_DOUBLE
		default: {
		} // nothing to do at this stage
	}

	switch (p_variant.get_type()) {
		case Variant::NIL: {
			//nothing to do
		} break;
		case Variant::BOOL: {
			if (buf) {
				encode_uint32(p_variant.operator bool(), buf);
			}

			pointer += 4;

		} break;
		case Variant::INT: {
			if (use64) {
				//64 bits
				if (buf) {
					encode_uint64(p_variant.operator int64_t(), buf);
				}

				pointer += 8;
			} else {
				if (buf) {
					encode_uint32(p_variant.operator int32_t(), buf);
				}

				pointer += 4;
			}
		} break;
		case Variant::FLOAT: {
			if (use64) {
				if (buf) {
					encode_double(p_variant.operator double(), buf);
				}

				pointer += 8;

			} else {
				if (buf) {
					encode_float(p_variant.operator float(), buf);
				}

				pointer += 4;
			}

		} break;
		case Variant::NODE_PATH: {
			// shallow copy
			_put_ptr<NodePath>(&p_variant.operator NodePath());
		} break;
		case Variant::STRING:
		case Variant::STRING_NAME: {
			// shallow copy
			_put_ptr<String>(&p_variant.operator String());
		} break;

		// math types
		case Variant::VECTOR2: {
			if (buf) {
				Vector2 v2 = p_variant;
				encode_real(v2.x, &buf[0]);
				encode_real(v2.y, &buf[sizeof(real_t)]);
			}

			pointer += 2 * sizeof(real_t);

		} break;
		case Variant::VECTOR2I: {
			if (buf) {
				Vector2i v2 = p_variant;
				encode_uint32(v2.x, &buf[0]);
				encode_uint32(v2.y, &buf[4]);
			}

			pointer += 2 * 4;

		} break;
		case Variant::RECT2: {
			if (buf) {
				Rect2 r2 = p_variant;
				encode_real(r2.position.x, &buf[0]);
				encode_real(r2.position.y, &buf[sizeof(real_t)]);
				encode_real(r2.size.x, &buf[sizeof(real_t) * 2]);
				encode_real(r2.size.y, &buf[sizeof(real_t) * 3]);
			}
			pointer += 4 * sizeof(real_t);

		} break;
		case Variant::RECT2I: {
			if (buf) {
				Rect2i r2 = p_variant;
				encode_uint32(r2.position.x, &buf[0]);
				encode_uint32(r2.position.y, &buf[4]);
				encode_uint32(r2.size.x, &buf[8]);
				encode_uint32(r2.size.y, &buf[12]);
			}
			pointer += 4 * 4;

		} break;
		case Variant::VECTOR3: {
			if (buf) {
				Vector3 v3 = p_variant;
				encode_real(v3.x, &buf[0]);
				encode_real(v3.y, &buf[sizeof(real_t)]);
				encode_real(v3.z, &buf[sizeof(real_t) * 2]);
			}

			pointer += 3 * sizeof(real_t);

		} break;
		case Variant::VECTOR3I: {
			if (buf) {
				Vector3i v3 = p_variant;
				encode_uint32(v3.x, &buf[0]);
				encode_uint32(v3.y, &buf[4]);
				encode_uint32(v3.z, &buf[8]);
			}

			pointer += 3 * 4;

		} break;
		case Variant::TRANSFORM2D: {
			if (buf) {
				Transform2D val = p_variant;
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 2; j++) {
						memcpy(&buf[(i * 2 + j) * sizeof(real_t)], &val.columns[i][j], sizeof(real_t));
					}
				}
			}

			pointer += 6 * sizeof(real_t);

		} break;
		case Variant::VECTOR4: {
			if (buf) {
				Vector4 v4 = p_variant;
				encode_real(v4.x, &buf[0]);
				encode_real(v4.y, &buf[sizeof(real_t)]);
				encode_real(v4.z, &buf[sizeof(real_t) * 2]);
				encode_real(v4.w, &buf[sizeof(real_t) * 3]);
			}

			pointer += 4 * sizeof(real_t);

		} break;
		case Variant::VECTOR4I: {
			if (buf) {
				Vector4i v4 = p_variant;
				encode_uint32(v4.x, &buf[0]);
				encode_uint32(v4.y, &buf[4]);
				encode_uint32(v4.z, &buf[8]);
				encode_uint32(v4.w, &buf[12]);
			}

			pointer += 4 * 4;

		} break;
		case Variant::PLANE: {
			if (buf) {
				Plane p = p_variant;
				encode_real(p.normal.x, &buf[0]);
				encode_real(p.normal.y, &buf[sizeof(real_t)]);
				encode_real(p.normal.z, &buf[sizeof(real_t) * 2]);
				encode_real(p.d, &buf[sizeof(real_t) * 3]);
			}

			pointer += 4 * sizeof(real_t);

		} break;
		case Variant::QUATERNION: {
			if (buf) {
				Quaternion q = p_variant;
				encode_real(q.x, &buf[0]);
				encode_real(q.y, &buf[sizeof(real_t)]);
				encode_real(q.z, &buf[sizeof(real_t) * 2]);
				encode_real(q.w, &buf[sizeof(real_t) * 3]);
			}

			pointer += 4 * sizeof(real_t);

		} break;
		case Variant::AABB: {
			if (buf) {
				AABB aabb = p_variant;
				encode_real(aabb.position.x, &buf[0]);
				encode_real(aabb.position.y, &buf[sizeof(real_t)]);
				encode_real(aabb.position.z, &buf[sizeof(real_t) * 2]);
				encode_real(aabb.size.x, &buf[sizeof(real_t) * 3]);
				encode_real(aabb.size.y, &buf[sizeof(real_t) * 4]);
				encode_real(aabb.size.z, &buf[sizeof(real_t) * 5]);
			}

			pointer += 6 * sizeof(real_t);

		} break;
		case Variant::BASIS: {
			if (buf) {
				Basis val = p_variant;
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						memcpy(&buf[(i * 3 + j) * sizeof(real_t)], &val.rows[i][j], sizeof(real_t));
					}
				}
			}

			pointer += 9 * sizeof(real_t);

		} break;
		case Variant::TRANSFORM3D: {
			if (buf) {
				Transform3D val = p_variant;
				for (int i = 0; i < 3; i++) {
					for (int j = 0; j < 3; j++) {
						memcpy(&buf[(i * 3 + j) * sizeof(real_t)], &val.basis.rows[i][j], sizeof(real_t));
					}
				}

				encode_real(val.origin.x, &buf[sizeof(real_t) * 9]);
				encode_real(val.origin.y, &buf[sizeof(real_t) * 10]);
				encode_real(val.origin.z, &buf[sizeof(real_t) * 11]);
			}

			pointer += 12 * sizeof(real_t);

		} break;
		case Variant::PROJECTION: {
			if (buf) {
				Projection val = p_variant;
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						memcpy(&buf[(i * 4 + j) * sizeof(real_t)], &val.columns[i][j], sizeof(real_t));
					}
				}
			}

			pointer += 16 * sizeof(real_t);

		} break;

		// misc types
		case Variant::COLOR: {
			if (buf) {
				Color c = p_variant;
				encode_float(c.r, &buf[0]);
				encode_float(c.g, &buf[4]);
				encode_float(c.b, &buf[8]);
				encode_float(c.a, &buf[12]);
			}

			pointer += 4 * 4; // Colors should always be in single-precision.

		} break;
		case Variant::RID: {
			RID rid = p_variant;

			if (buf) {
				encode_uint64(rid.get_id(), buf);
			}
			pointer += 8;
		} break;
		case Variant::OBJECT: {
			if (buf) {
				Object *obj = p_variant.get_validated_object();
				ObjectID id;
				if (obj) {
					id = obj->get_instance_id();
				}

				encode_uint64(id, buf);
			}

			pointer += 8;
		} break;
		case Variant::CALLABLE: {
			// shallow copy
			_put_ptr<Callable>(&p_variant.operator Callable());
		} break;
		case Variant::SIGNAL: {
			// shallow copy
			_put_ptr<Signal>(&p_variant.operator Signal());
		} break;
		case Variant::DICTIONARY: {
			// shallow copy
			_put_ptr<Dictionary>(&p_variant.operator Dictionary());
		} break;
		case Variant::ARRAY: {
			// shallow copy
			_put_ptr<Array>(&p_variant.operator Array());
		} break;
		// arrays
		case Variant::PACKED_BYTE_ARRAY: {
			// shallow copy
			_put_ptr<PackedByteArray>(&p_variant.operator PackedByteArray());
		} break;
		case Variant::PACKED_INT32_ARRAY: {
			// shallow copy
			_put_ptr<PackedInt32Array>(&p_variant.operator PackedInt32Array());
		} break;
		case Variant::PACKED_INT64_ARRAY: {
			// shallow copy
			_put_ptr<PackedInt64Array>(&p_variant.operator PackedInt64Array());
		} break;
		case Variant::PACKED_FLOAT32_ARRAY: {
			// shallow copy
			_put_ptr<PackedFloat32Array>(&p_variant.operator PackedFloat32Array());
		} break;
		case Variant::PACKED_FLOAT64_ARRAY: {
			// shallow copy
			_put_ptr<PackedFloat64Array>(&p_variant.operator PackedFloat64Array());
		} break;
		case Variant::PACKED_STRING_ARRAY: {
			// shallow copy
			_put_ptr<PackedStringArray>(&p_variant.operator PackedStringArray());
		} break;
		case Variant::PACKED_VECTOR2_ARRAY: {
			// shallow copy
			_put_ptr<PackedVector2Array>(&p_variant.operator PackedVector2Array());
		} break;
		case Variant::PACKED_VECTOR3_ARRAY: {
			// shallow copy
			_put_ptr<PackedVector3Array>(&p_variant.operator PackedVector3Array());
		} break;
		case Variant::PACKED_COLOR_ARRAY: {
			// shallow copy
			_put_ptr<PackedColorArray>(&p_variant.operator PackedColorArray());
		} break;
		default: {
			ERR_FAIL_V(ERR_BUG);
		}
	}

	return OK;
}

Error StructBuffer::put_data(const uint8_t *p_data, int p_bytes) {
	if (p_bytes <= 0) {
		return OK;
	}

	ERR_FAIL_COND_V(pointer + p_bytes > get_size(), ERR_OUT_OF_MEMORY);

	uint8_t *w = _get_data();
	memcpy(&w[pointer], p_data, p_bytes);

	pointer += p_bytes;
	return OK;
}

Error StructBuffer::get_data(uint8_t *p_buffer, int p_bytes) {
	int recv;
	get_partial_data(p_buffer, p_bytes, recv);
	if (recv != p_bytes) {
		return ERR_INVALID_PARAMETER;
	}

	return OK;
}

Error StructBuffer::get_partial_data(uint8_t *p_buffer, int p_bytes, int &r_received) {
	int size = get_size();
	if (pointer + p_bytes > size) {
		r_received = size - pointer;
		if (r_received <= 0) {
			r_received = 0;
			return OK; //you got 0
		}
	} else {
		r_received = p_bytes;
	}

	const uint8_t *r = _get_data();
	memcpy(p_buffer, r + pointer, r_received);

	pointer += r_received;
	return OK;
}

int StructBuffer::get_available_bytes() const {
	return get_size() - pointer;
}

void StructBuffer::seek(int p_pos) {
	ERR_FAIL_COND(p_pos < 0);
	ERR_FAIL_COND(p_pos > get_size());
	pointer = p_pos;
}

int StructBuffer::get_size() const {
	return _struct->get_capacity();
}

int StructBuffer::get_position() const {
	return pointer;
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

