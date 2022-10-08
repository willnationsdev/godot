/*************************************************************************/
/*  script_server_type_provider.h                                        */
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

#ifndef SCRIPT_SERVER_TYPE_PROVIDER_H
#define SCRIPT_SERVER_TYPE_PROVIDER_H

#include "servers/type_server.h"

#include "core/core_bind.h"

class ScriptServerTypeProvider : public TypeProvider {

	static const StringName &_extract_name(const Variant &p_type);

public:
	virtual StringName get_provider_name() const override { return SNAME("ClassDB"); }

	virtual PackedStringArray get_type_list(Dictionary p_state, bool p_no_named = false, bool p_no_anonymous = true) const override;
	virtual PackedStringArray get_inheriters_from_type(Dictionary p_query_state, const Variant &p_type) const override;
	virtual StringName get_parent_type(Dictionary p_query_state, const Variant &p_type) const override;
	virtual bool type_exists(Dictionary p_query_state, const Variant &p_type) const override;
	virtual bool is_parent_type(Dictionary p_query_state, const Variant &p_type, const Variant &p_inherits) const override;
	virtual bool can_instantiate(Dictionary p_query_state, const Variant &p_type) const override;
	virtual Variant instantiate(Dictionary p_query_state, const Variant &p_type) const override;
	virtual bool has_signal(Dictionary p_query_state, const Variant &p_type, StringName p_signal) const override;
	virtual Dictionary get_signal(Dictionary p_query_state, const Variant &p_type, StringName p_signal) const override;
	virtual TypedArray<Dictionary> get_type_signal_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance = false) const override;
	virtual TypedArray<Dictionary> get_type_property_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance = false) const override;
	virtual Variant get_property(Dictionary p_query_state, const Variant &p_source, const StringName &p_property) const override;
	virtual Error set_property(Dictionary p_query_state, const Variant &p_source, const StringName &p_property, const Variant &p_value) const override;
	virtual bool has_method(Dictionary p_query_state, const Variant &p_type, StringName p_method, bool p_no_inheritance = false) const override;
	virtual TypedArray<Dictionary> get_type_method_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance = false) const override;
	virtual PackedStringArray get_type_integer_constant_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance = false) const override;
	virtual bool has_integer_constant(Dictionary p_query_state, const Variant &p_type, const StringName &p_name) const override;
	virtual int64_t get_integer_constant(Dictionary p_query_state, const Variant &p_type, const StringName &p_name) const override;
	virtual bool has_enum(Dictionary p_query_state, const Variant &p_type, const StringName &p_name, bool p_no_inheritance = false) const override;
	virtual PackedStringArray get_enum_list(Dictionary p_query_state, const Variant &p_type, bool p_no_inheritance = false) const override;
	virtual PackedStringArray get_enum_constants(Dictionary p_query_state, const Variant &p_type, const StringName &p_enum, bool p_no_inheritance = false) const override;
	virtual StringName get_integer_constant_enum(Dictionary p_query_state, const Variant &p_type, const StringName &p_name, bool p_no_inheritance = false) const override;
	virtual bool is_type_enabled(Dictionary p_query_state, const Variant &p_type) const override;
};

#endif // SCRIPT_SERVER_TYPE_PROVIDER_H
