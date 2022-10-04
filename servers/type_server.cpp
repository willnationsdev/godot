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

void TypeOperator::configure(bool p_eager, bool p_handled, Variant result) {
	_eager = p_eager;
	_handled = p_handled;
	_result = result;
}

PackedStringArray TypeOperator::get_type_list(bool p_no_named = false, bool p_no_anonymous = true) const {
	PackedStringArray ret;
	if (GDVIRTUAL_CALL(_get_type_list, p_no_named, p_no_anonymous, ret)) {
		return ret;
	}
	return ret;
}

PackedStringArray TypeOperator::get_inheriters_from_type(const Variant &p_type) const {
	PackedStringArray ret;
	if (GDVIRTUAL_CALL(_get_inheriters_from_type, p_type, ret)) {
		return ret;
	}
	return ret;
}

StringName TypeOperator::get_parent_type(const Variant &p_type) const {
	StringName ret;
	if (GDVIRTUAL_CALL(_get_parent_type, p_type, ret)) {
		return ret;
	}
	return ret;
}

bool TypeOperator::type_exists(const Variant &p_type) const {
	bool ret;
	if (GDVIRTUAL_CALL(_type_exists, p_type, ret)) {
		return ret;
	}
	return ret;
}

bool TypeOperator::is_parent_type(const Variant &p_type, const Variant &p_inherits) const {
	bool ret;
	if (GDVIRTUAL_CALL(_is_parent_type, p_type, p_inherits, ret)) {
		return ret;
	}
	return ret;
}

bool TypeOperator::can_instantiate(const Variant &p_type) const {
	bool ret;
	if (GDVIRTUAL_CALL(_can_instantiate, p_type, ret)) {
		return ret;
	}
	return ret;
}

Variant TypeOperator::instantiate(const Variant &p_type) const {
	Variant ret;
	if (GDVIRTUAL_CALL(_instantiate, p_type, ret)) {
		return ret;
	}
	return ret;
}

bool TypeOperator::has_signal(const Variant &p_type, StringName p_signal) const {
	bool ret;
	if (GDVIRTUAL_CALL(_has_signal, p_type, p_signal, ret)) {
		return ret;
	}
	return ret;
}

Dictionary TypeOperator::get_signal(const Variant &p_type, StringName p_signal) const {
	Dictionary ret;
	if (GDVIRTUAL_CALL(_get_signal, p_type, p_signal, ret)) {
		return ret;
	}
	return ret;
}

TypedArray<Dictionary> TypeOperator::get_type_signal_list(const Variant &p_type, bool p_no_inheritance) const {
	TypedArray<Dictionary> ret;
	if (GDVIRTUAL_CALL(_get_type_signal_list, p_type, p_no_inheritance, ret)) {
		return ret;
	}
	return ret;
}

TypedArray<Dictionary> TypeOperator::get_type_property_list(const Variant &p_type, bool p_no_inheritance) const {
	TypedArray<Dictionary> ret;
	if (GDVIRTUAL_CALL(_get_type_property_list, p_type, p_no_inheritance, ret)) {
		return ret;
	}
	return ret;
}

Variant TypeOperator::get_property(const Variant &p_source, const StringName &p_property) const {
	Variant ret;
	if (GDVIRTUAL_CALL(_get_property, p_source, p_property, ret)) {
		return ret;
	}
	return ret;
}

Error TypeOperator::set_property(const Variant &p_source, const StringName &p_property, const Variant &p_value) const {
	Error ret;
	if (GDVIRTUAL_CALL(_set_property, p_source, p_property, p_value, ret)) {
		return ret;
	}
	return ret;
}

bool TypeOperator::has_method(const Variant &p_type, StringName p_method, bool p_no_inheritance) const {
	bool ret;
	if (GDVIRTUAL_CALL(_has_method, p_type, p_method, p_no_inheritance, ret)) {
		return ret;
	}
	return ret;
}

TypedArray<Dictionary> TypeOperator::get_type_method_list(const Variant &p_type, bool p_no_inheritance) const {
	TypedArray<Dictionary> ret;
	if (GDVIRTUAL_CALL(_get_type_method_list, p_type, p_no_inheritance, ret)) {
		return ret;
	}
	return ret;
}

PackedStringArray TypeOperator::get_type_integer_constant_list(const Variant &p_type, bool p_no_inheritance) const {
	PackedStringArray ret;
	if (GDVIRTUAL_CALL(_get_type_integer_constant_list, p_type, p_no_inheritance, ret)) {
		return ret;
	}
	return ret;
}

bool TypeOperator::has_integer_constant(const Variant &p_type, const StringName &p_name) const {
	bool ret;
	if (GDVIRTUAL_CALL(_has_integer_constant, p_type, p_name, ret)) {
		return ret;
	}
	return ret;
}

int64_t TypeOperator::get_integer_constant(const Variant &p_type, const StringName &p_name) const {
	int64_t ret;
	if (GDVIRTUAL_CALL(_get_integer_constant, p_type, p_name, ret)) {
		return ret;
	}
	return ret;
}

bool TypeOperator::has_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance) const {
	bool ret;
	if (GDVIRTUAL_CALL(_has_enum, p_type, p_name, p_no_inheritance, ret)) {
		return ret;
	}
	return ret;
}

PackedStringArray TypeOperator::get_enum_list(const Variant &p_type, bool p_no_inheritance) const {
	PackedStringArray ret;
	if (GDVIRTUAL_CALL(_get_enum_list, p_type, p_no_inheritance, ret)) {
		return ret;
	}
	return ret;
}

PackedStringArray TypeOperator::get_enum_constants(const Variant &p_type, const StringName &p_enum, bool p_no_inheritance) const {
	PackedStringArray ret;
	if (GDVIRTUAL_CALL(_get_enum_constants, p_type, p_enum, p_no_inheritance, ret)) {
		return ret;
	}
	return ret;
}

StringName TypeOperator::get_integer_constant_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance) const {
	StringName ret;
	if (GDVIRTUAL_CALL(_get_integer_constant_enum, p_type, p_name, p_no_inheritance, ret)) {
		return ret;
	}
	return ret;
}

bool TypeOperator::is_type_enabled(const Variant &p_type) const {
	bool ret;
	if (GDVIRTUAL_CALL(_is_type_enabled, p_type, ret)) {
		return ret;
	}
	return ret;
}

void TypeOperator::_bind_methods() {

	ClassDB::bind_method(D_METHOD("mark_handled"), &TypeOperator::mark_handled);
	ClassDB::bind_method(D_METHOD("is_handled"), &TypeOperator::is_handled);
	ClassDB::bind_method(D_METHOD("is_eager"), &TypeOperator::is_eager);
	ClassDB::bind_method(D_METHOD("get_result"), &TypeOperator::get_result);
	ClassDB::bind_method(D_METHOD("set_result", "value"), &TypeOperator::set_result);
	ClassDB::bind_method(D_METHOD("set_result", "value"), &TypeOperator::set_result);

	ADD_PROPERTY(PropertyInfo(Variant::NIL, "result"), "set_result", "get_result");

	GDVIRTUAL_BIND(_get_type_list, "no_named", "no_anonymous");
	GDVIRTUAL_BIND(_get_inheriters_from_type, "type");
	GDVIRTUAL_BIND(_get_parent_type, "type");
	GDVIRTUAL_BIND(_type_exists, "type");
	GDVIRTUAL_BIND(_is_parent_type, "type", "inherits");
	GDVIRTUAL_BIND(_can_instantiate, "type");
	GDVIRTUAL_BIND(_instantiate, "type");
	GDVIRTUAL_BIND(_has_signal, "type", "signal");
	GDVIRTUAL_BIND(_get_signal, "type", "signal");
	GDVIRTUAL_BIND(_get_type_signal_list, "type", "no_inheritance");
	GDVIRTUAL_BIND(_get_type_property_list, "type", "no_inheritance");
	GDVIRTUAL_BIND(_get_property, "source", "property");
	GDVIRTUAL_BIND(_set_property, "source", "property", "value");
	GDVIRTUAL_BIND(_has_method, "type", "method", "no_inheritance");
	GDVIRTUAL_BIND(_get_type_method_list, "type", "no_inheritance");
	GDVIRTUAL_BIND(_get_type_integer_constant_list, "type", "no_inheritance");
	GDVIRTUAL_BIND(_has_integer_constant, "type", "name");
	GDVIRTUAL_BIND(_get_integer_constant, "type", "name");
	GDVIRTUAL_BIND(_has_enum, "type", "name", "no_inheritance");
	GDVIRTUAL_BIND(_get_enum_list, "type", "no_inheritance");
	GDVIRTUAL_BIND(_get_enum_constants, "type", "enum", "no_inheritance");
	GDVIRTUAL_BIND(_get_integer_constant_enum, "type", "name", "no_inheritance");
	GDVIRTUAL_BIND(_is_type_enabled, "type");
}

/// TypeServer

TypeServer *TypeServer::singleton = nullptr;
TypeServer *(*TypeServer::create_func)() = nullptr;

void TypeServer::_process(OpContext *r_context, Handler p_handler) const {
	ERR_FAIL_COND(!r_context);
	for (int i = _operators.size()-1; i >= 0; i--) {
		TypeOperator *op = _operators[i];
		p_handler(*op, r_context);
		if (op->_handled) {
			r_context->handled = op->_handled;
			op->_handled = false;
		}
		if (r_context->eager && r_context->handled) {
			break;
		}
	}
}

PackedStringArray TypeServer::get_type_list(bool p_no_named, bool p_no_anonymous, bool p_first) const {
	TypeListOpContext context(p_no_named, p_no_anonymous, p_first, PackedStringArray());
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeListOpContext *ctx = (TypeListOpContext *)context;
		ctx->result.operator PackedStringArray().append_array(op.get_type_list(ctx->no_named, ctx->no_anonymous));
	});
	return context.result.operator PackedStringArray();
}

PackedStringArray TypeServer::get_inheriters_from_type(const Variant &p_type, bool p_eager) const {
	TypeOpContext context(p_type, p_eager, PackedStringArray());
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeOpContext *ctx = (TypeOpContext *)context;
		ctx->result.operator PackedStringArray().append_array(op.get_inheriters_from_type(ctx->type));
	});
	return context.result.operator PackedStringArray();
}

StringName TypeServer::get_parent_type(const Variant &p_type, bool p_eager) const {
	TypeOpContext context(p_type, p_eager, StringName());
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeOpContext *ctx = (TypeOpContext *)context;
		ctx->result = op.get_parent_type(ctx->type);
	});
	return context.result.operator StringName();
}

bool TypeServer::type_exists(const Variant &p_type, bool p_eager) const {
	TypeOpContext context(p_type, p_eager, false);
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeOpContext *ctx = (TypeOpContext *)context;
		ctx->result = ctx->result.operator bool() || op.type_exists(ctx->type);
	});
	return context.result.operator bool();
}

bool TypeServer::is_parent_type(const Variant &p_type, const Variant &p_inherits, bool p_eager) const {
	TypeInheritsOpContext context(p_type, p_inherits, p_eager, false);
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeInheritsOpContext *ctx = (TypeInheritsOpContext *)context;
		ctx->result = ctx->result.operator bool() || op.is_parent_type(ctx->type, ctx->inherits);
	});
	return context.result.operator bool();
}

bool TypeServer::can_instantiate(const Variant &p_type, bool p_eager) const {
	TypeOpContext context(p_type, p_eager, false);
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeOpContext *ctx = (TypeOpContext *)context;
		ctx->result = ctx->result.operator bool() || op.can_instantiate(ctx->type);
	});
	return context.result.operator bool();
}

Variant TypeServer::instantiate(const Variant &p_type, bool p_eager) const {
	TypeOpContext context(p_type, p_eager, Variant());
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeOpContext *ctx = (TypeOpContext *)context;
		ctx->result = op.instantiate(ctx->type);
	});
	return context.result;
}

bool TypeServer::has_signal(const Variant &p_type, StringName p_signal, bool p_eager) const {
	TypeMemberOpContext context(p_type, p_signal, p_eager, false);
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeMemberOpContext *ctx = (TypeMemberOpContext *)context;
		ctx->result = ctx->result.operator bool() || op.has_signal(ctx->type, ctx->name);
	});
	return context.result.operator bool();
}

Dictionary TypeServer::get_signal(const Variant &p_type, StringName p_signal, bool p_eager) const {
	TypeMemberOpContext context(p_type, p_signal, p_eager, Dictionary());
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeMemberOpContext *ctx = (TypeMemberOpContext *)context;
		ctx->result.operator Dictionary().merge(op.get_signal(ctx->type, ctx->name), true);
	});
	return context.result.operator Dictionary();
}

TypedArray<Dictionary> TypeServer::get_type_signal_list(const Variant &p_type, bool p_no_inheritance, bool p_eager) const {
	TypeQueryOpContext context(p_type, p_no_inheritance, p_eager, Array());
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeQueryOpContext *ctx = (TypeQueryOpContext *)context;
		ctx->result.operator Array().append_array(op.get_type_signal_list(ctx->type, ctx->no_inheritance));
	});
	return context.result.operator Array();
}

TypedArray<Dictionary> TypeServer::get_type_property_list(const Variant &p_type, bool p_no_inheritance, bool p_eager) const {
	TypeQueryOpContext context(p_type, p_no_inheritance, p_eager, TypedArray<Dictionary>());
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeQueryOpContext *ctx = (TypeQueryOpContext *)context;
		ctx->result.operator Array().append_array(op.get_type_property_list(ctx->type, ctx->no_inheritance));
	});
	return context.result.operator Array();
}

Variant TypeServer::get_property(const Variant &p_source, const StringName &p_property, bool p_eager) const {
	TypeMemberOpContext context(p_source, p_property, p_eager, Dictionary());
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeMemberOpContext *ctx = (TypeMemberOpContext *)context;
		ctx->result = op.get_property(ctx->type, ctx->name);
	});
	return context.result;
}

Error TypeServer::set_property(const Variant &p_source, const StringName &p_property, const Variant &p_value, bool p_eager) const {
	TypeMemberValueOpContext context(p_source, p_property, p_value, p_eager, OK);
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeMemberValueOpContext *ctx = (TypeMemberValueOpContext *)context;
		ctx->result = op.set_property(ctx->type, ctx->name, ctx->value);
	});
	return (Error)context.result.operator int();
}

bool TypeServer::has_method(const Variant &p_type, StringName p_method, bool p_no_inheritance, bool p_eager) const {
	TypeMemberOpContext context(p_type, p_method, p_eager, false);
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeMemberOpContext *ctx = (TypeMemberOpContext *)context;
		ctx->result = ctx->result.operator bool() || op.has_signal(ctx->type, ctx->name);
	});
	return context.result.operator bool();
}

TypedArray<Dictionary> TypeServer::get_type_method_list(const Variant &p_type, bool p_no_inheritance, bool p_eager) const {
	TypeQueryOpContext context(p_type, p_no_inheritance, p_eager, TypedArray<Dictionary>());
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeQueryOpContext *ctx = (TypeQueryOpContext *)context;
		ctx->result.operator Array().append_array(op.get_type_method_list(ctx->type, ctx->no_inheritance));
	});
	return context.result.operator Array();
}

PackedStringArray TypeServer::get_type_integer_constant_list(const Variant &p_type, bool p_no_inheritance, bool p_eager) const {
	TypeQueryOpContext context(p_type, p_no_inheritance, p_eager, PackedStringArray());
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeQueryOpContext *ctx = (TypeQueryOpContext *)context;
		ctx->result.operator PackedStringArray().append_array(op.get_type_integer_constant_list(ctx->type, ctx->no_inheritance));
	});
	return context.result.operator PackedStringArray();
}

bool TypeServer::has_integer_constant(const Variant &p_type, const StringName &p_name, bool p_eager) const {
	TypeMemberOpContext context(p_type, p_name, p_eager, false);
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeMemberOpContext *ctx = (TypeMemberOpContext *)context;
		ctx->result = ctx->result.operator bool() || op.has_integer_constant(ctx->type, ctx->name);
	});
	return context.result.operator bool();
}

int64_t TypeServer::get_integer_constant(const Variant &p_type, const StringName &p_name, bool p_eager) const {
	TypeMemberOpContext context(p_type, p_name, p_eager, Dictionary());
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeMemberOpContext *ctx = (TypeMemberOpContext *)context;
		ctx->result = op.get_integer_constant(ctx->type, ctx->name);
	});
	return context.result.operator signed int();
}

bool TypeServer::has_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance, bool p_eager) const {
	TypeMemberQueryOpContext context(p_type, p_name, p_no_inheritance, p_eager, false);
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeMemberQueryOpContext *ctx = (TypeMemberQueryOpContext *)context;
		ctx->result = ctx->result.operator bool() || op.has_enum(ctx->type, ctx->name, ctx->no_inheritance);
	});
	return context.result.operator bool();
}

PackedStringArray TypeServer::get_enum_list(const Variant &p_type, bool p_no_inheritance, bool p_eager) const {
	TypeQueryOpContext context(p_type, p_no_inheritance, p_eager, PackedStringArray());
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeQueryOpContext *ctx = (TypeQueryOpContext *)context;
		ctx->result.operator PackedStringArray().append_array(op.get_enum_list(ctx->type, ctx->no_inheritance));
	});
	return context.result.operator PackedStringArray();
}

PackedStringArray TypeServer::get_enum_constants(const Variant &p_type, const StringName &p_enum, bool p_no_inheritance, bool p_eager) const {
	TypeMemberQueryOpContext context(p_type, p_enum, p_no_inheritance, p_eager, PackedStringArray());
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeMemberQueryOpContext *ctx = (TypeMemberQueryOpContext *)context;
		ctx->result.operator PackedStringArray().append_array(op.get_enum_constants(ctx->type, ctx->name, ctx->no_inheritance));
	});
	return context.result.operator PackedStringArray();
}

StringName TypeServer::get_integer_constant_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance, bool p_eager) const {
	TypeMemberQueryOpContext context(p_type, p_name, p_no_inheritance, p_eager, StringName());
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeMemberQueryOpContext *ctx = (TypeMemberQueryOpContext *)context;
		ctx->result = op.get_integer_constant_enum(ctx->type, ctx->name, ctx->no_inheritance);
	});
	return context.result.operator StringName();
}

bool TypeServer::is_type_enabled(const Variant &p_type, bool p_eager) const {
	TypeOpContext context(p_type, p_eager, PackedStringArray());
	_process(&context, [](TypeOperator &op, OpContext *context) {
		TypeOpContext *ctx = (TypeOpContext *)context;
		ctx->result = ctx->result.operator bool() || op.is_type_enabled(ctx->type);
	});
	return context.result.operator bool();
}

TypeOperator* TypeServer::get_operator(const StringName &p_name) const {
	for (TypeOperator *op : _operators) {
		if (op->get_operator_name() == p_name) {
			return op;
		}
	}
	ERR_FAIL_V_MSG(nullptr, vformat("No %s with name '%s' has been registered.", SNAME("TypeOperator"), p_name));
}

void TypeServer::add_operator(TypeOperator * const p_operator) {
	ERR_FAIL_COND_MSG(!p_operator, vformat("Cannot add a null %s", SNAME("TypeOperator")));
	for (const TypeOperator *op : _operators) {
		ERR_FAIL_COND_MSG(op == p_operator, vformat("The given %s with name '%s' has already been registered.", SNAME("TypeOperator"), p_operator->get_operator_name()));
	}
	_operators.append(p_operator);
}

void TypeServer::remove_operator(const TypeOperator *p_operator) {
	ERR_FAIL_COND_MSG(!p_operator, vformat("Cannot remove a null %s", SNAME("TypeOperator")));
	for (int i = 0; i < _operators.size(); i++) {
		if (_operators[i] == p_operator) {
			_operators.remove_at(i);
		}
	}
	ERR_FAIL_MSG(vformat("The given %s with name '%s' is not present and could not be removed.", SNAME("TypeOperator"), p_operator->get_operator_name()));
}

#undef FIND_OP
