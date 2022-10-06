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

PackedStringArray TypeProvider::get_type_list(Dictionary p_query_state, bool p_no_named = false, bool p_no_anonymous = true) const {
	PackedStringArray ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_get_type_list, p_no_named, p_no_anonymous, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator PackedStringArray();
}

PackedStringArray TypeProvider::get_inheriters_from_type(Dictionary p_query_state, const Variant &p_type) const {
	PackedStringArray ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_get_inheriters_from_type, p_type, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator PackedStringArray();
}

StringName TypeProvider::get_parent_type(Dictionary p_query_state, const Variant &p_type) const {
	StringName ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_get_parent_type, p_type, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator StringName();
}

bool TypeProvider::type_exists(Dictionary p_query_state, const Variant &p_type) const {
	bool ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_type_exists, p_type, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator bool();
}

bool TypeProvider::is_parent_type(Dictionary p_query_state, const Variant &p_type, const Variant &p_inherits) const {
	bool ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_is_parent_type, p_type, p_inherits, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator bool();
}

bool TypeProvider::can_instantiate(Dictionary p_query_state, const Variant &p_type) const {
	bool ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_can_instantiate, p_type, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator bool();
}

Variant TypeProvider::instantiate(Dictionary p_query_state, const Variant &p_type) const {
	Variant ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_instantiate, p_type, ret)) {
		_set_result(p_query_state, ret);
	}
	return ret;
}

bool TypeProvider::has_signal(Dictionary p_query_state, const Variant &p_type, StringName p_signal) const {
	bool ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_has_signal, p_type, p_signal, ret)) {
		_set_result(p_query_state, ret);
	}
	return ret;
	return _get_result(p_query_state).operator bool();
}

Dictionary TypeProvider::get_signal(Dictionary p_query_state, const Variant &p_type, StringName p_signal) const {
	Dictionary ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_get_signal, p_type, p_signal, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator Dictionary();
}

TypedArray<Dictionary> TypeProvider::get_type_signal_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance) const {
	TypedArray<Dictionary> ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_get_type_signal_list, p_type, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator Array();
}

TypedArray<Dictionary> TypeProvider::get_type_property_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance) const {
	TypedArray<Dictionary> ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_get_type_property_list, p_type, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator Array();
}

Variant TypeProvider::get_property(Dictionary p_query_state, const Variant &p_source, const StringName &p_property) const {
	Variant ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_get_property, p_source, p_property, ret)) {
		_set_result(p_query_state, ret);
	}
	return ret;
}

Error TypeProvider::set_property(Dictionary p_query_state, const Variant &p_source, const StringName &p_property, const Variant &p_value) const {
	Error ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_set_property, p_source, p_property, p_value, ret)) {
		_set_result(p_query_state, ret);
	}
	return (Error)_get_result(p_query_state).operator uint64_t();
}

bool TypeProvider::has_method(Dictionary p_query_state, const Variant &p_type, StringName p_method, bool p_no_inheritance) const {
	bool ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_has_method, p_type, p_method, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator bool();
}

TypedArray<Dictionary> TypeProvider::get_type_method_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance) const {
	TypedArray<Dictionary> ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_get_type_method_list, p_type, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator Array();
}

PackedStringArray TypeProvider::get_type_integer_constant_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance) const {
	PackedStringArray ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_get_type_integer_constant_list, p_type, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator PackedStringArray();
}

bool TypeProvider::has_integer_constant(Dictionary p_query_state, const Variant &p_type, const StringName &p_name) const {
	bool ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_has_integer_constant, p_type, p_name, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator bool();
}

int64_t TypeProvider::get_integer_constant(Dictionary p_query_state, const Variant &p_type, const StringName &p_name) const {
	int64_t ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_get_integer_constant, p_type, p_name, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator int64_t();
}

bool TypeProvider::has_enum(Dictionary p_query_state, const Variant &p_type, const StringName &p_name, bool p_no_inheritance) const {
	bool ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_has_enum, p_type, p_name, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator bool();
}

PackedStringArray TypeProvider::get_enum_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance) const {
	PackedStringArray ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_get_enum_list, p_type, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator PackedStringArray();
}

PackedStringArray TypeProvider::get_enum_constants(Dictionary p_query_state, const Variant &p_type, const StringName &p_enum, bool p_no_inheritance) const {
	PackedStringArray ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_get_enum_constants, p_type, p_enum, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator PackedStringArray();
}

StringName TypeProvider::get_integer_constant_enum(Dictionary p_query_state, const Variant &p_type, const StringName &p_name, bool p_no_inheritance) const {
	StringName ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_get_integer_constant_enum, p_type, p_name, p_no_inheritance, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator StringName();
}

bool TypeProvider::is_type_enabled(Dictionary p_query_state, const Variant &p_type) const {
	bool ret;
	if (_can_participate(p_query_state) && GDVIRTUAL_CALL(_is_type_enabled, p_type, ret)) {
		_set_result(p_query_state, ret);
	}
	return _get_result(p_query_state).operator bool();
}

void TypeProvider::_bind_methods() {

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

#define TP_ITER(m_eager, m_cast, m_content)                    \
	RWLockRead rw(lock);                                       \
	Dictionary state = _new_query_state((m_eager));            \
	for (int i = _providers.size()-1; i >= 0; i--) {           \
		TypeProvider &p = *_providers[i];                      \
		TypeProvider::_set_result(state, m_content);           \
		if (TypeProvider::_can_participate(state)) {           \
			break;                                             \
		}                                                      \
	}                                                          \
	return TypeProvider::_get_result(state).operator m_cast();

template <class T, class... VarArgs>
T TypeServer::_process(bool p_eager, T (TypeServer::*handler)(Dictionary state, TypeProvider &p, VarArgs... args)) const {
	RWLockRead rw(lock);                                       
	Dictionary state = _new_query_state(p_eager);            
	for (int i = _providers.size()-1; i >= 0; i--) {           
		TypeProvider &p = *_providers[i];                      
		TypeProvider::_set_result(state, handler(state, p, args...));
		if (TypeProvider::_can_participate(state)) {           
			break;                                             
		}                                                      
	}                                                          
	return TypeProvider::_get_result(state).operator T();
}

PackedStringArray TypeServer::get_type_list(bool p_no_named, bool p_no_anonymous, bool p_eager) const {
	//TP_ITER(p_eager, PackedStringArray, p.get_type_list(state, p_no_named, p_no_anonymous));

	return _process<PackedStringArray, bool, bool, bool>(p_eager, [](Dictionary state, TypeProvider &p, bool no_named, bool no_anon, bool eager) {
		return p.get_type_list(state, no_named, no_anon);
	});

	// inline implementation
	RWLockRead rw(lock);                                       
	Dictionary state = _new_query_state((p_eager));            
	for (int i = _providers.size()-1; i >= 0; i--) {           
		TypeProvider &p = *_providers[i];                      
		TypeProvider::_set_result(state, p.get_type_list(state, p_no_named, p_no_anonymous));
		if (TypeProvider::_can_participate(state)) {           
			break;                                             
		}                                                      
	}                                                          
	return TypeProvider::_get_result(state).operator PackedStringArray();
}

PackedStringArray TypeServer::get_inheriters_from_type(const Variant &p_type, bool p_eager) const {
	TP_ITER(p_eager, PackedStringArray, p.get_inheriters_from_type(state, p_type));
}

StringName TypeServer::get_parent_type(const Variant &p_type, bool p_eager) const {
	TypeTypeDbContext context(p_type, p_eager, StringName());
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeTypeDbContext *ctx = (TypeTypeDbContext *)context;
		ctx->result = p.get_parent_type(ctx->type);
	});
	return context.result.operator StringName();
}

bool TypeServer::type_exists(const Variant &p_type, bool p_eager) const {
	TypeTypeDbContext context(p_type, p_eager, false);
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeTypeDbContext *ctx = (TypeTypeDbContext *)context;
		ctx->result = ctx->result.operator bool() || p.type_exists(ctx->type);
	});
	return context.result.operator bool();
}

bool TypeServer::is_parent_type(const Variant &p_type, const Variant &p_inherits, bool p_eager) const {
	TypeInheritsTypeDbContext context(p_type, p_inherits, p_eager, false);
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeInheritsTypeDbContext *ctx = (TypeInheritsTypeDbContext *)context;
		ctx->result = ctx->result.operator bool() || p.is_parent_type(ctx->type, ctx->inherits);
	});
	return context.result.operator bool();
}

bool TypeServer::can_instantiate(const Variant &p_type, bool p_eager) const {
	TypeTypeDbContext context(p_type, p_eager, false);
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeTypeDbContext *ctx = (TypeTypeDbContext *)context;
		ctx->result = ctx->result.operator bool() || p.can_instantiate(ctx->type);
	});
	return context.result.operator bool();
}

Variant TypeServer::instantiate(const Variant &p_type, bool p_eager) const {
	TypeTypeDbContext context(p_type, p_eager, Variant());
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeTypeDbContext *ctx = (TypeTypeDbContext *)context;
		ctx->result = p.instantiate(ctx->type);
	});
	return context.result;
}

bool TypeServer::has_signal(const Variant &p_type, StringName p_signal, bool p_eager) const {
	TypeMemberTypeDbContext context(p_type, p_signal, p_eager, false);
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeMemberTypeDbContext *ctx = (TypeMemberTypeDbContext *)context;
		ctx->result = ctx->result.operator bool() || p.has_signal(ctx->type, ctx->name);
	});
	return context.result.operator bool();
}

Dictionary TypeServer::get_signal(const Variant &p_type, StringName p_signal, bool p_eager) const {
	TypeMemberTypeDbContext context(p_type, p_signal, p_eager, Dictionary());
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeMemberTypeDbContext *ctx = (TypeMemberTypeDbContext *)context;
		ctx->result.operator Dictionary().merge(p.get_signal(ctx->type, ctx->name), true);
	});
	return context.result.operator Dictionary();
}

TypedArray<Dictionary> TypeServer::get_type_signal_list(const Variant &p_type, bool p_no_inheritance, bool p_eager) const {
	TypeQueryTypeDbContext context(p_type, p_no_inheritance, p_eager, TypedArray<Dictionary>());
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeQueryTypeDbContext *ctx = (TypeQueryTypeDbContext *)context;
		ctx->result.operator Array().append_array(p.get_type_signal_list(ctx->type, ctx->no_inheritance));
	});
	return context.result.operator Array();
}

TypedArray<Dictionary> TypeServer::get_type_property_list(const Variant &p_type, bool p_no_inheritance, bool p_eager) const {
	TypeQueryTypeDbContext context(p_type, p_no_inheritance, p_eager, TypedArray<Dictionary>());
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeQueryTypeDbContext *ctx = (TypeQueryTypeDbContext *)context;
		ctx->result.operator Array().append_array(p.get_type_property_list(ctx->type, ctx->no_inheritance));
	});
	return context.result.operator Array();
}

Variant TypeServer::get_property(const Variant &p_source, const StringName &p_property, bool p_eager) const {
	TypeMemberTypeDbContext context(p_source, p_property, p_eager, Variant());
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeMemberTypeDbContext *ctx = (TypeMemberTypeDbContext *)context;
		ctx->result = p.get_property(ctx->type, ctx->name);
	});
	return context.result;
}

Error TypeServer::set_property(const Variant &p_source, const StringName &p_property, const Variant &p_value, bool p_eager) const {
	TypeMemberValueTypeDbContext context(p_source, p_property, p_value, p_eager, OK);
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeMemberValueTypeDbContext *ctx = (TypeMemberValueTypeDbContext *)context;
		ctx->result = p.set_property(ctx->type, ctx->name, ctx->value);
	});
	return (Error)context.result.operator int();
}

bool TypeServer::has_method(const Variant &p_type, StringName p_method, bool p_no_inheritance, bool p_eager) const {
	TypeMemberQueryTypeDbContext context(p_type, p_method, p_no_inheritance, p_eager, false);
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeMemberQueryTypeDbContext *ctx = (TypeMemberQueryTypeDbContext *)context;
		ctx->result = ctx->result.operator bool() || p.has_method(ctx->type, ctx->name, ctx->no_inheritance);
	});
	return context.result.operator bool();
}

TypedArray<Dictionary> TypeServer::get_type_method_list(const Variant &p_type, bool p_no_inheritance, bool p_eager) const {
	TypeQueryTypeDbContext context(p_type, p_no_inheritance, p_eager, TypedArray<Dictionary>());
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeQueryTypeDbContext *ctx = (TypeQueryTypeDbContext *)context;
		ctx->result.operator Array().append_array(p.get_type_method_list(ctx->type, ctx->no_inheritance));
	});
	return context.result.operator Array();
}

PackedStringArray TypeServer::get_type_integer_constant_list(const Variant &p_type, bool p_no_inheritance, bool p_eager) const {
	TypeQueryTypeDbContext context(p_type, p_no_inheritance, p_eager, PackedStringArray());
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeQueryTypeDbContext *ctx = (TypeQueryTypeDbContext *)context;
		ctx->result.operator PackedStringArray().append_array(p.get_type_integer_constant_list(ctx->type, ctx->no_inheritance));
	});
	return context.result.operator PackedStringArray();
}

bool TypeServer::has_integer_constant(const Variant &p_type, const StringName &p_name, bool p_eager) const {
	TypeMemberTypeDbContext context(p_type, p_name, p_eager, false);
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeMemberTypeDbContext *ctx = (TypeMemberTypeDbContext *)context;
		ctx->result = ctx->result.operator bool() || p.has_integer_constant(ctx->type, ctx->name);
	});
	return context.result.operator bool();
}

int64_t TypeServer::get_integer_constant(const Variant &p_type, const StringName &p_name, bool p_eager) const {
	TypeMemberTypeDbContext context(p_type, p_name, p_eager, 0);
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeMemberTypeDbContext *ctx = (TypeMemberTypeDbContext *)context;
		ctx->result = p.get_integer_constant(ctx->type, ctx->name);
	});
	return context.result.operator int64_t();
}

bool TypeServer::has_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance, bool p_eager) const {
	TypeMemberQueryTypeDbContext context(p_type, p_name, p_no_inheritance, p_eager, false);
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeMemberQueryTypeDbContext *ctx = (TypeMemberQueryTypeDbContext *)context;
		ctx->result = ctx->result.operator bool() || p.has_enum(ctx->type, ctx->name, ctx->no_inheritance);
	});
	return context.result.operator bool();
}

PackedStringArray TypeServer::get_enum_list(const Variant &p_type, bool p_no_inheritance, bool p_eager) const {
	TypeQueryTypeDbContext context(p_type, p_no_inheritance, p_eager, PackedStringArray());
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeQueryTypeDbContext *ctx = (TypeQueryTypeDbContext *)context;
		ctx->result.operator PackedStringArray().append_array(p.get_enum_list(ctx->type, ctx->no_inheritance));
	});
	return context.result.operator PackedStringArray();
}

PackedStringArray TypeServer::get_enum_constants(const Variant &p_type, const StringName &p_enum, bool p_no_inheritance, bool p_eager) const {
	TypeMemberQueryTypeDbContext context(p_type, p_enum, p_no_inheritance, p_eager, PackedStringArray());
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeMemberQueryTypeDbContext *ctx = (TypeMemberQueryTypeDbContext *)context;
		ctx->result.operator PackedStringArray().append_array(p.get_enum_constants(ctx->type, ctx->name, ctx->no_inheritance));
	});
	return context.result.operator PackedStringArray();
}

StringName TypeServer::get_integer_constant_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance, bool p_eager) const {
	TypeMemberQueryTypeDbContext context(p_type, p_name, p_no_inheritance, p_eager, StringName());
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeMemberQueryTypeDbContext *ctx = (TypeMemberQueryTypeDbContext *)context;
		ctx->result = p.get_integer_constant_enum(ctx->type, ctx->name, ctx->no_inheritance);
	});
	return context.result.operator StringName();
}

bool TypeServer::is_type_enabled(const Variant &p_type, bool p_eager) const {
	TypeTypeDbContext context(p_type, p_eager, PackedStringArray());
	_process(&context, [](TypeProvider &p, TypeDbContext *context) {
		TypeTypeDbContext *ctx = (TypeTypeDbContext *)context;
		ctx->result = ctx->result.operator bool() || p.is_type_enabled(ctx->type);
	});
	return context.result.operator bool();
}

TypeProvider* TypeServer::get_provider(const StringName &p_name) const {
	TYPE_RLOCK
	for (TypeProvider *op : _providers) {
		if (op->get_provider_name() == p_name) {
			return op;
		}
	}
	ERR_FAIL_V_MSG(nullptr, vformat("No %s with name '%s' has been registered.", SNAME("TypeProvider"), p_name));
}

void TypeServer::add_provider(TypeProvider * const p_provider) {
	TYPE_WLOCK
	ERR_FAIL_COND_MSG(!p_provider, vformat("Cannot add a null %s", SNAME("TypeProvider")));
	for (const TypeProvider *op : _providers) {
		ERR_FAIL_COND_MSG(op == p_provider, vformat("The given %s with name '%s' has already been registered.", SNAME("TypeProvider"), p_provider->get_provider_name()));
	}
	_providers.append(p_provider);
}

void TypeServer::remove_provider(const TypeProvider *p_provider) {
	TYPE_WLOCK
	ERR_FAIL_COND_MSG(!p_provider, vformat("Cannot remove a null %s", SNAME("TypeProvider")));
	for (int i = 0; i < _providers.size(); i++) {
		if (_providers[i] == p_provider) {
			_providers.remove_at(i);
		}
	}
	ERR_FAIL_MSG(vformat("The given %s with name '%s' is not present and could not be removed.", SNAME("TypeProvider"), p_provider->get_provider_name()));
}

#undef TYPE_RLOCK
#undef TYPE_WLOCK

