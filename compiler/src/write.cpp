#include "commands/write.h"
#include <sstream>
#include "commands/translate.h"

Write::Write(Value* value, size_t line) :
	value(value),
	line(line)
{

}

const std::string Write::describe() const {
	std::stringstream ss;
	ss << "write  at " << line;
	return ss.str();
}

void Write::translate(const Program* program, Intercode* code) const {
	auto ass = std::visit(ValueTranslateVisitor(program, code), *value);
	code->add(Intercode::Operation::put, ass.a0, ass.a1, ass.a2);
};
