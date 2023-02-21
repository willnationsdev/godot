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

#ifndef STRUCT_H
#define STRUCT_H

#include "core/templates/hash_map.h"
#include "core/templates/hash_set.h"
#include "core/math/aabb.h"
#include "core/math/projection.h"
#include "core/math/transform_3d.h"
#include "core/object/object_id.h"
#include "core/variant/callable.h"

typedef uint16_t StructTypeId;
typedef uint8_t StructPropertyId;

class Struct;
class Variant;
struct StructPropertyInfo;

#define STRUCT_MAX_PROPERTY_COUNT 6

enum StructBucket : uint8_t {
	STRUCT_MINIMAL,
	STRUCT_SMALL,
	STRUCT_MEDIUM,
	STRUCT_LARGE
};

struct StructPreamble {
	StructTypeId type_id : 14; // 16_383 possible types engine-wide
	StructBucket bucket : 2; // 4 bucket states
	StructPropertyId property_ids[STRUCT_MAX_PROPERTY_COUNT]; // must include order of serialized properties to better recover from struct definition changes during deserialization.

	_FORCE_INLINE_ bool operator==(const StructPreamble& p_other) const {
		return type_id == p_other.type_id;
	}
	_FORCE_INLINE_ bool operator!=(const StructPreamble& p_other) const {
		return !(operator==(p_other));
	}
};

struct StructTypeInfo {
	StructTypeId id;
	StringName name;
	StructBucket bucket;
	StringName script_class;
	Callable constructor;
	Callable destructor;
	HashMap<StructPropertyId, StructPropertyInfo> properties;
	HashMap<StringName, StructPropertyInfo *> property_name_map;
	HashMap<int, Callable> operators;
	HashMap<StringName, Callable> methods;
	HashSet<Struct *> instances; // todo

	int get_length() const;
	int get_data_length() const;
	int get_capacity() const;
	Variant get_script() const;
	void assign(Struct *p_self, const Struct *p_value);
};

class Struct {
private:
	// Duplicate of ObjData used by Variant for sizing.
	class MinimalStructSize {
		ObjectID a;
		Object *b;
	};

public:
	StructPreamble preamble;

	bool operator==(const Struct &p_struct) const;

	_FORCE_INLINE_ uint8_t *get_data();
	_FORCE_INLINE_ const uint8_t *get_data_const() const;
	_FORCE_INLINE_ const StructTypeInfo *get_type() const;

	static const int bucket_minimal = sizeof(MinimalStructSize) > (sizeof(real_t) * 4) ? sizeof(MinimalStructSize) : (sizeof(real_t) * 4);
	static const int bucket_small = sizeof(AABB);
	static const int bucket_medium = sizeof(Transform3D);
	static const int bucket_large = sizeof(Projection);
};

class StructMinimal : public Struct {
	uint8_t data[(bucket_minimal - sizeof(StructPreamble)) / sizeof(uint8_t)]{ 0 };

public:
	static _FORCE_INLINE_ uint8_t *get_data(StructMinimal p_value) { return p_value.data; }
};

class StructSmall : public Struct {
	uint8_t data[(bucket_small - sizeof(StructPreamble)) / sizeof(uint8_t)]{ 0 };

public:
	static _FORCE_INLINE_ uint8_t *get_data(StructSmall p_value) { return p_value.data; }
};

class StructMedium : public Struct {
	uint8_t data[(bucket_medium - sizeof(StructPreamble)) / sizeof(uint8_t)]{ 0 };

public:
	static _FORCE_INLINE_ uint8_t *get_data(StructMedium p_value) { return p_value.data; }
};

class StructLarge : public Struct {
	uint8_t data[(bucket_large - sizeof(StructPreamble)) / sizeof(uint8_t)]{ 0 };

public:
	static _FORCE_INLINE_ uint8_t *get_data(StructLarge p_value) { return p_value.data; }
};

#endif // STRUCT_H
