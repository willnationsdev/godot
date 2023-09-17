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
#include "core/object/class_db.h"
#include "core/variant/variant.h"

struct StructPropertyInfo : public PropertyInfo {
	StructPropertyId id = 0;
	StructPropertyLength bytes = STRUCT_PROP_LEN_NONE;
	StructTypeId struct_type_id = 0; // only used when compared variant types are both STRUCT.
	Variant default_value;
	bool embed = false;

	StructPropertyInfo() {}
	StructPropertyInfo(const StructPropertyInfo& p_other)
		: PropertyInfo(p_other.type, p_other.name, p_other.hint, p_other.hint_string, p_other.usage, p_other.class_name),
		id(p_other.id), bytes(p_other.bytes), struct_type_id(p_other.struct_type_id), default_value(p_other.default_value), embed(p_other.embed) {}
	~StructPropertyInfo() {}
};

class StructDB {
private:
	friend Struct;

public:
	// TODO: Possibly refactor APIType to be outside of ClassDB to begin with.
	enum StructAPIType {
		STRUCT_API_CORE,
		STRUCT_API_EDITOR,
		STRUCT_API_EXTENSION,
		STRUCT_API_EDITOR_EXTENSION,
		STRUCT_API_NONE
	};

	struct PropertySetGet {
		StructPropertyId id;
		StringName setter;
		StringName getter;
		MethodBind *_setptr = nullptr;
		MethodBind *_getptr = nullptr;
		Variant::Type type;
	};

	struct StructTypeInfo {
		// Copied/Ripped from _VariantCall. May need to consider moving these classes to an independent file, perhaps in a namespace.
		struct ConstantData {
			HashMap<StringName, int64_t> value;
	#ifdef DEBUG_ENABLED
			List<StringName> value_ordered;
	#endif
			HashMap<StringName, Variant> variant_value;
	#ifdef DEBUG_ENABLED
			List<StringName> variant_value_ordered;
	#endif
		};
		struct EnumData {
			HashMap<StringName, HashMap<StringName, int>> value;
		};

		StructAPIType api = STRUCT_API_NONE;

		ObjectGDExtension *gdextension = nullptr;

		StructTypeId id = 0;
		StringName name;
		StructBucket bucket = STRUCT_MINIMAL;
		Variant script;

		HashMap<StringName, MethodBind *> method_map;
		HashMap<StringName, int64_t> constant_map;
		struct EnumInfo {
			List<StringName> constants;
			bool is_bitfield = false;
		};

		HashMap<StringName, EnumInfo> enum_map;
		List<StructPropertyInfo> property_list;
		HashMap<StructPropertyId, StructPropertyInfo> property_id_map;
		HashMap<StringName, StructPropertyInfo> property_name_map;
#ifdef DEBUG_METHODS_ENABLED
		List<StringName> constant_order;
		List<StringName> method_order;
		HashSet<StringName> methods_in_properties;
		HashMap<StringName, Vector<Error>> method_error_values;
		HashMap<StringName, List<StringName>> linked_properties;
#endif
		HashMap<StringName, PropertySetGet> property_setget;

		ConstantData constant_data;
		EnumData enum_data;

		HashSet<Struct *> instances; // todo

		int get_length() const;
		int get_variant_length() const;
		int get_capacity() const;
		const StructPropertyInfo &get_property_info(StructPropertyId p_id) const;
		const StructPropertyInfo &get_property_info(const StringName &p_name) const;
		void assign(Struct *p_self, const Struct *p_value);
		_FORCE_INLINE_ bool has_method(const StringName &p_name) const;

	private:
		friend StructDB;
		friend Struct;

		void _add_constant(StructTypeId p_type, StringName p_constant_name, int64_t p_constant_value);
		void _add_variant_constant(StructTypeId p_type, StringName p_constant_name, const Variant &p_constant_value);
		void _add_enum_constant(StructTypeId p_type, StringName p_enum_type_name, StringName p_enumeration_name, int p_enum_value);

		bool disabled = false;
		bool exposed = false;
		bool is_virtual = false;
		Struct *(*creation_func)() = nullptr;

		StructTypeInfo() {}
		~StructTypeInfo() {}
	};

	// Only really used to have more concise code everywhere else.
	struct StructInstanceInfo {
		Struct *ref; // TODO: surely there's a better way to simplify const-usability than by just making the properties mutable, no?
		StructTypeInfo *type;
		Struct::StructPreamble *preamble;
		uint8_t *data;
		_FORCE_INLINE_ bool is_valid() const { return ref && type && preamble && data; }

		StructInstanceInfo(Struct *p_ref = nullptr, StructTypeInfo *p_type = nullptr, Struct::StructPreamble *p_preamble = nullptr, uint8_t *p_data = nullptr) :
			ref(p_ref), type(p_type), preamble(p_preamble), data(p_data) {}
	};

	static uint8_t *get_data(Struct &p_struct);
	static const uint8_t *get_data_const(const Struct &p_struct);
	static void init_struct(Struct *p_struct);
	static void struct_assign(Struct *p_struct, const Struct *p_other, Error &r_error);
	static Error try_cast(StructInstanceInfo &p_info, StructInstanceInfo &p_other);

	static void struct_set_property(Struct *p_struct, StructPropertyId p_id, Variant p_value, Error &r_error);
	static Variant struct_get_property(Struct *p_struct, StructPropertyId p_id, Error &r_error);
	static Variant struct_get_property(const Struct *p_struct, StructPropertyId p_id, Error &r_error);
	static bool struct_has_method(const Struct *p_struct, const StringName &p_name);

	static void get_struct_type_names(Vector<StringName> &r_names);
	static _FORCE_INLINE_ const StringName &get_struct_type_name(StructTypeId p_id);
	static _FORCE_INLINE_ StructTypeId get_struct_type_id(const StringName &p_name);
	static void add_struct_type(const StructTypeInfo &p_info);
	static void remove_struct_type(StructTypeId p_id);
	static void remove_struct_type(const StringName &p_name);
	static StructBucket get_struct_type_bucket(StructTypeId p_id);

	static void initialize();
	static void finalize();

private:
	static RWLock _lock;
	static HashMap<StructTypeId, StructTypeInfo> _types;
	static HashMap<StringName, StructTypeId> _types_by_name;
	static RandomPCG _rand;

	static void _remove_struct_type(const StructTypeInfo *p_info, const char *p_lookup, String p_identifier);
	static _FORCE_INLINE_ StructTypeInfo *get_struct_type(StructTypeId p_id);
	static _FORCE_INLINE_ const StructTypeInfo *get_struct_type_const(const StructTypeId p_id);
	static StructTypeId _next_id();
	static StructBucket _evaluate_bucket_size(const StructTypeInfo &p_info);
	static StructPropertyId _evaluate_property_key(const StructTypeInfo &p_info, const StructPropertyInfo &p_property);
	static StructInstanceInfo &&_get_validated_struct_info(Struct *p_struct);
};

class StructBuffer {
private:
	Struct *_struct = nullptr;

protected:
	bool big_endian = false;
	int pointer = 0;

	uint8_t *_get_data();
	template <class T>
	void _put_ptr(const T *p_ptr);

public:
	virtual Error put_data(const uint8_t *p_data, int p_bytes);
	virtual Error get_data(uint8_t *p_buffer, int p_bytes);
	virtual Error get_partial_data(uint8_t *p_buffer, int p_bytes, int &r_received);
	virtual int get_available_bytes() const;

	/* helpers */
	void set_big_endian(bool p_big_endian);
	bool is_big_endian_enabled() const;

	void put_8(int8_t p_val);
	void put_u8(uint8_t p_val);
	void put_16(int16_t p_val);
	void put_u16(uint16_t p_val);
	void put_32(int32_t p_val);
	void put_u32(uint32_t p_val);
	void put_64(int64_t p_val);
	void put_u64(uint64_t p_val);
	void put_float(float p_val);
	void put_double(double p_val);
	void put_string(const String &p_string);
	void put_var(const Variant &p_variant, bool p_full_objects = false);
	Error put_value(const Variant &p_variant);

	uint8_t get_u8();
	int8_t get_8();
	uint16_t get_u16();
	int16_t get_16();
	uint32_t get_u32();
	int32_t get_32();
	uint64_t get_u64();
	int64_t get_64();
	float get_float();
	double get_double();
	String get_string();
	template <class T>
	T *get_ptr();
	Variant get_var(Variant::Type p_type, bool p_allow_objects = false);
	Variant get_value(Variant::Type p_type, Error &r_error);

	void seek(int p_pos);
	int get_size() const;
	int get_position() const;

	StructBuffer(Struct *p_struct) : _struct(p_struct) {}
	StructBuffer(const Struct *p_struct) : _struct(const_cast<Struct *>(p_struct)) {}
};

#endif // STRUCT_DB_H
