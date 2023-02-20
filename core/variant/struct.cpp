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

int StructTypeInfo::get_length() const {
	int len = 0;
	for (KeyValue<struct_property_t, StructPropertyInfo> pair : properties) {
		len += pair.value.bytes;
	}
	return len;
}

int StructTypeInfo::get_data_length() const {
	return sizeof(StructPreamble) + get_length();
}

int StructTypeInfo::get_capacity() const {
	switch (bucket) {
		case STRUCT_MINIMAL:
			return Struct::bucket_minimal;
		case STRUCT_SMALL:
			return Struct::bucket_small;
		case STRUCT_MEDIUM:
			return Struct::bucket_medium;
		case STRUCT_LARGE:
			return Struct::bucket_large;
		default:
			ERR_FAIL_V_MSG(0, vformat("Struct type '%s' has invalid bucket size '%d'.", name, bucket));
	}
}

Variant StructTypeInfo::get_script() const {
	if (script_class == StringName()) {
		return Variant();
	}
	String path = ScriptServer::get_global_class_path(script_class);
	if (path.is_empty()) {
		return Variant();
	}
	return ResourceLoader::load(path, SNAME("Script"));
}

void StructTypeInfo::assign(Struct *p_self, const Struct *p_value) {
	//Callable *assign = methods.getptr(SNAME("_op_assign"));
	//if (assign) {
	//	assign->callp(p_self, )
	//}
}

bool Struct::operator==(const Struct& p_struct) const {
	if (!(p_struct.preamble == preamble)) {
		return false;
	}
	uint8_t size;
}

uint8_t *Struct::get_data() {
	return StructDB::get_data(*this);
}

const uint8_t *Struct::get_data_const() const {
	return StructDB::get_data_const(*this);
}

const StructTypeInfo *Struct::get_type() const {
	return StructDB::_types.getptr(preamble.type_id);
}
