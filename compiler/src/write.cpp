#include "commands/write.h"
#include <sstream>

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
