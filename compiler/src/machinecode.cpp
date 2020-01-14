#include "machinecode/machinecode.h"
#include <assert.h>
#include <cstdlib>
#include <spdlog/spdlog.h>

void Machinecode::add(Machinecode::Operation op, const Address* addr, const Address* offset) {
	commands.push_back({
		op,
		addr,
		offset
	});
}

void Machinecode::lgl(Machinecode::Operation op, Machinecode::taddr addr) {
	code.push_back({
		op,
		addr
	});
}

void Machinecode::legalize() {
	setAddr(cn1);
	setAddr(cp1);
	setAddr(t1);
	setAddr(t2);
	setAddresses();
	generateConstants();
	for(auto command: commands) {
		switch(command.op) {
			case Operation::label: {
				lgl(Operation::label, labels[command.addr]);
				break;
			}
			case Operation::jump:
			case Operation::jpos:
			case Operation::jneg:
			case Operation::jzero: {
				lgl(command.op, labels[command.addr]);
				break;
			}
			case Operation::store: {
				if(not command.offset) {
					lgl(Operation::store, addresses[command.addr]);
				}
				else {
					lgl(Operation::store, addresses[t1]);
					lgl(Operation::load, addresses[command.addr]);
					lgl(Operation::add, addresses[command.offset]);
					lgl(Operation::store, addresses[t2]);
					lgl(Operation::load, addresses[t1]);
					lgl(Operation::store_ind, addresses[t2]);
				}
				break;
			}
			case Operation::load: {
				if(not command.offset) {
					lgl(Operation::load, addresses[command.addr]);
				}
				else {
					lgl(Operation::load, addresses[command.addr]);
					lgl(Operation::add, addresses[command.offset]);
					lgl(Operation::load_ind, 0);
					lgl(Operation::load, 0);
				}
				break;
			};
			default: {
				assert(not command.offset);
				lgl(command.op, addresses[command.addr]);
			}
		}
	}
	for(size_t i = 0; i<code.size(); i++) {
		if(code[i].op == Operation::label) {
			label_translation[code[i].addr] = i;
			code.erase(code.begin() + i);
			i--;
		}
	}
	for(auto c: code) {
		assert(c.op != Operation::label);
		switch(c.op) {
			case Operation::jump:
			case Operation::jpos:
			case Operation::jneg:
			case Operation::jzero: {
				c.addr = label_translation[c.addr];
				break;
			}
			default: continue;
		}
	}
	lgl(Operation::halt);
}

void Machinecode::setAddresses() {
	for(auto command: commands) {
		if(command.op != Operation::label) {
			if(command.addr) setAddr(command.addr);
			if(command.offset) setAddr(command.offset);
		}
		else {
			assert(command.addr);
			assert(labels.find(command.addr) == labels.end());
			labels[command.addr] = next_label++;
		}
	}
}

void Machinecode::setAddr(const Address* addr) {
	if(addresses.find(addr) == addresses.end()) {
		auto a = next_addr;
		next_addr += addr->size;
		addresses[addr] = a;
	}
}

void Machinecode::generateConstants() {
	for(auto addr: addresses) {
		if(addr.first->type == Address::Type::constant) generateConst(addr.first);
	}
}

void Machinecode::generateConst(const Address* addr) {
	assert(addr);
	spdlog::info("Generating const {}", addr->value);

	lgl(Operation::sub, 0);
	if(addr->value == 0) {
		lgl(Operation::store, addresses[addr]);
		return;
	}
	auto iop = addr->value > 0 ? Operation::inc : Operation::dec;
	lgl(iop);

	unsigned long long x = static_cast<unsigned long long>(addr->value);
	long long ac = addr->value > 0 ? 1 : -1;
	long long tt = 0;
	const unsigned long long goodBit = addr->value > 0 ? 1 : 0;
	while(x != 0) {
		unsigned long long bit = x & 1;
		if(bit == goodBit) {
			lgl(Operation::store, addresses[t1]);
			lgl(Operation::add, addresses[addr]);
			lgl(Operation::store, addresses[addr]);
			lgl(Operation::load, addresses[t1]);
			spdlog::debug(".. + {}", ac);
			tt += ac;
		}
		x >>= 1;
		lgl(Operation::shift, addresses[cp1]);
		ac *= 2;
	}
	if(addr->value < 0) {
		lgl(Operation::dec);
		tt--;
	}
	spdlog::debug(tt);
	assert(tt == addr->value);
}