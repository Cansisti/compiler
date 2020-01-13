#pragma once
#include "common/expressionType.h"

class ModExpression: public virtual ExpressionType {
	using ExpressionType::ExpressionType;
	virtual Intercode::Operation icop() const override { return Intercode::Operation::mod; }
};
