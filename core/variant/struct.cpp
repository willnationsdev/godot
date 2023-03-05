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

#include "core/variant/struct.h"
#include "core/variant/struct_db.h"
#include "core/io/resource_loader.h"

bool Struct::operator==(const Struct& p_struct) const {
	if (!(p_struct.preamble == preamble)) {
		return false;
	}
	uint8_t size;
}

void Struct::assign(const Struct* p_other, Error& r_error) {
	StructDB::struct_assign(this, p_other, r_error);
}

template <class T>
T Struct::get_value(int p_offset, bool *r_valid) const {
	const uint8_t *ptr = StructDB::get_data_const(*this);
	const StructDB::StructTypeInfo *t = StructDB::get_struct_type(get_type_id());
	if (!t) {
		if (r_valid) {
			*r_valid = false;
		}
		return T();
	}

	int cap = t->get_capacity();
	const uint8_t *end = ptr + cap;
	int i = 0;

	for (; i < max_property_count && ptr < end && i < p_offset; i++) {
		ptr += t->property_id_map[preamble.property_ids[i]].bytes;
	}
	if (i == p_offset) {
		if (r_valid) {
			*r_valid = true;
		}
		// TODO: This isn't enough. Needs to respect the property's "bytes" configuration, not the amount of memory associated with T.
		// How to do that? I need to have a stack-allocated instance of T and then use memcpy with configured bytes amount? Possibly...
		return reinterpret_cast<T>(&ptr);
	}
	if (r_valid) {
		*r_valid = false;
	}
	return T();
}

int Struct::get_capacity(StructBucket p_bucket) {
	switch (p_bucket) {
		case STRUCT_MINIMAL:
			return Struct::bucket_minimal;
		case STRUCT_SMALL:
			return Struct::bucket_small;
		case STRUCT_MEDIUM:
			return Struct::bucket_medium;
		case STRUCT_LARGE:
			return Struct::bucket_large;
		default:
			return 0;
	}
}

int Struct::get_capacity() const {
	int a = get_value<int>(0);
	const StructDB::StructTypeInfo *t = StructDB::get_struct_type(get_type_id());
	return t ? t->get_capacity() : 0;
}

uint8_t *Struct::get_data() {
	return StructDB::get_data(*this);
}

const uint8_t *Struct::get_data_const() const {
	return StructDB::get_data_const(*this);
}

StringName Struct::get_type_name() const {
	const StructDB::StructTypeInfo *struct_type = StructDB::get_struct_type(preamble.type_id);
	ERR_FAIL_COND_V(!struct_type, StringName());
	return struct_type->name;
}

void Struct::_get_property_list(List<StructPropertyInfo *> *r_list, bool p_reversed) const {
	ERR_FAIL_COND(!r_list);
	StructTypeId id = get_type_id();
	const StructDB::StructTypeInfo *t = StructDB::get_struct_type(id);
	ERR_FAIL_COND_MSG(!t, vformat("Unknown type with id '%d'.)", id));
	for (int i = 0; i < Struct::max_property_count; i++) {
		StructPropertyId id = preamble.property_ids[i++];
		if (!id) {
			continue;
		}
		const StructPropertyInfo *p = t->property_id_map.getptr(id);
		if (p) {
			StructPropertyInfo spi(*p);
			r_list->push_back(&spi);
		}
	}
}

void Struct::get_property_list(List<PropertyInfo> *r_list, bool p_reversed) const {
	ERR_FAIL_COND(!r_list);
	List<StructPropertyInfo *> list;
	_get_property_list(&list);
	for (const StructPropertyInfo *p : list) {
		r_list->push_back(*p);
	}
}

void Struct::get_property_list(List<StructPropertyInfo> *r_list, bool p_reversed) const {
	ERR_FAIL_COND(!r_list);
	List<StructPropertyInfo *> list;
	_get_property_list(&list);
	for (const StructPropertyInfo *p : list) {
		r_list->push_back(*p);
	}
}

bool Struct::has_method(const StringName &p_name) const {
	return StructDB::struct_has_method(this, p_name);
}

Variant Struct::callp(const StringName &p_method, const Variant **p_args, int p_argcount, Callable::CallError &r_error) {
	r_error.error = Callable::CallError::CALL_OK;
	const StructDB::StructTypeInfo *struct_type = StructDB::get_struct_type(preamble.type_id);
	ERR_FAIL_COND_V_MSG(!struct_type, Variant(), vformat("Unknown struct_type for struct during method call '%s'.", p_method));

	Variant ret;

	Ref<Script> script = struct_type->script;

	if (script.is_valid()) {
		ret = script->callp(p_method, p_args, p_argcount, r_error);
		//force jumptable
		switch (r_error.error) {
			case Callable::CallError::CALL_OK:
				return ret;
			case Callable::CallError::CALL_ERROR_INVALID_METHOD:
				break;
			case Callable::CallError::CALL_ERROR_INVALID_ARGUMENT:
			case Callable::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS:
			case Callable::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS:
			case Callable::CallError::CALL_ERROR_METHOD_NOT_CONST:
				return ret;
			case Callable::CallError::CALL_ERROR_INSTANCE_IS_NULL: {
			}
		}
	} else {

	}

	//extension does not need this, because all methods are registered in MethodBind

	// TODO: Surely this must eventually become a way of accessing the PropertySetGet in StructDB, no?

	MethodBind *const *method = struct_type->method_map.getptr(p_method);

	if (method) {
		// There is a compiler error here b/c Struct is not an Object.
		// Perhaps we need to have a StructApi singleton extending Object
		// that implements the logic for basic struct operations, that way
		// members of structs can be accessed dynamically in the Godot API?

		//ret = (*method)->call(this, p_args, p_argcount, r_error);
	} else {
		r_error.error = Callable::CallError::CALL_ERROR_INVALID_METHOD;
	}

	return ret;
}

