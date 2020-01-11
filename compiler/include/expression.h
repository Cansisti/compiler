#pragma once
#include <variant>
#include "anything.h"
#include "value.h"

#include "expressions/addExpression.h"
#include "expressions/subExpression.h"
#include "expressions/mulExpression.h"
#include "expressions/divExpression.h"
#include "expressions/modExpression.h"

typedef std::variant<
	AddExpression*,
	SubExpression*,
	MulExpression*,
	DivExpression*,
	ModExpression*
> AnyExpression;

typedef std::variant<AnyExpression*, Value*> Expression;
