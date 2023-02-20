/**************************************************************************/
/*  struct.h                                                              */
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

#ifndef STRUCT_DB_H
#define STRUCT_DB_H

#include "core/templates/hash_map.h"
#include "core/templates/hash_set.h"
#include "core/templates/vector.h"
#include "core/object/object.h"
#include "core/variant/variant.h"

enum StructPropertyLength : uint8_t {
	STRUCT_PROP_LEN_1 = 1,
	STRUCT_PROP_LEN_2 = 2,
	STRUCT_PROP_LEN_4 = 4,
	STRUCT_PROP_LEN_8 = 8,
	STRUCT_PROP_LEN_16 = 16,
	STRUCT_PROP_LEN_32 = 32,
	STRUCT_PROP_LEN_64 = 64,
	STRUCT_PROP_LEN_REAL = sizeof(real_t),
	STRUCT_PROP_LEN_PTR = sizeof(Object*),
};

struct StructPropertyInfo : public PropertyInfo {
	struct_property_t id;
	StructPropertyLength bytes;
	Variant default_value;
	bool embed;
};

class StructDB {
private:
	friend Struct;

protected:
	static uint8_t *get_data(Struct &p_struct);
	static const uint8_t *get_data_const(const Struct &p_struct);
	template <class TParser>
	static void init_struct(Struct *p_struct);
	template <class TParser>
	static void clear_struct(Struct *p_struct);
	template <class TParser>
	static void set_struct_property(Struct *p_struct, struct_property_t p_id, Variant p_value, Error &r_error);
	template <class TParser>
	static Variant get_struct_property(Struct *p_struct, struct_property_t p_id);

	static const char _fallback_property_keys[];

public:
	static void get_struct_type_names(Vector<StringName> &r_names);
	static void add_struct_type(const StructTypeInfo &p_info);
	static void remove_struct_type(struct_type_t p_id);
	static void remove_struct_type(const StringName &p_name);

private:
	static RWLock _lock;
	static HashMap<struct_type_t, StructTypeInfo> _types;
	static HashMap<StringName, struct_type_t> _name_map;
	static RandomPCG _rand;

	static _FORCE_INLINE_ StructTypeInfo *_get_struct_type_ptr(struct_type_t p_id);
	static _FORCE_INLINE_ StructTypeInfo *_get_struct_type_ptr(const StringName &p_name);
	static _FORCE_INLINE_ void _remove_struct_type(const StructTypeInfo *p_info, const char *p_lookup, String p_identifier);
	static struct_type_t _next_id();
	static StructBucket _evaluate_bucket_size(const StructTypeInfo &p_info);
	static struct_property_t _evaluate_property_key(const StructTypeInfo &p_info, const StructPropertyInfo &p_property);
	static bool _validate_struct(Struct *p_struct, StructTypeInfo *r_type, uint8_t *r_data);
};

class StructDefaultParser {
public:
	void init(const StructTypeInfo &p_info, const StructPreamble &p_preamble, uint8_t *p_data);
	void clear(const StructTypeInfo &p_info, const StructPreamble &p_preamble, uint8_t *p_data);
	void set_property(const StructTypeInfo &p_info, const StructPreamble &p_preamble, uint8_t *p_data, struct_property_t p_id, Variant p_value, Error &r_error);
	Variant get_property(const StructTypeInfo &p_info, const StructPreamble &p_preamble, uint8_t *p_data, struct_property_t p_id);
};

#endif // STRUCT_DB_H
