#pragma once
#include <variant>

#include "identifier.h"

typedef std::variant<Identifier*, Num> Value;
