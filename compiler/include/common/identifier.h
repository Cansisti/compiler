#pragma once
#include <variant>

#include "identifiers/constantTableIdentifier.h"
#include "identifiers/labeledTableIdentifier.h"
#include "identifiers/variableIdentifier.h"

typedef std::variant<
	ConstantTableIdentifier,
	LabeledTableIdentifier,
	VariableIdentifier
> Identifier;
