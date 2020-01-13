#pragma once
#include "common/expressionType.h"

class SubExpression: public virtual ExpressionType {
	using ExpressionType::ExpressionType;
	virtual Intercode::Operation icop() const override { return Intercode::Operation::sub; }
};
