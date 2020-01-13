#include "intercode/intercode.h"

Intercode::Intercode() {
	vars[0] = nullptr;
}

void Intercode::declare(size_t id, Address::Type type, size_t size) {
	vars[id] = new Address(type, size);
}

void Intercode::add(Intercode::Operation op, size_t s0, size_t s1, size_t s2) {
	commands.push_back({
		op,
		vars[s0],
		vars[s1],
		vars[s2]
	});
}

void Intercode::constant(size_t id, long long value) {
	auto addr = new Address(Address::Type::constant, 1);
	addr->value = value;
	vars[id] = addr;
}