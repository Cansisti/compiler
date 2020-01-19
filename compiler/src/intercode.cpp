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
	code->setAddr(r0);
	code->setAddr(r1);
	code->setAddr(r2);
	code->setAddr(r3);
	code->setAddr(r4);
	code->setAddr(rt);
	code->setAddr(acc);
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
				translateMul(code, command.a0, command.a1, command.a2);
				code->add(Machinecode::Operation::load, acc);
				code->add(Machinecode::Operation::store, rt);
				break;
			}
			case Operation::div: {
				translateDiv_v2(code, command.a0, command.a1, command.a2);
				code->add(Machinecode::Operation::load, acc);
				code->add(Machinecode::Operation::store, rt);
				break;
			}
			case Operation::mod: {
				translateDiv_v2(code, command.a0, command.a1, command.a2);
				code->add(Machinecode::Operation::load, r1);
				code->add(Machinecode::Operation::store, rt);
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
	code->add(Machinecode::Operation::sub);
	code->add(Machinecode::Operation::store, acc);

	code->add(Machinecode::Operation::load, a0, a1);
	code->add(Machinecode::Operation::store, r2);

	code->add(Machinecode::Operation::load, a2);
	code->add(Machinecode::Operation::store, r4);

	theTrickOfSign(code, r2, r4);

	auto loop = generateLabel();
	auto finish_r2_pos = generateLabel();
	auto finish_r4_pos = generateLabel();
	auto finish_r2_neg = generateLabel();
	auto finish_r4_neg = generateLabel();
	auto end = generateLabel();

	code->add(Machinecode::Operation::label, vars[loop]);

	code->add(Machinecode::Operation::load, r4);
	// if r4 == 0
	code->add(Machinecode::Operation::jzero, vars[end]);
	// if r4 == 1 finish
	code->add(Machinecode::Operation::dec);
	code->add(Machinecode::Operation::jzero, vars[finish_r4_pos]);
	// if r4 == -1 finish
	code->add(Machinecode::Operation::inc);
	code->add(Machinecode::Operation::inc);
	code->add(Machinecode::Operation::jzero, vars[finish_r4_neg]);

	code->add(Machinecode::Operation::load, r2);
	// if r2 == 0 finish
	code->add(Machinecode::Operation::jzero, vars[end]);
	// if r2 == 1 finish
	code->add(Machinecode::Operation::dec);
	code->add(Machinecode::Operation::jzero, vars[finish_r2_pos]);
	// if r2 == -1 finish
	code->add(Machinecode::Operation::inc);
	code->add(Machinecode::Operation::inc);
	code->add(Machinecode::Operation::jzero, vars[finish_r2_neg]);

	factorize(code, r2, r1);
	code->add(Machinecode::Operation::store, r2);

	factorize(code, r4, r3);
	code->add(Machinecode::Operation::store, r4);

	// remember r4 * 2^r1
	code->add(Machinecode::Operation::shift, r1);
	code->add(Machinecode::Operation::store, rt);

	// remember r2 * 2^r3
	code->add(Machinecode::Operation::load, r2);
	code->add(Machinecode::Operation::shift, r3);

	// sum those two
	code->add(Machinecode::Operation::add, rt);
	code->add(Machinecode::Operation::store, rt);

	// change it's sign
	code->add(Machinecode::Operation::sub, rt);
	code->add(Machinecode::Operation::sub, rt);
	code->add(Machinecode::Operation::store, rt);

	// calculate 2^(r1+r3)
	code->add(Machinecode::Operation::sub, rt);
	code->add(Machinecode::Operation::inc);
	code->add(Machinecode::Operation::shift, r1);
	code->add(Machinecode::Operation::shift, r3);

	// add prev two
	code->add(Machinecode::Operation::add, rt);

	// remember full thing
	code->add(Machinecode::Operation::add, acc);
	code->add(Machinecode::Operation::store, acc);

	// multiply r2*r4
	code->add(Machinecode::Operation::jump, vars[loop]);

	// finish when r2 == 1
	code->add(Machinecode::Operation::label, vars[finish_r2_pos]);
	code->add(Machinecode::Operation::load, r4);
	code->add(Machinecode::Operation::add, acc);
	code->add(Machinecode::Operation::store, acc);
	code->add(Machinecode::Operation::jump, vars[end]);

	// finish when r2 == -1
	code->add(Machinecode::Operation::label, vars[finish_r2_neg]);
	code->add(Machinecode::Operation::sub);
	code->add(Machinecode::Operation::sub, r4);
	code->add(Machinecode::Operation::add, acc);
	code->add(Machinecode::Operation::store, acc);
	code->add(Machinecode::Operation::jump, vars[end]);

	// finish when r4 == 1
	code->add(Machinecode::Operation::label, vars[finish_r4_pos]);
	code->add(Machinecode::Operation::load, r2);
	code->add(Machinecode::Operation::add, acc);
	code->add(Machinecode::Operation::store, acc);
	code->add(Machinecode::Operation::jump, vars[end]);

	// finish when r4 == -1
	code->add(Machinecode::Operation::label, vars[finish_r4_neg]);
	code->add(Machinecode::Operation::sub);
	code->add(Machinecode::Operation::sub, r2);
	code->add(Machinecode::Operation::add, acc);
	code->add(Machinecode::Operation::store, acc);
	code->add(Machinecode::Operation::jump, vars[end]);

	code->add(Machinecode::Operation::label, vars[end]);

	auto end_end = generateLabel();
	auto change_sign = generateLabel();

	code->add(Machinecode::Operation::load, r5);
	code->add(Machinecode::Operation::jzero, vars[change_sign]);
	code->add(Machinecode::Operation::jump, vars[end_end]);
	code->add(Machinecode::Operation::label, vars[change_sign]);
	code->add(Machinecode::Operation::sub, acc);
	code->add(Machinecode::Operation::store, acc);
	code->add(Machinecode::Operation::label, vars[end_end]);
}

void Intercode::factorize(Machinecode* code, Address* num, Address* power_of_to) {
	auto loop = generateLabel();
	auto end_pos = generateLabel();
	auto end_neg = generateLabel();
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
	code->add(Machinecode::Operation::jzero, vars[end_pos]);

	// Cause floor is a bitch (aka it rounds down)
	code->add(Machinecode::Operation::inc);
	code->add(Machinecode::Operation::jzero, vars[end_neg]);
	code->add(Machinecode::Operation::dec);

	// Else, count the shift
	// Remember r0
	code->add(Machinecode::Operation::store, r0);

	// Increment shift counter
	code->add(Machinecode::Operation::load, power_of_to);
	code->add(Machinecode::Operation::inc);
	code->add(Machinecode::Operation::store, power_of_to);

	// Shift again
	code->add(Machinecode::Operation::jump, vars[loop]);

	// Label end - shifting finished with 1
	code->add(Machinecode::Operation::label, vars[end_pos]);
	// p0 is 0 for sure
	code->add(Machinecode::Operation::inc);
	// now it's 1
	code->add(Machinecode::Operation::shift, power_of_to);
	// and now its 2^power_of_to
	code->add(Machinecode::Operation::sub, num);
	// currently, num = 2^power_of_to - p0
	// and p0 == remainder
	code->add(Machinecode::Operation::jump, vars[end]);

	// Label end - shifting finished with -1
	code->add(Machinecode::Operation::label, vars[end_neg]);
	code->add(Machinecode::Operation::dec);
	code->add(Machinecode::Operation::shift, power_of_to);
	code->add(Machinecode::Operation::sub, num);
	// currently, num = 2^power_of_to + p0
	// need to make it -p0
	code->add(Machinecode::Operation::store, r0);
	code->add(Machinecode::Operation::sub, r0);
	code->add(Machinecode::Operation::sub, r0);
	// now p0 == remainder
	code->add(Machinecode::Operation::jump, vars[end]);

	// the end
	code->add(Machinecode::Operation::label, vars[end]);
}

void Intercode::translateDiv(Machinecode* code, Address* a0, Address* a1, Address* a2) {
	code->add(Machinecode::Operation::sub);
	code->add(Machinecode::Operation::store, acc); // count here
	code->add(Machinecode::Operation::store, r1); // powers of two

	auto end_result_zero = generateLabel();
	auto end_result_one = generateLabel();
	auto end_result_left = generateLabel();
	auto end_div_by_zero = generateLabel();
	auto end = generateLabel();

	code->add(Machinecode::Operation::load, a2);
	// check division by 0
	code->add(Machinecode::Operation::jzero, vars[end_div_by_zero]);
	// check if someone is trying to trick us
	code->add(Machinecode::Operation::dec);
	code->add(Machinecode::Operation::jzero, vars[end_result_left]);
	code->add(Machinecode::Operation::inc); // alright, take it back
	code->add(Machinecode::Operation::store, r4);
	code->add(Machinecode::Operation::store, r0); // for some later use

	// save those
	code->add(Machinecode::Operation::load, a0, a1);
	code->add(Machinecode::Operation::store, rt);

	theTrickOfSign(code, r4, rt);
	getRidOfSign(code, r4);
	getRidOfSign(code, rt);

	auto exceeded = generateLabel();
	// not a trick but still
	// got rt here from last load/store
	code->add(Machinecode::Operation::sub, r4);
	code->add(Machinecode::Operation::jneg, vars[exceeded]); // if right is more
	code->add(Machinecode::Operation::jzero, vars[end_result_one]); // if they the same

	// the magic
	// ---------
	auto loop = generateLabel();
	auto end_loop = generateLabel();

	// before start r0 = r4
	code->add(Machinecode::Operation::label, vars[loop]); // label loop
	// maybe 2*r0 fits? PROBLEM HERE IF r0 < 0 (getRidOfSign fixes it)
	code->add(Machinecode::Operation::load, r0);
	code->add(Machinecode::Operation::shift, code->cp1);
	code->add(Machinecode::Operation::store, r0);
	// does it? does it?
	code->add(Machinecode::Operation::sub, rt); // threat rt as rest (not divied yet)
	code->add(Machinecode::Operation::jpos, vars[exceeded]);
	// yay, it does, remember it
	code->add(Machinecode::Operation::load, r1);
	code->add(Machinecode::Operation::inc);
	code->add(Machinecode::Operation::store, r1);
	// nah, it doesn't
	code->add(Machinecode::Operation::label, vars[exceeded]); // label exceeded
	code->add(Machinecode::Operation::sub);
	code->add(Machinecode::Operation::inc);
	code->add(Machinecode::Operation::shift, r1);
	code->add(Machinecode::Operation::add, acc);
	code->add(Machinecode::Operation::store, acc); // remember how much
	code->add(Machinecode::Operation::load, r4);
	code->add(Machinecode::Operation::store, r0); // renew r0
	code->add(Machinecode::Operation::shift, r1); // how much of it fits already
	code->add(Machinecode::Operation::store, r2); // why not remember it
	code->add(Machinecode::Operation::load, rt);
	code->add(Machinecode::Operation::sub, r2);
	code->add(Machinecode::Operation::store, rt); // and have a little less to divide
	code->add(Machinecode::Operation::sub, r4); // can it fit any more?
	code->add(Machinecode::Operation::jneg, vars[end_loop]); // uff, it can't
	code->add(Machinecode::Operation::jump, vars[loop]); // well then, keep it up
	code->add(Machinecode::Operation::label, vars[end_loop]); // label end_loop
	
	// change those signs, before you go
	auto change_sign_end = generateLabel();
	auto change_sign = generateLabel();

	code->add(Machinecode::Operation::load, r5);
	code->add(Machinecode::Operation::jzero, vars[change_sign]);
	code->add(Machinecode::Operation::jump, vars[change_sign_end]);
	code->add(Machinecode::Operation::label, vars[change_sign]);
	code->add(Machinecode::Operation::sub, acc);
	code->add(Machinecode::Operation::dec);
	code->add(Machinecode::Operation::store, acc);

	code->add(Machinecode::Operation::load, rt);
	code->add(Machinecode::Operation::sub, r4);
	code->add(Machinecode::Operation::store, rt);
	code->add(Machinecode::Operation::label, vars[change_sign_end]);

	// to satisfy alien's civilization negative modulo
	auto dividee_is_negative = generateLabel();
	auto end_dividee_is_negative = generateLabel();
	code->add(Machinecode::Operation::load, a0, a1);
	code->add(Machinecode::Operation::jneg, vars[dividee_is_negative]);
	code->add(Machinecode::Operation::jump, vars[end_dividee_is_negative]);
	code->add(Machinecode::Operation::label, vars[dividee_is_negative]);
	code->add(Machinecode::Operation::sub);
	code->add(Machinecode::Operation::sub, rt);
	code->add(Machinecode::Operation::store, rt);
	code->add(Machinecode::Operation::label, vars[end_dividee_is_negative]);

	// TODO MOD SIGN
	// TODO CHECK IN TEST (AND IMPLEMENT?) "A DIV -1" AND IT'S SIGNS

	code->add(Machinecode::Operation::jump, vars[end]);

	// those tricks
	// -----------
	code->add(Machinecode::Operation::label, vars[end_result_zero]); // if rt - r4 < 0
	code->add(Machinecode::Operation::sub);
	code->add(Machinecode::Operation::store, acc); // result is 0
	code->add(Machinecode::Operation::load, rt);
	code->add(Machinecode::Operation::store, rt); // remainder is rt
	code->add(Machinecode::Operation::jump, vars[end]);

	code->add(Machinecode::Operation::label, vars[end_result_one]); // if rt - r4 = 0
	// already got 0 cause jzero in here
	code->add(Machinecode::Operation::store, rt); // remainder is 0
	code->add(Machinecode::Operation::inc);
	code->add(Machinecode::Operation::store, acc); // result is 1
	code->add(Machinecode::Operation::jump, vars[end]);

	code->add(Machinecode::Operation::label, vars[end_result_left]); // if r4 = 1
	// already got 0 cause jzero in here
	code->add(Machinecode::Operation::store, rt); // remainder is 0
	code->add(Machinecode::Operation::load, a0, a1); // careful, as we don't have it yet in rt
	code->add(Machinecode::Operation::store, acc); // result is a0(a1)
	code->add(Machinecode::Operation::jump, vars[end]);

	code->add(Machinecode::Operation::label, vars[end_div_by_zero]); // if r4 = 0 !!!
	// already got 0 cause jzero in here
	code->add(Machinecode::Operation::store, rt); // remainder is 0 for div 0
	code->add(Machinecode::Operation::store, acc); // result is 0 also
	code->add(Machinecode::Operation::jump, vars[end]);

	code->add(Machinecode::Operation::label, vars[end]);
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

void Intercode::theTrickOfSign(Machinecode* code, Address* left, Address* right) {
	auto left_negative = generateLabel();
	auto check_right_negative = generateLabel();
	auto right_negative = generateLabel();
	auto end_check = generateLabel();

	code->add(Machinecode::Operation::sub);
	code->add(Machinecode::Operation::inc);
	code->add(Machinecode::Operation::store, r5);
	code->add(Machinecode::Operation::load, left);
	code->add(Machinecode::Operation::jneg, vars[left_negative]);
	code->add(Machinecode::Operation::jump, vars[check_right_negative]);
	code->add(Machinecode::Operation::label, vars[left_negative]);
	code->add(Machinecode::Operation::load, r5);
	code->add(Machinecode::Operation::dec);
	code->add(Machinecode::Operation::store, r5);
	code->add(Machinecode::Operation::label, vars[check_right_negative]);
	code->add(Machinecode::Operation::load, right);
	code->add(Machinecode::Operation::jneg, vars[right_negative]);
	code->add(Machinecode::Operation::jump, vars[end_check]);
	code->add(Machinecode::Operation::label, vars[right_negative]);
	code->add(Machinecode::Operation::load, r5);
	code->add(Machinecode::Operation::dec);
	code->add(Machinecode::Operation::store, r5);
	code->add(Machinecode::Operation::label, vars[end_check]);
}

void Intercode::getRidOfSign(Machinecode* code, Address* a) {
	auto negative = generateLabel();
	auto end = generateLabel();
	code->add(Machinecode::Operation::load, a);
	code->add(Machinecode::Operation::jneg, vars[negative]);
	code->add(Machinecode::Operation::jump, vars[end]);
	code->add(Machinecode::Operation::label, vars[negative]);
	code->add(Machinecode::Operation::sub);
	code->add(Machinecode::Operation::sub, a);
	code->add(Machinecode::Operation::store, a);
	code->add(Machinecode::Operation::label, vars[end]);
}

void Intercode::translateDiv_v2(Machinecode* code, Address* a0, Address* a1, Address* a2) {
	auto finish = generateLabel();
	code->add(Machinecode::Operation::sub);
	code->add(Machinecode::Operation::store, acc);
	code->add(Machinecode::Operation::store, r1);
	code->add(Machinecode::Operation::store, r3);

	code->add(Machinecode::Operation::load, a2);
	code->add(Machinecode::Operation::jzero, vars[finish]);
	code->add(Machinecode::Operation::store, r0);
	code->add(Machinecode::Operation::store, r2);

	code->add(Machinecode::Operation::load, a0, a1);
	code->add(Machinecode::Operation::store, r1);

	theTrickOfSign(code, r1, r0);

	auto sign_different = generateLabel();
	code->add(Machinecode::Operation::load, r5); // it should be already here, but take it easy
	code->add(Machinecode::Operation::jzero, vars[sign_different]);

	{ // SAME SIGN - add here, until sign changes
		code->add(Machinecode::Operation::load, r1);
		auto dividee_positive = generateLabel();
		code->add(Machinecode::Operation::jpos, vars[dividee_positive]);
		{ // DIVIDEE NEGATIVE - sign changes when r1 is POSITIVE
			auto begin = generateLabel();
			code->add(Machinecode::Operation::label, vars[begin]);
			code->add(Machinecode::Operation::jzero, vars[finish]);
				magic(code, Machinecode::Operation::jpos, Machinecode::Operation::sub, Machinecode::Operation::add);
			code->add(Machinecode::Operation::jump, vars[finish]);
		}
		{ // DIVIDEE POSITIVE - sign changes when r1 is NEGATIVE
			code->add(Machinecode::Operation::label, vars[dividee_positive]);
			auto begin = generateLabel();
			code->add(Machinecode::Operation::label, vars[begin]);
			code->add(Machinecode::Operation::jzero, vars[finish]);
				magic(code, Machinecode::Operation::jneg, Machinecode::Operation::sub, Machinecode::Operation::add);
			code->add(Machinecode::Operation::jump, vars[finish]);
		}
	}

	{ // DIFFERENT SIGN
		code->add(Machinecode::Operation::label, vars[sign_different]);
		code->add(Machinecode::Operation::load, r1);
		auto dividee_positive = generateLabel();
		code->add(Machinecode::Operation::jpos, vars[dividee_positive]);
		{ // DIVIDEE NEGATIVE - sign changes when r1 is POSITIVE
			auto begin = generateLabel();
			code->add(Machinecode::Operation::label, vars[begin]);
			code->add(Machinecode::Operation::jpos, vars[finish]);
			code->add(Machinecode::Operation::jzero, vars[finish]);
				magic(code, Machinecode::Operation::jpos, Machinecode::Operation::add, Machinecode::Operation::sub, [&](){
					loadPerformStore(code, Machinecode::Operation::dec, acc);
					code->add(Machinecode::Operation::load, r1);
					code->add(Machinecode::Operation::add, r2);
					code->add(Machinecode::Operation::store, r1);
				});
			code->add(Machinecode::Operation::jump, vars[begin]);	
		}
		{ // DIVIDEE POSITIVE - sign changes when r1 is NEGATIVE
			code->add(Machinecode::Operation::label, vars[dividee_positive]);
			auto begin = generateLabel();
			code->add(Machinecode::Operation::label, vars[begin]);
			code->add(Machinecode::Operation::jneg, vars[finish]);
			code->add(Machinecode::Operation::jzero, vars[finish]);
				magic(code, Machinecode::Operation::jneg, Machinecode::Operation::add, Machinecode::Operation::sub, [&](){
					loadPerformStore(code, Machinecode::Operation::dec, acc);
					code->add(Machinecode::Operation::load, r1);
					code->add(Machinecode::Operation::add, r2);
					code->add(Machinecode::Operation::store, r1);
				});
			code->add(Machinecode::Operation::jump, vars[begin]);	
		}
	}
	code->add(Machinecode::Operation::label, vars[finish]);
}

void Intercode::loadPerformStore(Machinecode* code, Machinecode::Operation op, Address* a) {
	code->add(Machinecode::Operation::load, a);
	code->add(op);
	code->add(Machinecode::Operation::store, a);
}

void Intercode::magic(
	Machinecode* code,
	Machinecode::Operation exceeded_policy,
	Machinecode::Operation exceed_operation,
	Machinecode::Operation acc_operation,
	std::function<void()> on_remainder_callback
) {
	auto loop = generateLabel();
	auto exceeded = generateLabel();
	auto check_remainder = generateLabel();
	auto quit = generateLabel();
	code->add(Machinecode::Operation::label, vars[loop]);
		code->add(Machinecode::Operation::load, r1);
		code->add(exceed_operation, r2);
		code->add(exceeded_policy, vars[exceeded]);
		loadPerformStore(code, Machinecode::Operation::inc, r3);
		code->add(Machinecode::Operation::load, r2);
		code->add(Machinecode::Operation::shift, code->cp1);
		code->add(Machinecode::Operation::store, r2);
		code->add(Machinecode::Operation::jump, vars[loop]);
	code->add(Machinecode::Operation::label, vars[exceeded]);
		code->add(Machinecode::Operation::load, r3);
		code->add(Machinecode::Operation::jzero, vars[check_remainder]);
		code->add(Machinecode::Operation::dec);
		code->add(Machinecode::Operation::store, r4);
		code->add(Machinecode::Operation::sub);
		code->add(Machinecode::Operation::store, r3);
		code->add(Machinecode::Operation::inc);
		code->add(Machinecode::Operation::shift, r4);
		code->add(Machinecode::Operation::store, r4);
		code->add(Machinecode::Operation::load, acc);
		code->add(acc_operation, r4);
		code->add(Machinecode::Operation::store, acc);
		code->add(Machinecode::Operation::load, r2);
		code->add(Machinecode::Operation::shift, code->cn1);
		code->add(Machinecode::Operation::store, r2);
		code->add(Machinecode::Operation::load, r1);
		code->add(exceed_operation, r2);
		code->add(Machinecode::Operation::store, r1);
		code->add(Machinecode::Operation::load, r0);
		code->add(Machinecode::Operation::store, r2);
		code->add(Machinecode::Operation::jump, vars[loop]);
	code->add(Machinecode::Operation::label, vars[check_remainder]);
		code->add(Machinecode::Operation::load, r1);
		code->add(Machinecode::Operation::jzero, vars[quit]);
		on_remainder_callback();
	code->add(Machinecode::Operation::label, vars[quit]);
}