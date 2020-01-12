#pragma once
#include <variant>
#include <string>

#include "identifier.h"
#include "value.h"
#include "expression.h"
#include "condition.h"
#include "anyCommand.h"

typedef std::variant<
	PId,
	Num,
	Identifier*,
	Value*,
	Expression*,
	Condition*,
	AnyCommand*,
	Commands*
> Any;
