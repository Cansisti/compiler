#include "commands/translate.h"

size_t getValueAddress(const Program* program, Intercode* code, const Value* value) {
	auto addr = std::visit(ValueTranslateVisitor(program, code), *value);
	if(addr.a1 == Intercode::not_an_addr) {
		return addr.a0;
	}
	else {
		code->add(Intercode::Operation::remember, addr.a0, addr.a1);
		return Intercode::temp_addr;
	}
}
