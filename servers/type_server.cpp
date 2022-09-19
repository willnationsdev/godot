#include "servers/type_server.h"
#include "core/core_bind.h"

#define ERR_NO_BEST_FIT_MSG "A best fit type operator could not be determined."

StringName TypeServer::get_operator_name() const {
	return SNAME("TypeServer");
}

int TypeServer::has_priority(const Variant &p_type, TypeOpKind p_op_kind) const {
	return INT_MAX;
}

PackedStringArray TypeServer::get_type_list(bool p_no_named = false, bool p_no_anonymous = true) const {
	PackedStringArray ret;
	for (const TypeOperator *op : _operators) {
		ret.append_array(op->get_type_list(p_no_named, p_no_anonymous));
	}
	return ret;
}

PackedStringArray TypeServer::get_inheriters_from_type(const Variant &p_type) const {
	PackedStringArray ret;
	Vector<TypeOperator *> ops = filter_operators(p_type, TYPE_OP_KIND_QUERY_INHERITANCE, TYPE_OP_FILTER_SUPPORTS);
	for (const TypeOperator *op : ops) {
		ret.append_array(op->get_inheriters_from_type(p_type));
	}
	return ret;
}

StringName TypeServer::get_parent_type(const Variant &p_type) const {
	TypeOperator *optr = find_best_fit_operator(p_type, TYPE_OP_KIND_QUERY_INHERITANCE);
	ERR_FAIL_COND_V_MSG(!optr, StringName(), ERR_NO_BEST_FIT_MSG);
	return optr->get_parent_type(p_type);
}

bool TypeServer::type_exists(const Variant &p_type) const {
	Vector<TypeOperator *> ops = filter_operators(p_type, TYPE_OP_KIND_QUERY_STATUS, TYPE_OP_FILTER_SUPPORTS);
	for (const TypeOperator *op : ops) {
		if (op->type_exists(p_type)) {
			return true;
		}
	}
	return false;
}

bool TypeServer::is_parent_type(const Variant &p_type, const Variant &p_inherits) const {
	Vector<TypeOperator *> ops = filter_operators(p_type, TYPE_OP_KIND_QUERY_INHERITANCE, TYPE_OP_FILTER_SUPPORTS);
	for (const TypeOperator *op : ops) {
		if (op->type_exists(p_type)) {
			return true;
		}
	}
	return false;
}

bool TypeServer::can_instantiate(const Variant &p_type) const {
	Vector<TypeOperator *> ops = filter_operators(p_type, TYPE_OP_KIND_QUERY_STATUS, TYPE_OP_FILTER_SUPPORTS);
	for (const TypeOperator *op : ops) {
		if (op->can_instantiate(p_type)) {
			return true;
		}
	}
	return false;
}

Variant TypeServer::instantiate(const Variant &p_type) const {
	TypeOperator *optr = find_best_fit_operator(p_type, TYPE_OP_KIND_INSTANTIATE);
	ERR_FAIL_COND_V_MSG(!optr, Variant(), ERR_NO_BEST_FIT_MSG);
	return optr->instantiate(p_type);
}

bool TypeServer::has_signal(const Variant &p_type, StringName p_signal) const {
	
}
Dictionary TypeServer::get_signal(const Variant &p_type, StringName p_signal) const {
	
}
TypedArray<Dictionary> TypeServer::get_signal_list(const Variant &p_type, bool p_no_inheritance) const {
	
}

TypedArray<Dictionary> TypeServer::get_property_list(const Variant &p_type, bool p_no_inheritance) const {
	
}
Variant TypeServer::get_property(const Variant &p_source, const StringName &p_property) const {
	
}
Error TypeServer::set_property(const Variant &p_source, const StringName &p_property, const Variant &p_value) const {
	
}

bool TypeServer::has_method(const Variant &p_type, StringName p_method, bool p_no_inheritance) const {
	
}

TypedArray<Dictionary> TypeServer::get_method_list(const Variant &p_type, bool p_no_inheritance) const {
	
}

PackedStringArray TypeServer::get_integer_constant_list(const Variant &p_type, bool p_no_inheritance) const {
	
}
bool TypeServer::has_integer_constant(const Variant &p_type, const StringName &p_name) const {
	
}
int64_t TypeServer::get_integer_constant(const Variant &p_type, const StringName &p_name) const {
	
}

bool TypeServer::has_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance) const {
	
}
PackedStringArray TypeServer::get_enum_list(const Variant &p_type, bool p_no_inheritance) const {
	
}
PackedStringArray TypeServer::get_enum_constants(const Variant &p_type, const StringName &p_enum, bool p_no_inheritance) const {
	
}
StringName TypeServer::get_integer_constant_enum(const Variant &p_type, const StringName &p_name, bool p_no_inheritance) const {
	
}

bool TypeServer::is_type_enabled(const Variant &p_type) const {
	
}

const Vector<TypeOperator *> &TypeServer::filter_operators(const Variant& p_type, TypeOpKind p_op_kind, TypeOpFilter p_op_filter) const {
	Vector<TypeOperator *> ops;
	ERR_FAIL_COND_V(_operators.is_empty(), ops);
	ops.resize(_operators.size());
	ops.fill(nullptr);
	int idx = 0;
	TypeOperator *parent = _operators[0]; // start at first/root.
	ops.write[idx++] = parent;
	int max = parent->has_priority(p_type, p_op_kind);

	// TODO: Finish rewriting this into a sort of tree-like structure.
	// 1. To be considered, `has_priority` must return a positive number.
	// 2. Of those, break it down to those which exist at the deepest levels of the tree (with the longest chain of parents).
	// 3. Of those, order them by their priority level in descending order and insert them into the `ops` array.
	// This then ensures that the returned array contains only those operators that have the highest level of specificity for handling the provided type and kind of operation
	// with the first element being the one most suitable for processing the request if it is handled by a single operator.

	// Also....looking at all this.....it seems like this stuff is becoming excessively complicated.
	// I should probably just go with whatever is gonna be simplest *first*, and
	// then add in whatever complexity is needed for actionable & verified, missing requirements.

	for (int i = 1; i < _operators.size(); i++) {
		TypeOperator *op = _operators[i];

		int priority = op->has_priority(p_type, p_op_kind);
		if (priority > max) {
			max = priority;
			idx = 0;
			ops.fill(nullptr);
			ops.write[idx++] = op;
		} else if (priority == max) {
			ops.write[idx++] = op;
		}
	}
	//switch (p_op_filter) {
	//	case TYPE_OP_FILTER_SUPPORTS: {
	//		for (TypeOperator *op : _operators) {
	//			int priority = op->has_priority(p_type, p_op_kind);
	//			if (priority > 0) {
	//				ops.write[idx++] = op;
	//			}
	//		}
	//	} break;
	//	case TYPE_OP_FILTER_MAX_PRIORITY: {
	//		int max = 0;
	//		for (TypeOperator *op : _operators) {
	//			int priority = op->has_priority(p_type, p_op_kind);
	//			if (priority > max) {
	//				max = priority;
	//				idx = 0;
	//				ops.fill(nullptr);
	//				ops.write[idx++] = op;
	//			} else if (priority == max) {
	//				ops.write[idx++] = op;
	//			}
	//		}
	//	} break;
	//	default:
	//		break;
	//}
	return ops;
}

TypeOperator *TypeServer::find_best_fit_operator(const Variant& p_type, TypeOpKind p_op_kind) const {
	Vector<TypeOperator *> ops = filter_operators(p_type, p_op_kind, TYPE_OP_FILTER_MAX_PRIORITY);
	return ops.is_empty() ? nullptr : ops[0];
}

bool TypeServer::get_operator(const StringName &p_name, TypeOperator *r_op) const {
	for (TypeOperator *op : _operators) {
		if (op->get_operator_name() == p_name) {
			r_op = op;
			return true;
		}
	}
	r_op = nullptr;
	ERR_FAIL_V_MSG(false, vformat("No %s with name '%s' has been registered.", SNAME("TypeOperator"), p_name));
}

void TypeServer::add_operator(TypeOperator * const p_operator) {
	ERR_FAIL_COND_MSG(!p_operator, vformat("Cannot add a null %s", SNAME("TypeOperator")));
	for (const TypeOperator *op : _operators) {
		ERR_FAIL_COND_MSG(op == p_operator, vformat("The given %s with name '%s' has already been registered.", SNAME("TypeOperator"), p_operator->get_operator_name()));
	}
	_operators.append(p_operator);
}

void TypeServer::remove_operator(const TypeOperator *p_operator) {
	ERR_FAIL_COND_MSG(!p_operator, vformat("Cannot remove a null %s", SNAME("TypeOperator")));
	for (int i = 0; i < _operators.size(); i++) {
		if (_operators[i] == p_operator) {
			_operators.remove_at(i);
		}
	}
	ERR_FAIL_MSG(vformat("The given %s with name '%s' is not present and could not be removed.", SNAME("TypeOperator"), p_operator->get_operator_name()));
}

#undef FIND_OP
