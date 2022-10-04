#pragma once

#include "servers/type_server.h"

#include "core/core_bind.h"

class CoreTypeOperator : public TypeOperator {

public:
	virtual PackedStringArray CoreTypeOperator::all_get_type_list(bool p_no_named = false, bool p_no_anonymous = true) const override;
};
