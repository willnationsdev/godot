#pragma once

#include "servers/type_server.h"

#include "core/core_bind.h"

class CoreTypeOperator : public TypeOperator {

public:
	virtual PackedStringArray CoreTypeOperator::get_type_list() const override;
};
