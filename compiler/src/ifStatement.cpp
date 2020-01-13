#include "commands/ifStatement.h"
#include <sstream>

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

void IfStatement::translate(const Program*, Intercode*) const {
	
};