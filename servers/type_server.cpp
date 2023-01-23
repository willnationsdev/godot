/*************************************************************************/
/*  type_server.cpp                                                      */
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

#include "servers/type_server.h"

#include "core/core_bind.h"

TypedArray<StringName> TypeProvider::extract_names(Variant p_type) const {
	TypedArray<StringName> ret;
	GDVIRTUAL_CALL(_extract_names, p_type, ret);

	StringName (*name_from_file)(const String &) = [](const String &path) {
		List<StringName> classes;
		ScriptServer::get_global_class_list(&classes);
		for (const StringName &E : classes) {
			if (path == ScriptServer::get_global_class_path(E)) {
				return E;
			}
		}
		return StringName();
	};

	StringName name;
	switch (p_type.get_type()) {
		case Variant::STRING: {
			String str = p_type;
			if (str.is_valid_identifier()) {
				name = str;
			} else if (FileAccess::exists(str)) {
				name = name_from_file(str);
			}
		} break;
		case Variant::STRING_NAME: {
			name = p_type.operator StringName();
		} break;
		case Variant::OBJECT: {
			Object *obj = p_type;
			if (obj) {
				Ref<Script> scr = obj->is_class("Script") ? obj : obj->get_script();
				if (scr.is_valid()) {
					const String &path = scr->get_path();
					name = name_from_file(path);
				}
			}
		} break;
		default:
			break;
	}
	if (name != StringName()) {
		ret.push_back(name);
	}
	return ret;
}

TypedArray<String> TypeProvider::extract_paths(Variant p_type) const {
	TypedArray<String> ret;
	GDVIRTUAL_CALL(_extract_paths, p_type, ret);
	return ret;
}

PackedStringArray TypeProvider::get_type_list(Dictionary p_query_state, bool p_no_named, bool p_no_anonymous) const {
	PackedStringArray ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_get_type_list, p_query_state, p_no_named, p_no_anonymous, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator PackedStringArray();
}

PackedStringArray TypeProvider::get_inheriters_from_type(Dictionary p_query_state, const Variant &p_type) const {
	PackedStringArray ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_get_inheriters_from_type, p_query_state, p_type, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator PackedStringArray();
}

StringName TypeProvider::get_parent_type(Dictionary p_query_state, const Variant &p_type) const {
	StringName ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_get_parent_type, p_query_state, p_type, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator StringName();
}

bool TypeProvider::type_exists(Dictionary p_query_state, const Variant &p_type) const {
	bool ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_type_exists, p_query_state, p_type, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator bool();
}

bool TypeProvider::is_parent_type(Dictionary p_query_state, const Variant &p_type, const Variant &p_inherits) const {
	bool ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_is_parent_type, p_query_state, p_type, p_inherits, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator bool();
}

bool TypeProvider::can_instantiate(Dictionary p_query_state, const Variant &p_type) const {
	bool ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_can_instantiate, p_query_state, p_type, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator bool();
}

Variant TypeProvider::instantiate(Dictionary p_query_state, const Variant &p_type) const {
	Variant ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_instantiate, p_query_state, p_type, ret)) {
		_set_result(p_query_state, ret);
	}
	return ret;
}

bool TypeProvider::has_signal(Dictionary p_query_state, const Variant &p_type, StringName p_signal) const {
	bool ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_has_signal, p_query_state, p_type, p_signal, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator bool();
}

Dictionary TypeProvider::get_signal(Dictionary p_query_state, const Variant &p_type, StringName p_signal) const {
	Dictionary ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_get_signal, p_query_state, p_type, p_signal, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator Dictionary();
}

TypedArray<Dictionary> TypeProvider::get_type_signal_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance) const {
	TypedArray<Dictionary> ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_get_type_signal_list, p_query_state, p_type, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator Array();
}

TypedArray<Dictionary> TypeProvider::get_type_property_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance) const {
	TypedArray<Dictionary> ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_get_type_property_list, p_query_state, p_type, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator Array();
}

Variant TypeProvider::get_property(Dictionary p_query_state, const Variant &p_source, const StringName &p_property) const {
	Variant ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_get_property, p_query_state, p_source, p_property, ret)) {
		_set_result(p_query_state, ret);
	}
	return ret;
}

Error TypeProvider::set_property(Dictionary p_query_state, const Variant &p_source, const StringName &p_property, const Variant &p_value) const {
	Error ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_set_property, p_query_state, p_source, p_property, p_value, ret)) {
		_set_result(p_query_state, ret);
	}
	return (Error)_get_result(p_query_state).operator uint64_t();
}

bool TypeProvider::has_method(Dictionary p_query_state, const Variant &p_type, StringName p_method, bool p_no_inheritance) const {
	bool ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_has_method, p_query_state, p_type, p_method, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator bool();
}

TypedArray<Dictionary> TypeProvider::get_type_method_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance) const {
	TypedArray<Dictionary> ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_get_type_method_list, p_query_state, p_type, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator Array();
}

PackedStringArray TypeProvider::get_type_integer_constant_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance) const {
	PackedStringArray ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_get_type_integer_constant_list, p_query_state, p_type, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator PackedStringArray();
}

bool TypeProvider::has_integer_constant(Dictionary p_query_state, const Variant &p_type, const StringName &p_name) const {
	bool ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_has_integer_constant, p_query_state, p_type, p_name, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator bool();
}

int64_t TypeProvider::get_integer_constant(Dictionary p_query_state, const Variant &p_type, const StringName &p_name) const {
	int64_t ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_get_integer_constant, p_query_state, p_type, p_name, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator int64_t();
}

bool TypeProvider::has_enum(Dictionary p_query_state, const Variant &p_type, const StringName &p_name, bool p_no_inheritance) const {
	bool ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_has_enum, p_query_state, p_type, p_name, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator bool();
}

PackedStringArray TypeProvider::get_enum_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance) const {
	PackedStringArray ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_get_enum_list, p_query_state, p_type, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator PackedStringArray();
}

PackedStringArray TypeProvider::get_enum_constants(Dictionary p_query_state, const Variant &p_type, const StringName &p_enum, bool p_no_inheritance) const {
	PackedStringArray ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_get_enum_constants, p_query_state, p_type, p_enum, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator PackedStringArray();
}

StringName TypeProvider::get_integer_constant_enum(Dictionary p_query_state, const Variant &p_type, const StringName &p_name, bool p_no_inheritance) const {
	StringName ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_get_integer_constant_enum, p_query_state, p_type, p_name, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator StringName();
}

bool TypeProvider::is_type_enabled(Dictionary p_query_state, const Variant &p_type) const {
	bool ret;
	if (_is_query_done(p_query_state) && GDVIRTUAL_CALL(_is_type_enabled, p_query_state, p_type, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator bool();
}

template <>
static bool TypeProvider::_aggregate<bool>(Dictionary p_query_state, bool p_value) {
	return TypeProvider::_get_result(p_query_state) || p_value;
}

template <>
static PackedStringArray TypeProvider::_aggregate<PackedStringArray>(Dictionary p_query_state, PackedStringArray p_value) {
	PackedStringArray result = _get_result(p_query_state);
	result.append_array(p_value);
	return result;
}

template <>
static TypedArray<Dictionary> TypeProvider::_aggregate<TypedArray<Dictionary>>(Dictionary p_query_state, TypedArray<Dictionary> p_value) {
	TypedArray<Dictionary> result = _get_result(p_query_state);
	result.append_array(p_value);
	return result;
}

template <>
static StringName TypeProvider::_overwrite<StringName>(Dictionary p_query_state, StringName p_value) {
	return _is_handled(p_query_state) ? _get_result(p_query_state) : p_value;
}

void TypeProvider::_bind_methods() {

	GDVIRTUAL_BIND(_get_type_list, "query_state", "no_named", "no_anonymous");
	GDVIRTUAL_BIND(_get_inheriters_from_type, "query_state", "type");
	GDVIRTUAL_BIND(_get_parent_type, "query_state", "type");
	GDVIRTUAL_BIND(_type_exists, "query_state", "type");
	GDVIRTUAL_BIND(_is_parent_type, "query_state", "type", "inherits");
	GDVIRTUAL_BIND(_can_instantiate, "query_state", "type");
	GDVIRTUAL_BIND(_instantiate, "query_state", "type");
	GDVIRTUAL_BIND(_has_signal, "query_state", "type", "signal");
	GDVIRTUAL_BIND(_get_signal, "query_state", "type", "signal");
	GDVIRTUAL_BIND(_get_type_signal_list, "query_state", "type", "no_inheritance");
	GDVIRTUAL_BIND(_get_type_property_list, "query_state", "type", "no_inheritance");
	GDVIRTUAL_BIND(_get_property, "query_state", "source", "property");
	GDVIRTUAL_BIND(_set_property, "query_state", "source", "property", "value");
	GDVIRTUAL_BIND(_has_method, "query_state", "type", "method", "no_inheritance");
	GDVIRTUAL_BIND(_get_type_method_list, "query_state", "type", "no_inheritance");
	GDVIRTUAL_BIND(_get_type_integer_constant_list, "query_state", "type", "no_inheritance");
	GDVIRTUAL_BIND(_has_integer_constant, "query_state", "type", "name");
	GDVIRTUAL_BIND(_get_integer_constant, "query_state", "type", "name");
	GDVIRTUAL_BIND(_has_enum, "query_state", "type", "name", "no_inheritance");
	GDVIRTUAL_BIND(_get_enum_list, "query_state", "type", "no_inheritance");
	GDVIRTUAL_BIND(_get_enum_constants, "query_state", "type", "enum", "no_inheritance");
	GDVIRTUAL_BIND(_get_integer_constant_enum, "query_state", "type", "name", "no_inheritance");
	GDVIRTUAL_BIND(_is_type_enabled, "query_state", "type");
}

/// TypeServer

TypeServer *TypeServer::singleton = nullptr;
TypeServer *(*TypeServer::create_func)() = nullptr;

Variant TypeServer::_process(Variant p_state, const Variant **p_args, int p_argcount, bool p_is_eager, bool p_aggregate, PackedStringArray p_filters, Callable(* p_callable_factory)(TypeProvider *)) const {
	RWLockRead rw(lock);
	Callable::CallError error;
	if (p_aggregate) {
		for (int i = _providers.size()-1; i >= 0; i--) {
			TypeProvider *p = _providers[i];
			if (!p_filters.is_empty() && p_filters.find(p->get_provider_name()) == -1) {
				continue;
			}
			Callable c = p_callable_factory(p);
			Variant ret;
			c.callp(p_args, p_argcount, ret, error);
			switch (ret.get_type()) {
				case Variant::BOOL: {
					bool result = TypeProvider::_get_result(p_state);
					TypeProvider::_set_result(p_state, result || ret.operator bool());
				} break;
				case Variant::ARRAY: {
					Array result = TypeProvider::_get_result(p_state);
					result.append_array(ret);
					TypeProvider::_set_result(p_state, result);
				} break;
				case Variant::PACKED_STRING_ARRAY: {
					PackedStringArray result = TypeProvider::_get_result(p_state);
					result.append_array(ret);
					TypeProvider::_set_result(p_state, result);
				} break;
				default: {
				} break;
			}
			if (!TypeProvider::_is_query_done(p_state)) {
				break;
			}
		}
	} else {
		for (int i = _providers.size()-1; i >= 0; i--) {
			TypeProvider *p = _providers[i];
			if (!p_filters.is_empty() && p_filters.find(p->get_provider_name()) == -1) {
				continue;
			}
			Callable c = p_callable_factory(p);
			Variant ret;
			c.callp(p_args, p_argcount, ret, error);
			TypeProvider::_set_result(p_state, ret);
			if (!TypeProvider::_is_query_done(p_state)) {
				break;
			}
		}
	}
	return p_state;
}

template <class ...VarArgs>
Variant TypeServer::_query(bool p_is_eager, bool p_aggregate, PackedStringArray p_filters, Callable (*p_handler)(TypeProvider *p), int p_argc, VarArgs... p_args) const {
	Variant args[] = { p_args... };
	const Variant *argsp = args;
	Dictionary state;
	state = _process(state, &argsp, p_argc, p_is_eager, p_aggregate, p_filters, p_handler);
	return TypeProvider::_get_result(state);
}

TypedArray<StringName> TypeServer::extract_names(Variant p_type, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::extract_names); }, 1, p_type).operator Array();
}

TypedArray<String> TypeServer::extract_paths(Variant p_type, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::extract_paths); }, 1, p_type).operator Array();
}

PackedStringArray TypeServer::get_type_list(bool p_no_named, bool p_no_anonymous, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::extract_paths); }, 2, p_no_named, p_no_anonymous).operator PackedStringArray();
}

PackedStringArray TypeServer::get_inheriters_from_type(const Variant &p_type, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::get_inheriters_from_type); }, 1, p_type).operator PackedStringArray();
}

StringName TypeServer::get_parent_type(const Variant &p_type, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, false, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::get_parent_type); }, 1, p_type).operator StringName();
}

bool TypeServer::type_exists(const Variant &p_type, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::type_exists); }, 1, p_type).operator bool();
}

bool TypeServer::is_parent_type(const Variant &p_type, const Variant &p_inherits, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::is_parent_type); }, 2, p_type, p_inherits).operator bool();
}

bool TypeServer::can_instantiate(const Variant &p_type, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::can_instantiate); }, 1, p_type).operator bool();
}

Variant TypeServer::instantiate(const Variant &p_type, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::instantiate); }, 1, p_type);
}

bool TypeServer::has_signal(const Variant &p_type, StringName p_signal, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::has_signal); }, 2, p_type, p_signal).operator bool();
}

Dictionary TypeServer::get_signal(const Variant &p_type, StringName p_signal, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, false, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::get_signal); }, 2, p_type, p_signal);
}

TypedArray<Dictionary> TypeServer::get_type_signal_list(const Variant &p_type, bool p_no_inheritance, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::get_type_signal_list); }, 2, p_type, p_no_inheritance).operator Array();
}

TypedArray<Dictionary> TypeServer::get_type_property_list(const Variant &p_type, bool p_no_inheritance, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::get_type_property_list); }, 2, p_type, p_no_inheritance).operator Array();
}

Variant TypeServer::get_property(const Variant &p_source, const StringName &p_property, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, false, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::get_property); }, 2, p_source, p_property);
}

Error TypeServer::set_property(const Variant &p_source, const StringName &p_property, const Variant &p_value, bool p_is_eager, PackedStringArray p_filters) const {
	int result = _query(p_is_eager, false, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::set_property); }, 3, p_source, p_property, p_value).operator int();
	return (Error)result;
}

bool TypeServer::has_method(const Variant &p_type, StringName p_method, bool p_no_inheritance, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::has_method); }, 2, p_type, p_method).operator bool();
}

TypedArray<Dictionary> TypeServer::get_type_method_list(const Variant &p_type, bool p_no_inheritance, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::get_type_method_list); }, 2, p_type, p_no_inheritance).operator Array();
}

PackedStringArray TypeServer::get_type_integer_constant_list(const Variant &p_type, bool p_no_inheritance, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::get_type_integer_constant_list); }, 2, p_type, p_no_inheritance).operator PackedStringArray();
}

bool TypeServer::has_integer_constant(const Variant &p_type, const StringName &p_name, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::has_integer_constant); }, 2, p_type, p_name).operator bool();
}

int64_t TypeServer::get_integer_constant(const Variant &p_type, const StringName &p_name, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, false, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::get_integer_constant); }, 2, p_type, p_name).operator int64_t();
}

bool TypeServer::has_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::has_enum); }, 2, p_type, p_name).operator bool();
}

PackedStringArray TypeServer::get_enum_list(const Variant &p_type, bool p_no_inheritance, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::get_enum_list); }, 2, p_type, p_no_inheritance).operator PackedStringArray();
}

PackedStringArray TypeServer::get_enum_constants(const Variant &p_type, const StringName &p_enum, bool p_no_inheritance, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::get_enum_constants); }, 2, p_type, p_no_inheritance).operator PackedStringArray();
}

StringName TypeServer::get_integer_constant_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, false, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::get_integer_constant_enum); }, 3, p_type, p_name, p_no_inheritance).operator StringName();
}

bool TypeServer::is_type_enabled(const Variant &p_type, bool p_is_eager, PackedStringArray p_filters) const {
	return _query(p_is_eager, true, p_filters, [](TypeProvider *p) { return callable_mp(p, &TypeProvider::is_type_enabled); }, 1, p_type).operator bool();
}

TypeProvider* TypeServer::get_provider(const StringName &p_name) const {
	RWLockRead rw(lock);
	for (TypeProvider *op : _providers) {
		if (op->get_provider_name() == p_name) {
			return op;
		}
	}
	ERR_FAIL_V_MSG(nullptr, vformat("No %s with name '%s' has been registered.", SNAME("TypeProvider"), p_name));
}

void TypeServer::add_provider(TypeProvider * const p_provider) {
	RWLockWrite rw(lock);
	ERR_FAIL_COND_MSG(!p_provider, vformat("Cannot add a null %s", SNAME("TypeProvider")));
	for (const TypeProvider *op : _providers) {
		ERR_FAIL_COND_MSG(op == p_provider, vformat("The given %s with name '%s' has already been registered.", SNAME("TypeProvider"), p_provider->get_provider_name()));
	}
	_providers.append(p_provider);
}

void TypeServer::remove_provider(const TypeProvider *p_provider) {
	RWLockWrite rw(lock);
	ERR_FAIL_COND_MSG(!p_provider, vformat("Cannot remove a null %s", SNAME("TypeProvider")));
	for (int i = 0; i < _providers.size(); i++) {
		if (_providers[i] == p_provider) {
			_providers.remove_at(i);
		}
	}
	ERR_FAIL_MSG(vformat("The given %s with name '%s' is not present and could not be removed.", SNAME("TypeProvider"), p_provider->get_provider_name()));
}

