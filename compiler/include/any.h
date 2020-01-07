#pragma once
#include <variant>
#include <string>

#include "declaration.h"
#include "expression.h"

typedef std::variant<
	std::string,
	long long,
	Declaration*,
	Expression*
> Any;
