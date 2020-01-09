#pragma once
#include <variant>
#include "anything.h"
#include "value.h"

class ExpressionType: public virtual Anything {
	public:
		ExpressionType(const std::string name, const size_t line, const Value* left, const Value* right) :
			Anything("expression", name, line),
			left(left),
			right(right)
		{}

		const Value* left;
		const Value* right;
};

typedef std::variant<ExpressionType*, Value*> Expression;
