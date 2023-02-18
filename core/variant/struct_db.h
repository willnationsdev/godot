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

#include <core/templates/hash_map.h>
#include <core/templates/vector.h>
#include <core/object/object.h>
#include <core/variant/variant.h>

enum Operator;

typedef unsigned char StructPropertyID;

struct StructPropertyInfo : public PropertyInfo {
	StructPropertyID id;
	uint8_t bytes;
};

class StructDB {
public:
	struct StructTypeInfo {
		StructID id;
		StringName name;
		uint8_t bucket_size;
		HashMap<StructPropertyID, StructPropertyInfo> properties;
		HashMap<Operator, Callable> operators;
		HashMap<StringName, Callable> methods;
	};

	static void get_struct_type_names(Vector<StringName> &r_names);
	static bool try_get_struct_type(StructID p_id, StructTypeInfo *r_info);
	static bool try_get_struct_type(const StringName &p_name, StructTypeInfo *r_info);
	static void add_struct_type(const StructTypeInfo &p_info);
	static void remove_struct_type(StructID p_id);

private:
	static RWLock _lock;
	static HashMap<StructID, StructTypeInfo> _types;
	static HashMap<StringName, StructID> _name_map;
	static RandomPCG _rand;

	static StructID _next_id();
	static StructPropertyID _next_property_id(StructID p_type_id);
	static uint8_t _evaluate_bucket_size(StructTypeInfo &info);
};

#endif // STRUCT_DB_H
