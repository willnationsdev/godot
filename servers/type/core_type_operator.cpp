
#include "core_type_operator.h"

PackedStringArray CoreTypeOperator::all_get_type_list(bool p_no_named, bool p_no_anonymous) const {
	if (p_no_named) {
		return PackedStringArray();
	}
	core_bind::special::ClassDB *db = Object::cast_to<core_bind::special::ClassDB>(Engine::get_singleton()->get_singleton_object(SNAME("ClassDB")));
	PackedStringArray arr = db->get_class_list();

	List<StringName> classes;
	ScriptServer::get_global_class_list(&classes);
	for (const StringName &name : classes) {
		arr.append(name);
	}
	return arr;
}
