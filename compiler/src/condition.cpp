#include "common/condition.h"
#include "common/program.h"
#include "commands/translate.h"
#include "assert.h"

Condition::Condition(PId name, size_t line, const Value* left, const Value* right, Condition::Modifier mod):
	Anything("condition", name, line),
	left(left),
	right(right),
	modifier(mod)
{

}

Condition::Translation Condition::translate(const Program* program, Intercode* code) const {
	auto left_addr = std::visit(ValueTranslateVisitor(program, code), *left);
	auto right_addr = getValueAddress(program, code, right);
	code->add(Intercode::Operation::sub, left_addr.a0, left_addr.a1, right_addr);
	switch(modifier) {
		case Modifier::equal: {
			return { true, Intercode::Operation::jump_zero };
		}
		case Modifier::different: {
			return { false, Intercode::Operation::jump_zero };
		}
		case Modifier::greater: {
			return { true, Intercode::Operation::jump_pos };
		}
		case Modifier::less: {
			return { true, Intercode::Operation::jump_neg };
		}
		case Modifier::greater_or_equal: {
			return { false, Intercode::Operation::jump_neg };
		}
		case Modifier::less_or_equal: {
			return { false, Intercode::Operation::jump_pos };
		}
	}
	assert(false);
	return { true, Intercode::Operation::jump };
}