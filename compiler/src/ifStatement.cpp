#include "commands/ifStatement.h"
#include <sstream>
#include "common/program.h"

IfStatement::IfStatement(Condition* condition, Program* positive, Program* negative) :
	condition(condition),
	positive(positive),
	negative(negative)
{

}

const std::string IfStatement::describe() const {
	std::stringstream ss;
	ss << "if statement " << " on " << condition;
	return ss.str();
}

void IfStatement::translate(const Program* program, Intercode* code) const {
	auto skip_label = code->generateLabel();
	auto end_label = code->generateLabel();
	auto cndtr = condition->translate(program, code);
	code->add(cndtr.op, skip_label);
	if(!cndtr.positive) positive->translate(code);
	else if(negative) negative->translate(code);
	code->add(Intercode::Operation::jump, end_label);
	code->putLabel(skip_label);
	if(cndtr.positive) positive->translate(code);
	else if(negative) negative->translate(code);
	code->putLabel(end_label);
};