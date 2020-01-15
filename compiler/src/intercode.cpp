#include "intercode/intercode.h"
#include "common/declaration.h"
#include <assert.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

const size_t Intercode::not_an_addr = 0;
const size_t Intercode::temp_addr = -1;

Intercode::Intercode() {
	vars[not_an_addr] = nullptr;
	vars[temp_addr] = rt;
}

void Intercode::declare(size_t id, Address::Type type, std::string name, size_t size, long long offset) {
	vars[id] = new Address(type, size, name, offset);
}

void Intercode::add(Intercode::Operation op, size_t s0, size_t s1, size_t s2) {
	commands.push_back({
		op,
		vars[s0],
		vars[s1],
		vars[s2]
	});
}

size_t Intercode::generateLabel(std::string name) {
	vars[next_label] = new Address(Address::Type::label, 1, name);
	return next_label--;
}

void Intercode::putLabel(size_t l) {
	assert(vars[l]);
	add(Operation::label, l);
}

size_t Intercode::constant(long long value) {
	auto id = Declaration::next_address++;
	auto addr = new Address(Address::Type::constant, 1, "c(" + std::to_string(value) + ")", value);
	vars[id] = addr;
	return id;
}

void Intercode::translate(Machinecode* code) {
	spdlog::info("Start intercode translation...");
	for(auto command: commands) {
		spdlog::debug("{}:\t{}\t{}\t{}", (int) command.op, (void*) command.a0, (void*) command.a1, (void*) command.a2);
		switch(command.op) {
			case Operation::assign: {
				code->add(Machinecode::Operation::load, command.a2);
				code->add(Machinecode::Operation::store, command.a0, command.a1);
				break;
			}
			case Operation::get: {
				code->add(Machinecode::Operation::get);
				code->add(Machinecode::Operation::store, command.a0, command.a1);
				break;
			}
			case Operation::put: {
				code->add(Machinecode::Operation::load, command.a0, command.a1);
				code->add(Machinecode::Operation::put);
				break;
			}
			case Operation::add : {
				code->add(Machinecode::Operation::load, command.a0, command.a1);
				code->add(Machinecode::Operation::add, command.a2);
				code->add(Machinecode::Operation::store, rt);
				break;
			}
			case Operation::sub : {
				code->add(Machinecode::Operation::load, command.a0, command.a1);
				code->add(Machinecode::Operation::sub, command.a2);
				code->add(Machinecode::Operation::store, rt);
				break;
			}
			case Operation::mul: {
				code->add(Machinecode::Operation::load, acc);
				code->add(Machinecode::Operation::sub, acc);
				code->add(Machinecode::Operation::store, acc);
				translateMul(code, command.a0, command.a1, command.a2);
				code->add(Machinecode::Operation::load, acc);
				code->add(Machinecode::Operation::store, rt);
				break;
			}
			case Operation::div: {
				throw std::runtime_error("div is not supported yet");
				break;
			}
			case Operation::mod: {
				throw std::runtime_error("mod is not supported yet");
				break;
			}
			case Operation::inc: {
				code->add(Machinecode::Operation::load, command.a0, command.a1);
				code->add(Machinecode::Operation::inc);
				code->add(Machinecode::Operation::store, command.a0, command.a1);
				break;
			};
			case Operation::dec: {
				code->add(Machinecode::Operation::load, command.a0, command.a1);
				code->add(Machinecode::Operation::dec);
				code->add(Machinecode::Operation::store, command.a0, command.a1);
				break;
			};
			case Operation::jump: {
				code->add(Machinecode::Operation::jump, command.a0);
				break;
			}
			case Operation::jump_pos: {
				code->add(Machinecode::Operation::jpos, command.a0);
				break;
			}
			case Operation::jump_neg: {
				code->add(Machinecode::Operation::jneg, command.a0);
				break;
			}
			case Operation::jump_zero: {
				code->add(Machinecode::Operation::jzero, command.a0);
				break;
			}
			case Operation::remember: {
				code->add(Machinecode::Operation::load, command.a0, command.a1);
				code->add(Machinecode::Operation::store, rt);
				break;
			}
			case Operation::label: {
				code->add(Machinecode::Operation::label, command.a0);
				break;
			}
		}
	}
	spdlog::info("Intercode translation done");
}

void Intercode::translateMul(Machinecode* code, Address* a0, Address* a1, Address* a2) {
	code->add(Machinecode::Operation::load, a0, a1);
	code->add(Machinecode::Operation::store, r2);

	code->add(Machinecode::Operation::load, a2);
	code->add(Machinecode::Operation::store, r4);

	auto loop = generateLabel();
	auto finish_r2 = generateLabel();
	auto finish_r4 = generateLabel();
	auto end = generateLabel();

	code->add(Machinecode::Operation::label, vars[loop]);

	code->add(Machinecode::Operation::load, r4);
	code->add(Machinecode::Operation::dec);
	code->add(Machinecode::Operation::jzero, vars[finish_r4]);

	code->add(Machinecode::Operation::load, r2);
	code->add(Machinecode::Operation::dec);
	code->add(Machinecode::Operation::jzero, vars[finish_r2]);

	factorize(code, r2, r1);
	code->add(Machinecode::Operation::store, r2);

	factorize(code, a2, r3);
	code->add(Machinecode::Operation::store, r4);

	code->add(Machinecode::Operation::shift, r1);
	code->add(Machinecode::Operation::store, rt);

	code->add(Machinecode::Operation::load, r2);
	code->add(Machinecode::Operation::shift, r3);
	code->add(Machinecode::Operation::add, rt);
	code->add(Machinecode::Operation::store, rt);

	code->add(Machinecode::Operation::sub, rt);
	code->add(Machinecode::Operation::inc);
	code->add(Machinecode::Operation::shift, r1);
	code->add(Machinecode::Operation::shift, r3);
	code->add(Machinecode::Operation::add, rt);

	code->add(Machinecode::Operation::add, acc);
	code->add(Machinecode::Operation::store, acc);

	code->add(Machinecode::Operation::jump, vars[loop]);
	code->add(Machinecode::Operation::label, vars[finish_r2]);
	
	code->add(Machinecode::Operation::load, r4);
	code->add(Machinecode::Operation::add, acc);
	code->add(Machinecode::Operation::store, acc);

	code->add(Machinecode::Operation::jump, vars[end]);
	code->add(Machinecode::Operation::label, vars[finish_r4]);

	code->add(Machinecode::Operation::load, r2);
	code->add(Machinecode::Operation::add, acc);
	code->add(Machinecode::Operation::store, acc);

	code->add(Machinecode::Operation::label, vars[end]);
}

void Intercode::factorize(Machinecode* code, Address* num, Address* power_of_to) {
	auto loop = generateLabel();
	auto end = generateLabel();

	code->add(Machinecode::Operation::load, num);
	code->add(Machinecode::Operation::store, r0);

	code->add(Machinecode::Operation::load, power_of_to);
	code->add(Machinecode::Operation::sub, power_of_to);
	code->add(Machinecode::Operation::store, power_of_to);
	
	// Label loop
	code->add(Machinecode::Operation::label, vars[loop]);

	// Shift r0
	code->add(Machinecode::Operation::load, r0);
	code->add(Machinecode::Operation::shift, code->cn1);

	// If zero, shifting finished
	code->add(Machinecode::Operation::jzero, vars[end]);

	// Else, count the shift
	// Remember r0
	code->add(Machinecode::Operation::store, r0);

	// Increment shift counter
	code->add(Machinecode::Operation::load, power_of_to);
	code->add(Machinecode::Operation::inc);
	code->add(Machinecode::Operation::store, power_of_to);

	// Shift again
	code->add(Machinecode::Operation::jump, vars[loop]);

	// Label end - shifting finished
	code->add(Machinecode::Operation::label, vars[end]);
	// p0 is 0 for sure
	code->add(Machinecode::Operation::inc);
	// now it's 1
	code->add(Machinecode::Operation::shift, power_of_to);
	// and now its 2^power_of_to
	code->add(Machinecode::Operation::sub, num);
	// currently, num = 2^power_of_to - p0
	// and p0 == remainder
}

void Intercode::save(std::ofstream& file) {
	file << "DECLARATIONS:" << std::endl;
	for(auto addr: vars) {
		file << (int) addr.first << ":\t" << addr.second << "\t" << (addr.second ? addr.second->name : "-") << std::endl;
	}
	static const std::string oprs[] = {
		"assign",
		"get   ",
		"put   ",
		"add   ",
		"sub   ",
		"mul   ",
		"div   ",
		"mod   ",
		"inc   ",
		"dec   ",
		"jump  ",
		"jpos  ",
		"jneg  ",
		"jzero ",
		"rem   ",
		"label "
	};
	file << std::endl;
	file << "COMMANDS:" << std::endl;
	for(auto command: commands) {
		file << oprs[(int)command.op] << "\t";
		if(command.a0 and command.a0->name != "") file << command.a0->name << "\t"; else file << command.a0 << "\t";
		if(command.a1 and command.a1->name != "") file << command.a1->name << "\t"; else file << command.a1 << "\t";
		if(command.a2 and command.a2->name != "") file << command.a2->name << "\t"; else file << command.a2 << "\t";
		file << std::endl;
	}
}