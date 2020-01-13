#include "commands/conditionalLoop.h"
#include <sstream>
#include "common/program.h"

ConditionalLoop::ConditionalLoop(Condition* condition, ConditionalLoop::Modifier modifier, Program* program) :
	condition(condition),
	modifier(modifier),
	program(program)
{

}

const std::string ConditionalLoop::describe() const {
	std::stringstream ss;
	ss << "loop " << (modifier == Modifier::do_while ? "do-while" : "while-do") << " on " << condition;
	return ss.str();
}

void ConditionalLoop::translate(const Program* program, Intercode* code) const {
	if(modifier == Modifier::do_while) {
		this->program->translate(code);
	}
	auto begin_label = code->generateLabel();
	auto body_label = code->generateLabel();
	auto end_label = code->generateLabel();
	code->putLabel(begin_label);
	auto cndtr = condition->translate(program, code);
	code->add(cndtr.op, cndtr.positive ? body_label : end_label);
	if(cndtr.positive) code->add(Intercode::Operation::jump, end_label);
	else code->putLabel(body_label);
	this->program->translate(code);
	code->add(Intercode::Operation::jump, begin_label);
	code->putLabel(end_label);
};