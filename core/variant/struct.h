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

class StructDB;
class Variant;
struct PropertyInfo;
struct StructPropertyInfo;

enum StructBucket : uint8_t {
	STRUCT_MINIMAL,
	STRUCT_SMALL,
	STRUCT_MEDIUM,
	STRUCT_LARGE
};

enum StructPropertyLength : uint8_t {
	STRUCT_PROP_LEN_NONE = 0,
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

class Struct {
private:
	// Duplicate of ObjData used by Variant for sizing.
	class MinimalStructSize {
		ObjectID a;
		Object *b;
	};

	friend StructDB;

public:
	static const int bucket_minimal = sizeof(MinimalStructSize) > (sizeof(real_t) * 4) ? sizeof(MinimalStructSize) : (sizeof(real_t) * 4);
	static const int bucket_small = sizeof(AABB);
	static const int bucket_medium = sizeof(Transform3D);
	static const int bucket_large = sizeof(Projection);
	static const int max_property_count = 6;

protected:
	struct StructPreamble {
		StructTypeId type_id : 14; // 16_383 possible types engine-wide
		StructBucket bucket : 2; // 4 bucket states
		StructPropertyId property_ids[max_property_count]{ 0, 0, 0, 0, 0, 0 }; // must include order of serialized properties to better recover from struct definition changes during deserialization.

		StructPreamble() :
				type_id(0), bucket(STRUCT_MINIMAL) {}

		_FORCE_INLINE_ bool operator==(const StructPreamble& p_other) const {
			return type_id == p_other.type_id;
		}
		_FORCE_INLINE_ bool operator!=(const StructPreamble& p_other) const {
			return !(operator==(p_other));
		}
	};

	StructPreamble preamble;

	_FORCE_INLINE_ void _get_property_list(List<StructPropertyInfo *> *r_list) const;

public:
	bool operator==(const Struct &p_struct) const;

	void assign(const Struct *p_other, Error &r_error);

	static _FORCE_INLINE_ int get_capacity(StructBucket p_bucket);
	_FORCE_INLINE_ int get_capacity() const;
	_FORCE_INLINE_ uint8_t *get_data();
	_FORCE_INLINE_ const uint8_t *get_data_const() const;

	_FORCE_INLINE_ StructTypeId get_type_id() const { return preamble.type_id; }
	_FORCE_INLINE_ StringName get_type_name() const;
	_FORCE_INLINE_ StructBucket get_bucket() const { return preamble.bucket; }

	_FORCE_INLINE_ void get_property_list(List<PropertyInfo> *r_list) const;
	_FORCE_INLINE_ void get_property_list(List<StructPropertyInfo> *r_list) const;
	_FORCE_INLINE_ bool has_method(const StringName &p_name) const;

	Variant callp(const StringName &p_method, const Variant **p_args, int p_argcount, Callable::CallError &r_error);
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
