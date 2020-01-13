#include "commands/forLoop.h"
#include <sstream>
#include <spdlog/spdlog.h>
#include "common/program.h"

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

struct ForLoop::ValidateCounterViolationVisitor {
	const PId counter;

	ValidateCounterViolationVisitor(const PId counter) : counter(counter) {}

	template<class T>
	bool operator()(const T* t) const {
		return true;
	}

	bool operator()(const Assign* assign) const {
		return not assign->modifies(counter);
	}

	bool operator()(const Read* read) const {
		return not read->modifies(counter);
	}

	bool operator()(const ForLoop* loop) const {
		return loop->validateCounterViolation(counter);
	}
};

bool ForLoop::validateCounterViolation() const {
	return validateCounterViolation(counter);
}

bool ForLoop::validateCounterViolation(PId counter) const {
	for(auto command: *program->commands) {
		if(!std::visit(ValidateCounterViolationVisitor(counter), *command)) {
			spdlog::error("Loop counter '{}' violation:", counter);
			spdlog::error(*std::visit(AnyCommandVisitor(), *command));
			return false;
		}
	}
	return true;
}

void ForLoop::translate(const Program*, Intercode*) const {
	
};