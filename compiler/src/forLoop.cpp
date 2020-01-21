#include "commands/forLoop.h"
#include <sstream>
#include <spdlog/spdlog.h>
#include "common/program.h"
#include "commands/translate.h"

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

void ForLoop::translate(const Program* program, Intercode* code) const {
	auto begin_label = code->generateLabel("f-beg");
	auto end_label = code->generateLabel("f-end");
	this->program->declare(code);
	auto c = this->program->findDeclaration(counter)->address;
	auto f = getValueAddress(program, code, from);
	code->add(Intercode::Operation::assign, c, Intercode::not_an_addr, f);
	auto t = Declaration::next_address++;
	code->declare(t, Address::Type::variable, "*t");
	code->add(Intercode::Operation::assign, t, Intercode::not_an_addr, getValueAddress(program, code, to));
	code->add(Intercode::Operation::sub, c, Intercode::not_an_addr, t);
	code->add(modifier == Modifier::up ? Intercode::Operation::jump_pos : Intercode::Operation::jump_neg, end_label);
	code->add(modifier == Modifier::up ? Intercode::Operation::inc : Intercode::Operation::dec, t);
	code->putLabel(begin_label);
	code->add(Intercode::Operation::sub, c, Intercode::not_an_addr, t);
	code->add(Intercode::Operation::jump_zero, end_label);
	this->program->translate(code);
	code->add(modifier == Modifier::up ? Intercode::Operation::inc : Intercode::Operation::dec, c);
	code->add(Intercode::Operation::jump, begin_label);
	code->putLabel(end_label);
};