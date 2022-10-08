/*************************************************************************/
/*  type_server.h                                                        */
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

#ifndef TYPE_SERVER_H
#define TYPE_SERVER_H

#include "core/object/class_db.h"
#include "core/object/gdvirtual.gen.inc"
#include "core/object/object.h"
#include "core/object/script_language.h"
#include "core/variant/typed_array.h"

class TypeHandle;
class TypeServer;

class TypeProvider : public Object {

	friend class TypeServer;

protected:
	static void _bind_methods();

	static _FORCE_INLINE_ void _mark_handled(Dictionary p_query_state) { p_query_state["is_handled"] = true; }
	static _FORCE_INLINE_ bool _is_handled(Dictionary p_query_state) { return p_query_state["is_handled"].operator bool(); }
	static _FORCE_INLINE_ bool _is_eager(Dictionary p_query_state) { return p_query_state["is_eager"].operator bool(); }
	static _FORCE_INLINE_ Variant _get_result(Dictionary p_query_state) { return p_query_state["result"]; }
	static _FORCE_INLINE_ void _set_result(Dictionary p_query_state, const Variant &p_value) { p_query_state["result"] = p_value; }
	static _FORCE_INLINE_ bool _is_query_done(Dictionary p_query_state) { return !(_is_eager(p_query_state) && _is_handled(p_query_state)); }
	template <class T>
	static _FORCE_INLINE_ bool _is_query_done(bool p_is_eager, TypedArray<T> p_state) { return p_is_eager && !p_state.is_empty(); }
	template <class T>
	static _FORCE_INLINE_ T _result_as(Dictionary p_query_state) { return p_query_state["result"].operator T(); }

	template <class T>
	static T _aggregate(Dictionary p_query_state, T p_value);

	template <class T>
	static T _overwrite(Dictionary p_query_state, T p_value);

	GDVIRTUAL1RC(TypedArray<StringName>, _extract_names, Variant);
	GDVIRTUAL1RC(TypedArray<String>, _extract_paths, Variant);
	GDVIRTUAL3RC(PackedStringArray, _get_type_list, Dictionary, bool, bool);
	GDVIRTUAL2RC(PackedStringArray, _get_inheriters_from_type, Dictionary, Variant);
	GDVIRTUAL2RC(StringName, _get_parent_type, Dictionary, Variant);
	GDVIRTUAL2RC(bool, _type_exists, Dictionary, Variant);
	GDVIRTUAL3RC(bool, _is_parent_type, Dictionary, Variant, Variant);
	GDVIRTUAL2RC(bool, _can_instantiate, Dictionary, Variant);
	GDVIRTUAL2RC(Variant, _instantiate, Dictionary, Variant);
	GDVIRTUAL3RC(bool, _has_signal, Dictionary, Variant, StringName);
	GDVIRTUAL3RC(Dictionary, _get_signal, Dictionary, Variant, StringName);
	GDVIRTUAL3RC(TypedArray<Dictionary>, _get_type_signal_list, Dictionary, Variant, bool);
	GDVIRTUAL3RC(TypedArray<Dictionary>, _get_type_property_list, Dictionary, Variant, bool);
	GDVIRTUAL3RC(Variant, _get_property, Dictionary, Variant, StringName);
	GDVIRTUAL4RC(Error, _set_property, Dictionary, Variant, StringName, Variant);
	GDVIRTUAL4RC(bool, _has_method, Dictionary, Variant, StringName, bool);
	GDVIRTUAL3RC(TypedArray<Dictionary>, _get_type_method_list, Dictionary, Variant, bool);
	GDVIRTUAL3RC(PackedStringArray, _get_type_integer_constant_list, Dictionary, Variant, bool);
	GDVIRTUAL3RC(bool, _has_integer_constant, Dictionary, Variant, StringName);
	GDVIRTUAL3RC(int64_t, _get_integer_constant, Dictionary, Variant, StringName);
	GDVIRTUAL4RC(bool, _has_enum, Dictionary, Variant, StringName, bool);
	GDVIRTUAL3RC(PackedStringArray, _get_enum_list, Dictionary, Variant, bool);
	GDVIRTUAL4RC(PackedStringArray, _get_enum_constants, Dictionary, Variant, StringName, bool);
	GDVIRTUAL4RC(StringName, _get_integer_constant_enum, Dictionary, Variant, StringName, bool);
	GDVIRTUAL2RC(bool, _is_type_enabled, Dictionary, Variant);

public:

	virtual StringName get_provider_name() const { return StringName(); }

	virtual TypedArray<StringName> extract_names(Variant p_type) const;
	virtual TypedArray<String> extract_paths(Variant p_type) const;

	virtual PackedStringArray get_type_list(Dictionary p_query_state, bool p_no_named = false, bool p_no_anonymous = true) const;
	virtual PackedStringArray get_inheriters_from_type(Dictionary p_query_state, const Variant &p_type) const;
	virtual StringName get_parent_type(Dictionary p_query_state, const Variant &p_type) const;
	virtual bool type_exists(Dictionary p_query_state, const Variant &p_type) const;
	virtual bool is_parent_type(Dictionary p_query_state, const Variant &p_type, const Variant &p_inherits) const;
	virtual bool can_instantiate(Dictionary p_query_state, const Variant &p_type) const;
	virtual Variant instantiate(Dictionary p_query_state, const Variant &p_type) const;
	virtual bool has_signal(Dictionary p_query_state, const Variant &p_type, StringName p_signal) const;
	virtual Dictionary get_signal(Dictionary p_query_state, const Variant &p_type, StringName p_signal) const;
	virtual TypedArray<Dictionary> get_type_signal_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance = false) const;
	virtual TypedArray<Dictionary> get_type_property_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance = false) const;
	virtual Variant get_property(Dictionary p_query_state, const Variant &p_source, const StringName &p_property) const;
	virtual Error set_property(Dictionary p_query_state, const Variant &p_source, const StringName &p_property, const Variant &p_value) const;
	virtual bool has_method(Dictionary p_query_state, const Variant &p_type, StringName p_method, bool p_no_inheritance = false) const;
	virtual TypedArray<Dictionary> get_type_method_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance = false) const;
	virtual PackedStringArray get_type_integer_constant_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance = false) const;
	virtual bool has_integer_constant(Dictionary p_query_state, const Variant &p_type, const StringName &p_name) const;
	virtual int64_t get_integer_constant(Dictionary p_query_state, const Variant &p_type, const StringName &p_name) const;
	virtual bool has_enum(Dictionary p_query_state, const Variant &p_type, const StringName &p_name, bool p_no_inheritance = false) const;
	virtual PackedStringArray get_enum_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance = false) const;
	virtual PackedStringArray get_enum_constants(Dictionary p_query_state, const Variant &p_type, const StringName &p_enum, bool p_no_inheritance = false) const;
	virtual StringName get_integer_constant_enum(Dictionary p_query_state, const Variant &p_type, const StringName &p_name, bool p_no_inheritance = false) const;
	virtual bool is_type_enabled(Dictionary p_query_state, const Variant &p_type) const;

	TypeProvider() {}
	~TypeProvider() {}
};

class TypeServer {

	Vector<TypeProvider *> _providers;

	static TypeServer *singleton;

	Dictionary _new_query_state(bool p_eager = false) const {
		Dictionary d;
		d["is_handled"] = false;
		d["is_eager"] = p_eager;
		d["result"] = Variant();
		return d;
	}

	//template <class T>
	//Dictionary _process(Variant *p_args, bool p_is_eager, PackedStringArray p_filters, T (*p_handler)(Dictionary state, TypeProvider *provider, Variant *args)) const;
	template <class TResult, class TState>
	TState _process(TState p_state, Variant *p_args, bool p_is_eager, PackedStringArray p_filters, TResult (*p_handler)(TState state, TypeProvider *provider, Variant *args)) const;
	template <class T>
	static _FORCE_INLINE_ T _as_result(Dictionary p_state) { return TypeProvider::_get_result(p_state).operator T(); }

	template <class T, class ...VarArgs>
	Variant _query(bool p_is_eager, PackedStringArray p_filters, T (TypeProvider::*p_handler)(Dictionary state, const Variant &type, VarArgs... args) const, VarArgs... p_args) const;
	template <class T, class V1, class ...VarArgs>
	Variant _query(bool p_is_eager, PackedStringArray p_filters, T (TypeProvider::*p_handler)(Dictionary state, const Variant &type, const V1 &v1, VarArgs... args) const, const V1 &p_v1, VarArgs... p_args) const;
	template <class T, class V1, class V2, class ...VarArgs>
	Variant _query(bool p_is_eager, PackedStringArray p_filters, T (TypeProvider::*p_handler)(Dictionary state, const Variant &type, const V1 &v1, const V2 &v2, VarArgs... args) const, const V1 &p_v1, const V2 &p_v2, VarArgs... p_args) const;
	template <class T, class V1, class V2, class V3, class ...VarArgs>
	Variant _query(bool p_is_eager, PackedStringArray p_filters, T (TypeProvider::*p_handler)(Dictionary state, const Variant &type, const V1 &v1, const V2 &v2, const V3 &v3, VarArgs... args) const, const V1 &p_v1, const V2 &p_v2, const V3 &p_v3, VarArgs... p_args) const;

protected:
	static TypeServer *(*create_func)();
	static void _bind_methods();

	template <class T>
	static TypeServer *_create_builtin() {
		return memnew(T);
	}

	static RWLock lock;
	static RWLock loop_lock;
	
public:
	static TypeServer* get_singleton() {
		return singleton;
	}

	template <class T>
	static void make_default() {
		create_func = _create_builtin<T>;
	}

	static TypeServer *create() {
		TypeServer *server = create_func ? create_func() : memnew(TypeServer);
		return server;
	};

	TypeProvider *get_provider(const StringName &p_name) const;
	void add_provider(TypeProvider * const p_provider);
	void remove_provider(const TypeProvider *p_provider);

	virtual TypedArray<StringName> extract_names(Variant p_type, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;
	virtual TypedArray<String> extract_paths(Variant p_type, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;

	virtual PackedStringArray get_type_list(bool p_no_named = false, bool p_no_anonymous = true, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;
	virtual PackedStringArray get_inheriters_from_type(const Variant &p_type, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;
	virtual StringName get_parent_type(const Variant &p_type, bool p_is_eager = true, PackedStringArray p_filters = PackedStringArray()) const;
	virtual bool type_exists(const Variant &p_type, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;
	virtual bool is_parent_type(const Variant &p_type, const Variant &p_inherits, bool p_is_eager = true, PackedStringArray p_filters = PackedStringArray()) const;
	virtual bool can_instantiate(const Variant &p_type, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;
	virtual Variant instantiate(const Variant &p_type, bool p_is_eager = true, PackedStringArray p_filters = PackedStringArray()) const;
	virtual bool has_signal(const Variant &p_type, StringName p_signal, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;
	virtual Dictionary get_signal(const Variant &p_type, StringName p_signal, bool p_is_eager = true, PackedStringArray p_filters = PackedStringArray()) const;
	virtual TypedArray<Dictionary> get_type_signal_list(const Variant &p_type, bool p_no_inheritance = false, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;
	virtual TypedArray<Dictionary> get_type_property_list(const Variant &p_type, bool p_no_inheritance = false, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;
	virtual Variant get_property(const Variant &p_source, const StringName &p_property, bool p_is_eager = true, PackedStringArray p_filters = PackedStringArray()) const;
	virtual Error set_property(const Variant &p_source, const StringName &p_property, const Variant &p_value, bool p_is_eager = true, PackedStringArray p_filters = PackedStringArray()) const;
	virtual bool has_method(const Variant &p_type, StringName p_method, bool p_no_inheritance = false, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;
	virtual TypedArray<Dictionary> get_type_method_list(const Variant &p_type, bool p_no_inheritance = false, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;
	virtual PackedStringArray get_type_integer_constant_list(const Variant &p_type, bool p_no_inheritance = false, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;
	virtual bool has_integer_constant(const Variant &p_type, const StringName &p_name, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;
	virtual int64_t get_integer_constant(const Variant &p_type, const StringName &p_name, bool p_is_eager = true, PackedStringArray p_filters = PackedStringArray()) const;
	virtual bool has_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance = false, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;
	virtual PackedStringArray get_enum_list(const Variant &p_type, bool p_no_inheritance = false, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;
	virtual PackedStringArray get_enum_constants(const Variant &p_type, const StringName &p_enum, bool p_no_inheritance = false, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;
	virtual StringName get_integer_constant_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance = false, bool p_is_eager = true, PackedStringArray p_filters = PackedStringArray()) const;
	virtual bool is_type_enabled(const Variant &p_type, bool p_is_eager = false, PackedStringArray p_filters = PackedStringArray()) const;

	TypeServer() {}
	~TypeServer() {}
};

#endif // TYPE_SERVER_H
