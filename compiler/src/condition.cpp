#include "common/condition.h"

Condition::Condition(PId name, size_t line, const Value* left, const Value* right, Condition::Modifier mod):
	Anything("condition", name, line),
	left(left),
	right(right),
	modifier(mod)
{

}