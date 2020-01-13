#include "commands/forLoop.h"
#include <sstream>

ForLoop::ForLoop(PId counter, ForLoop::Modifier modifier, Value* from, Value* to, Program* program) :
	counter(counter),
	modifier(modifier),
	from(from),
	to(to),
	program(program)
{

}

const std::string ForLoop::describe() const {
	std::stringstream ss;
	ss << "for loop " << (modifier == Modifier::up ? "up" : "down") << " on " << counter;
	return ss.str();
}

void ForLoop::translate(const Program*, Intercode*) const {
	
};