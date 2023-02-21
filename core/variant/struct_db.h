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
	StructPropertyId id;
	StructPropertyLength bytes;
	StructTypeId struct_type_id; // only used when compared variant types are both STRUCT.
	Variant default_value;
	bool embed;
};

struct StructInstanceInfo {
	Struct *const ref;
	StructTypeInfo *const type;
	mutable StructPreamble *preamble;
	mutable uint8_t *data;
	_FORCE_INLINE_ bool is_valid() { return ref && type && preamble && data; }

	StructInstanceInfo(Struct* p_ref = nullptr, StructTypeInfo* p_type = nullptr, StructPreamble* p_preamble = nullptr, uint8_t* p_data = nullptr) :
		ref(p_ref), type(p_type), preamble(p_preamble), data(p_data) {}
};

class StructParser;

class StructDB {
private:
	friend Struct;

	static StructParser *struct_parser;

	static RWLock _lock;
	static HashMap<StructTypeId, StructTypeInfo> _types;
	static HashMap<StringName, StructTypeId> _name_map;
	static RandomPCG _rand;

protected:
	static const char _fallback_property_keys[];

public:
	static uint8_t *get_data(Struct &p_struct);
	static const uint8_t *get_data_const(const Struct &p_struct);
	static void init_struct(Struct *p_struct);
	static void clear_struct(Struct *p_struct);
	static void assign_struct(Struct *p_struct, Struct *p_other, Error &r_error);
	static void set_struct_property(Struct *p_struct, StructPropertyId p_id, Variant p_value, Error &r_error);
	static Variant get_struct_property(Struct *p_struct, StructPropertyId p_id);

	static void get_struct_type_names(Vector<StringName> &r_names);
	static _FORCE_INLINE_ const StringName &get_struct_type_name(StructTypeId p_id);
	static _FORCE_INLINE_ StructTypeId get_struct_type_id(const StringName &p_name);
	static void add_struct_type(const StructTypeInfo &p_info);
	static void remove_struct_type(StructTypeId p_id);
	static void remove_struct_type(const StringName &p_name);
	static StructBucket get_struct_type_bucket(StructTypeId p_id);

	static _FORCE_INLINE_ void set_struct_parser(StructParser *p_parser);
	static _FORCE_INLINE_ StructParser *get_struct_parser();

	static void initialize();
	static void finalize();

private:
	static _FORCE_INLINE_ void _remove_struct_type(const StructTypeInfo *p_info, const char *p_lookup, String p_identifier);
	static StructTypeId _next_id();
	static StructBucket _evaluate_bucket_size(const StructTypeInfo &p_info);
	static StructPropertyId _evaluate_property_key(const StructTypeInfo &p_info, const StructPropertyInfo &p_property);
	static StructInstanceInfo &&_get_validated_struct_info(Struct *p_struct);
};

class StructParser {
public:
	virtual void init(StructInstanceInfo &p_info) {}
	virtual void clear(StructInstanceInfo &p_info) {}
	virtual void assign(StructInstanceInfo &p_info, StructInstanceInfo &p_other, Error &r_error) {}
	virtual int try_cast(StructInstanceInfo &p_info, StructInstanceInfo &p_other, Error &r_error) { return true; }
	virtual void set_property(StructInstanceInfo &p_instance, StructPropertyId p_id, Variant p_value, Error &r_error) {}
	virtual Variant get_property(const StructInstanceInfo &p_instance, StructPropertyId p_id) { return Variant(); }
};

class DefaultStructParser : public StructParser {
	virtual void init(StructInstanceInfo &p_info) override;
	virtual void clear(StructInstanceInfo &p_info) override;
	virtual void assign(StructInstanceInfo &p_info, StructInstanceInfo &p_other, Error &r_error) override;
	virtual int try_cast(StructInstanceInfo &p_info, StructInstanceInfo &p_other, Error &r_error) override;
	virtual void set_property(StructInstanceInfo &p_instance, StructPropertyId p_id, Variant p_value, Error &r_error) override;
	virtual Variant get_property(const StructInstanceInfo &p_instance, StructPropertyId p_id) override;
};

#endif // STRUCT_DB_H
