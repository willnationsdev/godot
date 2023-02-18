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

#include "core/math/aabb.h"
#include "core/math/projection.h"
#include "core/math/transform_3d.h"
#include "core/object/object_id.h"

class Object;
typedef StructID;

struct StructValueInfo {
	StructID type_id : 14;
	uint8_t bucket : 2;
};

class Struct {
	StructValueInfo _value_info;

public:
	_FORCE_INLINE_ StructID get_type_id() { return _value_info.type_id; }
	_FORCE_INLINE_ uint8_t get_bucket() { return _value_info.bucket; }

	bool operator==(const Struct &p_struct) const;
	virtual uint8_t *get_data() = 0;

	struct MinimalSize {
		ObjectID a;
		Object *b;
	};

	static const int bucket_minimal = sizeof(MinimalSize) > (sizeof(real_t) * 4) ? sizeof(MinimalSize) : (sizeof(real_t) * 4);
	static const int bucket_small = sizeof(AABB);
	static const int bucket_medium = sizeof(Transform3D);
	static const int bucket_large = sizeof(Projection);
};

class StructMinimal : public Struct {
	uint8_t data[bucket_minimal]{ 0 };

public:
	virtual uint8_t *get_data() override { return data; }
};

class StructSmall : public Struct {
	uint8_t data[bucket_small]{ 0 };

public:
	virtual uint8_t *get_data() override { return data; }
};

class StructMedium : public Struct {
	uint8_t data[bucket_medium]{ 0 };

public:
	virtual uint8_t *get_data() override { return data; }
};

class StructLarge : public Struct {
	uint8_t data[bucket_large]{ 0 };

public:
	virtual uint8_t *get_data() override { return data; }
};

#endif // STRUCT_H
