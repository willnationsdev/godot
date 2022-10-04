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

class TypeOperator : public Object {

	friend class TypeServer;

	mutable bool _handled = false;
	mutable bool _eager = false;
	mutable Variant _result;

protected:
	static void _bind_methods();

	_FORCE_INLINE_ void mark_handled() const { _handled = true; }
	_FORCE_INLINE_ bool is_handled() const { return _handled; }
	_FORCE_INLINE_ bool is_eager() const { return _eager; }
	_FORCE_INLINE_ Variant get_result() const { _handled = true; }
	_FORCE_INLINE_ void set_result(Variant p_value) const { _result = p_value; }

	GDVIRTUAL2RC(PackedStringArray, _get_type_list, bool, bool);
	GDVIRTUAL1RC(PackedStringArray, _get_inheriters_from_type, Variant);
	GDVIRTUAL1RC(StringName, _get_parent_type, Variant);
	GDVIRTUAL1RC(bool, _type_exists, Variant);
	GDVIRTUAL2RC(bool, _is_parent_type, Variant, Variant);
	GDVIRTUAL1RC(bool, _can_instantiate, Variant);
	GDVIRTUAL1RC(Variant, _instantiate, Variant);
	GDVIRTUAL2RC(bool, _has_signal, Variant, StringName);
	GDVIRTUAL2RC(Dictionary, _get_signal, Variant, StringName);
	GDVIRTUAL2RC(TypedArray<Dictionary>, _get_type_signal_list, Variant, bool);
	GDVIRTUAL2RC(TypedArray<Dictionary>, _get_type_property_list, Variant, bool);
	GDVIRTUAL2RC(Variant, _get_property, Variant, StringName);
	GDVIRTUAL3RC(Error, _set_property, Variant, StringName, Variant);
	GDVIRTUAL3RC(bool, _has_method, Variant, StringName, bool);
	GDVIRTUAL2RC(TypedArray<Dictionary>, _get_type_method_list, Variant, bool);
	GDVIRTUAL2RC(PackedStringArray, _get_type_integer_constant_list, Variant, bool);
	GDVIRTUAL2RC(bool, _has_integer_constant, Variant, StringName);
	GDVIRTUAL2RC(int64_t, _get_integer_constant, Variant, StringName);
	GDVIRTUAL3RC(bool, _has_enum, Variant, StringName, bool);
	GDVIRTUAL2RC(PackedStringArray, _get_enum_list, Variant, bool);
	GDVIRTUAL3RC(PackedStringArray, _get_enum_constants, Variant, StringName, bool);
	GDVIRTUAL3RC(StringName, _get_integer_constant_enum, Variant, StringName, bool);
	GDVIRTUAL1RC(bool, _is_type_enabled, Variant);

public:

	virtual StringName get_operator_name() const { return StringName(); }
	virtual void configure(bool p_eager, bool p_handled, Variant result);

	virtual PackedStringArray get_type_list(bool p_no_named = false, bool p_no_anonymous = true) const;
	virtual PackedStringArray get_inheriters_from_type(const Variant &p_type) const;
	virtual StringName get_parent_type(const Variant &p_type) const;
	virtual bool type_exists(const Variant &p_type) const;
	virtual bool is_parent_type(const Variant &p_type, const Variant &p_inherits) const;
	virtual bool can_instantiate(const Variant &p_type) const;
	virtual Variant instantiate(const Variant &p_type) const;
	virtual bool has_signal(const Variant &p_type, StringName p_signal) const;
	virtual Dictionary get_signal(const Variant &p_type, StringName p_signal) const;
	virtual TypedArray<Dictionary> get_type_signal_list(const Variant &p_type, bool p_no_inheritance = false) const;
	virtual TypedArray<Dictionary> get_type_property_list(const Variant &p_type, bool p_no_inheritance = false) const;
	virtual Variant get_property(const Variant &p_source, const StringName &p_property) const;
	virtual Error set_property(const Variant &p_source, const StringName &p_property, const Variant &p_value) const;
	virtual bool has_method(const Variant &p_type, StringName p_method, bool p_no_inheritance = false) const;
	virtual TypedArray<Dictionary> get_type_method_list(const Variant &p_type, bool p_no_inheritance = false) const;
	virtual PackedStringArray get_type_integer_constant_list(const Variant &p_type, bool p_no_inheritance = false) const;
	virtual bool has_integer_constant(const Variant &p_type, const StringName &p_name) const;
	virtual int64_t get_integer_constant(const Variant &p_type, const StringName &p_name) const;
	virtual bool has_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance = false) const;
	virtual PackedStringArray get_enum_list(const Variant &p_type, bool p_no_inheritance = false) const;
	virtual PackedStringArray get_enum_constants(const Variant &p_type, const StringName &p_enum, bool p_no_inheritance = false) const;
	virtual StringName get_integer_constant_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance = false) const;
	virtual bool is_type_enabled(const Variant &p_type) const;

	TypeOperator() {}
	~TypeOperator() {}
};

class TypeServer {

	struct OpContext {
		bool eager;
		bool handled = false;
		Variant result;

		OpContext(bool p_eager, Variant p_result) :
				eager(p_eager), result(p_result) {}
	};

	struct TypeListOpContext : OpContext {
		bool no_named;
		bool no_anonymous;

		TypeListOpContext(bool p_no_named, bool p_no_anonymous, bool p_eager, Variant r_result) :
				OpContext(p_eager, r_result), no_named(p_no_named), no_anonymous(p_no_anonymous) {}
	};

	struct TypeOpContext : OpContext {
		Variant type;

		TypeOpContext(Variant p_type, bool p_eager, Variant r_result) :
				OpContext(p_eager, r_result), type(p_type) {}
	};

	struct TypeInheritsOpContext : TypeOpContext {
		Variant inherits;

		TypeInheritsOpContext(Variant p_type, Variant p_inherits, bool p_eager, Variant r_result) :
				TypeOpContext(p_type, p_eager, r_result), inherits(p_inherits) {}
	};

	struct TypeMemberOpContext : TypeOpContext {
		StringName name;

		TypeMemberOpContext(Variant p_type, StringName p_name, bool p_eager, Variant r_result) :
				TypeOpContext(p_type, p_eager, r_result), name(p_name) {}
	};

	struct TypeMemberValueOpContext : TypeMemberOpContext {
		Variant value;

		TypeMemberValueOpContext(Variant p_type, StringName p_name, Variant p_value, bool p_eager, Variant r_result) :
				TypeMemberOpContext(p_type, p_name, p_eager, r_result), value(p_value) {}
	};

	struct TypeQueryOpContext : TypeOpContext {
		bool no_inheritance;

		TypeQueryOpContext(Variant p_type, bool p_no_inheritance, bool p_eager, Variant r_result) :
				TypeOpContext(p_type, p_eager, r_result), no_inheritance(p_no_inheritance) {}
	};

	struct TypeMemberQueryOpContext : TypeQueryOpContext {
		StringName name;

		TypeMemberQueryOpContext(Variant p_type, StringName p_name, bool p_no_inheritance, bool p_eager, Variant r_result) :
				TypeQueryOpContext(p_type, p_no_inheritance, p_eager, r_result), name(p_name) {}
	};

	typedef void (*Handler)(TypeOperator &p_op, OpContext *r_context);
	void _process(OpContext *r_context, Handler p_handler) const;

	Vector<TypeOperator *> _operators;

	static TypeServer *singleton;

protected:
	static TypeServer *(*create_func)();
	static void _bind_methods();

	template <class T>
	static TypeServer *_create_builtin() {
		return memnew(T);
	}
	
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

	TypeOperator *get_operator(const StringName &p_name) const;
	void add_operator(TypeOperator * const p_operator);
	void remove_operator(const TypeOperator *p_operator);

	virtual PackedStringArray get_type_list(bool p_no_named = false, bool p_no_anonymous = true, bool p_first = false) const;
	virtual PackedStringArray get_inheriters_from_type(const Variant &p_type, bool p_first = false) const;
	virtual StringName get_parent_type(const Variant &p_type, bool p_first = true) const;
	virtual bool type_exists(const Variant &p_type, bool p_first = false) const;
	virtual bool is_parent_type(const Variant &p_type, const Variant &p_inherits, bool p_first = true) const;
	virtual bool can_instantiate(const Variant &p_type, bool p_first = false) const;
	virtual Variant instantiate(const Variant &p_type, bool p_first = true) const;
	virtual bool has_signal(const Variant &p_type, StringName p_signal, bool p_first = false) const;
	virtual Dictionary get_signal(const Variant &p_type, StringName p_signal, bool p_first = true) const;
	virtual TypedArray<Dictionary> get_type_signal_list(const Variant &p_type, bool p_no_inheritance = false, bool p_first = false) const;
	virtual TypedArray<Dictionary> get_type_property_list(const Variant &p_type, bool p_no_inheritance = false, bool p_first = false) const;
	virtual Variant get_property(const Variant &p_source, const StringName &p_property, bool p_first = true) const;
	virtual Error set_property(const Variant &p_source, const StringName &p_property, const Variant &p_value, bool p_first = true) const;
	virtual bool has_method(const Variant &p_type, StringName p_method, bool p_no_inheritance = false, bool p_first = false) const;
	virtual TypedArray<Dictionary> get_type_method_list(const Variant &p_type, bool p_no_inheritance = false, bool p_first = false) const;
	virtual PackedStringArray get_type_integer_constant_list(const Variant &p_type, bool p_no_inheritance = false, bool p_first = false) const;
	virtual bool has_integer_constant(const Variant &p_type, const StringName &p_name, bool p_first = false) const;
	virtual int64_t get_integer_constant(const Variant &p_type, const StringName &p_name, bool p_first = true) const;
	virtual bool has_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance = false, bool p_first = false) const;
	virtual PackedStringArray get_enum_list(const Variant &p_type, bool p_no_inheritance = false, bool p_first = false) const;
	virtual PackedStringArray get_enum_constants(const Variant &p_type, const StringName &p_enum, bool p_no_inheritance = false, bool p_first = false) const;
	virtual StringName get_integer_constant_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance = false, bool p_first = true) const;
	virtual bool is_type_enabled(const Variant &p_type, bool p_first = false) const;

	TypeServer() {}
	~TypeServer() {}
};

#endif // TYPE_SERVER_H
