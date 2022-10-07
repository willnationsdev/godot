/*************************************************************************/
/*  core_type_provider.cpp                                               */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
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

#include "classdb_type_provider.h"

const core_bind::special::ClassDB *_db = nullptr;

PackedStringArray ClassDBTypeProvider::get_type_list(Dictionary p_query_state, bool p_no_named, bool p_no_anonymous) const {
	if (p_no_named) {
		return _get_result(p_query_state);
	}
	return _aggregate(p_query_state, get_db()->get_class_list());
}

PackedStringArray ClassDBTypeProvider::get_inheriters_from_type(Dictionary p_query_state, const Variant &p_type) const {
	return _aggregate(p_query_state, get_db()->get_inheriters_from_class(p_type));
}

StringName ClassDBTypeProvider::get_parent_type(Dictionary p_query_state, const Variant &p_type) const {
	return _overwrite(p_query_state, get_db()->get_parent_class(p_type));
}

bool ClassDBTypeProvider::type_exists(Dictionary p_query_state, const Variant &p_type) const {
	return _aggregate(p_query_state, get_db()->class_exists(p_type));
}

bool ClassDBTypeProvider::is_parent_type(Dictionary p_query_state, const Variant &p_type, const Variant &p_inherits) const {
	return _aggregate(p_query_state, get_db()->is_parent_class(p_type, p_inherits));
}

bool ClassDBTypeProvider::can_instantiate(Dictionary p_query_state, const Variant &p_type) const {
	return _aggregate(p_query_state, get_db()->can_instantiate(p_type));
}

Variant ClassDBTypeProvider::instantiate(Dictionary p_query_state, const Variant &p_type) const {
	return _overwrite(p_query_state, get_db()->instantiate(p_type));
}

bool ClassDBTypeProvider::has_signal(Dictionary p_query_state, const Variant &p_type, StringName p_signal) const {
	return _aggregate(p_query_state, get_db()->has_signal(p_type, p_signal));
}

Dictionary ClassDBTypeProvider::get_signal(Dictionary p_query_state, const Variant &p_type, StringName p_signal) const {
	return _overwrite(p_query_state, get_db()->get_signal(p_type, p_signal));
}

TypedArray<Dictionary> ClassDBTypeProvider::get_type_signal_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance) const {
	return _aggregate(p_query_state, get_db()->get_signal_list(p_type, p_no_inheritance));
}

TypedArray<Dictionary> ClassDBTypeProvider::get_type_property_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance) const {
	return _aggregate(p_query_state, get_db()->get_property_list(p_type, p_no_inheritance));
}

Variant ClassDBTypeProvider::get_property(Dictionary p_query_state, const Variant &p_source, const StringName &p_property) const {
	return _overwrite(p_query_state, get_db()->get_property(p_source, p_property));
}

Error ClassDBTypeProvider::set_property(Dictionary p_query_state, const Variant &p_source, const StringName &p_property, const Variant &p_value) const {
	return _overwrite(p_query_state, get_db()->set_property(p_source, p_property, p_value));
}

bool ClassDBTypeProvider::has_method(Dictionary p_query_state, const Variant &p_type, StringName p_method, bool p_no_inheritance) const {
	return _aggregate(p_query_state, get_db()->has_method(p_type, p_method, p_no_inheritance));
}

TypedArray<Dictionary> ClassDBTypeProvider::get_type_method_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance) const {
	return _aggregate(p_query_state, get_db()->get_method_list(p_type, p_no_inheritance));
}

PackedStringArray ClassDBTypeProvider::get_type_integer_constant_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance) const {
	return _aggregate(p_query_state, get_db()->get_integer_constant_list(p_type, p_no_inheritance));
}

bool ClassDBTypeProvider::has_integer_constant(Dictionary p_query_state, const Variant &p_type, const StringName &p_name) const {
	return _aggregate(p_query_state, get_db()->has_integer_constant(p_type, p_name));
}

int64_t ClassDBTypeProvider::get_integer_constant(Dictionary p_query_state, const Variant &p_type, const StringName &p_name) const {
	return _overwrite(p_query_state, get_db()->get_integer_constant(p_type, p_name));
}

bool ClassDBTypeProvider::has_enum(Dictionary p_query_state, const Variant &p_type, const StringName &p_name, bool p_no_inheritance) const {
	return _aggregate(p_query_state, get_db()->has_integer_constant(p_type, p_name));
}

PackedStringArray ClassDBTypeProvider::get_enum_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance) const {
	return _aggregate(p_query_state, get_db()->get_enum_list(p_type, p_no_inheritance));
}

PackedStringArray ClassDBTypeProvider::get_enum_constants(Dictionary p_query_state, const Variant &p_type, const StringName &p_enum, bool p_no_inheritance) const {
	return _aggregate(p_query_state, get_db()->get_enum_constants(p_type, p_enum, p_no_inheritance));
}

StringName ClassDBTypeProvider::get_integer_constant_enum(Dictionary p_query_state, const Variant &p_type, const StringName &p_name, bool p_no_inheritance) const {
	return _overwrite(p_query_state, get_db()->get_integer_constant_enum(p_type, p_name, p_no_inheritance));
}

bool ClassDBTypeProvider::is_type_enabled(Dictionary p_query_state, const Variant &p_type) const {
	return _aggregate(p_query_state, get_db()->is_class_enabled(p_type));
}

