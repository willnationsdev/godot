/*************************************************************************/
/*  script_language.h                                                    */
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
#include "core/object/object.h"

class TypeHandle;

class TypeOperator : public Object {

public:
	enum TypeOpKind {
		TYPE_OP_KIND_INSTANTIATE,
		TYPE_OP_KIND_QUERY_INHERITANCE,
		TYPE_OP_KIND_QUERY_MEMBERS,
		TYPE_OP_KIND_QUERY_STATUS,
	};

protected:
	static void _bind_methods();

public:
	virtual TypeOperator *get_parent() const = 0;
	virtual StringName get_operator_name() const = 0;
	virtual int has_priority(const Variant &p_type, TypeOpKind p_op_kind) const = 0;

	virtual PackedStringArray get_type_list(bool p_no_named = false, bool p_no_anonymous = true) const = 0;
	virtual PackedStringArray get_inheriters_from_type(const Variant &p_type) const = 0;
	virtual StringName get_parent_type(const Variant &p_type) const = 0;
	virtual bool type_exists(const Variant &p_type) const = 0;
	virtual bool is_parent_type(const Variant &p_type, const Variant &p_inherits) const = 0;
	virtual bool can_instantiate(const Variant &p_type) const = 0;
	virtual Variant instantiate(const Variant &p_type) const = 0;

	virtual bool has_signal(const Variant &p_type, StringName p_signal) const = 0;
	virtual Dictionary get_signal(const Variant &p_type, StringName p_signal) const = 0;
	virtual TypedArray<Dictionary> get_signal_list(const Variant &p_type, bool p_no_inheritance = false) const = 0;

	virtual TypedArray<Dictionary> get_property_list(const Variant &p_type, bool p_no_inheritance = false) const = 0;
	virtual Variant get_property(const Variant &p_source, const StringName &p_property) const = 0;
	virtual Error set_property(const Variant &p_source, const StringName &p_property, const Variant &p_value) const = 0;

	virtual bool has_method(const Variant &p_type, StringName p_method, bool p_no_inheritance = false) const = 0;

	virtual TypedArray<Dictionary> get_method_list(const Variant &p_type, bool p_no_inheritance = false) const = 0;

	virtual PackedStringArray get_integer_constant_list(const Variant &p_type, bool p_no_inheritance = false) const = 0;
	virtual bool has_integer_constant(const Variant &p_type, const StringName &p_name) const = 0;
	virtual int64_t get_integer_constant(const Variant &p_type, const StringName &p_name) const = 0;

	virtual bool has_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance = false) const = 0;
	virtual PackedStringArray get_enum_list(const Variant &p_type, bool p_no_inheritance = false) const = 0;
	virtual PackedStringArray get_enum_constants(const Variant &p_type, const StringName &p_enum, bool p_no_inheritance = false) const = 0;
	virtual StringName get_integer_constant_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance = false) const = 0;

	virtual bool is_type_enabled(const Variant &p_type) const = 0;

	TypeOperator() {}
	~TypeOperator() {}
};

class TypeServer : public TypeOperator {

public:
	enum TypeOpFilter {
		TYPE_OP_FILTER_SUPPORTS,
		TYPE_OP_FILTER_MAX_PRIORITY,
	};

private:
	Vector<TypeOperator *> _operators;
	
public:
	TypeOperator *TypeServer::find_best_fit_operator(const Variant &p_type, TypeOpKind p_op_kind) const;
	const Vector<TypeOperator *> &TypeServer::filter_operators(const Variant &p_type, TypeOpKind p_op_kind, TypeOpFilter p_op_filter) const;
	bool get_operator(const StringName &p_name, TypeOperator *r_operator) const;
	void add_operator(TypeOperator * const p_operator);
	void remove_operator(const TypeOperator *p_operator);

	// TypeOperator overrides

	virtual StringName get_operator_name() const override;
	virtual int has_priority(const Variant &p_type, TypeOpKind p_op_kind) const override;

	virtual PackedStringArray get_type_list(bool p_no_named = false, bool p_no_anonymous = true) const override;
	virtual PackedStringArray get_inheriters_from_type(const Variant &p_type) const override;
	virtual StringName get_parent_type(const Variant &p_type) const override;
	virtual bool type_exists(const Variant &p_type) const override;
	virtual bool is_parent_type(const Variant &p_type, const Variant &p_inherits) const override;
	virtual bool can_instantiate(const Variant &p_type) const override;
	virtual Variant instantiate(const Variant &p_type) const override;

	virtual bool has_signal(const Variant &p_type, StringName p_signal) const override;
	virtual Dictionary get_signal(const Variant &p_type, StringName p_signal) const override;
	virtual TypedArray<Dictionary> get_signal_list(const Variant &p_type, bool p_no_inheritance = false) const override;

	virtual TypedArray<Dictionary> get_property_list(const Variant &p_type, bool p_no_inheritance = false) const override;
	virtual Variant get_property(const Variant &p_source, const StringName &p_property) const override;
	virtual Error set_property(const Variant &p_source, const StringName &p_property, const Variant &p_value) const override;

	virtual bool has_method(const Variant &p_type, StringName p_method, bool p_no_inheritance = false) const override;

	virtual TypedArray<Dictionary> get_method_list(const Variant &p_type, bool p_no_inheritance = false) const override;

	virtual PackedStringArray get_integer_constant_list(const Variant &p_type, bool p_no_inheritance = false) const override;
	virtual bool has_integer_constant(const Variant &p_type, const StringName &p_name) const override;
	virtual int64_t get_integer_constant(const Variant &p_type, const StringName &p_name) const override;

	virtual bool has_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance = false) const override;
	virtual PackedStringArray get_enum_list(const Variant &p_type, bool p_no_inheritance = false) const override;
	virtual PackedStringArray get_enum_constants(const Variant &p_type, const StringName &p_enum, bool p_no_inheritance = false) const override;
	virtual StringName get_integer_constant_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance = false) const override;

	virtual bool is_type_enabled(const Variant &p_type) const override;

	// End TypeOperator overrides


	TypeServer() {}
	~TypeServer() {}
};

#endif // TYPE_SERVER_H
