#include "commands/conditionalLoop.h"
#include <sstream>

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
