#include "machinecode/machinecode.h"
#include <assert.h>
#include <spdlog/spdlog.h>

#define abs(x) (x >= 0 ? x : -x)

void Machinecode::add(Machinecode::Operation op, const Address* addr, const Address* offset) {
	commands.push_back({
		op,
		addr,
		offset
	});
}

void Machinecode::lgl(Machinecode::Operation op, Machinecode::taddr addr, std::string c) {
	code.push_back({
		op,
		addr,
		c
	});
}

void Machinecode::legalize() {
	setAddr(cn1);
	setAddr(cp1);
	setAddr(t1);
	setAddr(t2);
	setAddresses();
	generateConstants();
	for(const auto command: commands) {
		spdlog::debug("'{}' {}", command.addr ? command.addr->name : "-", command.op);
		switch(command.op) {
			case Operation::label: {
				spdlog::debug("label {}:", labels[command.addr]);
				lgl(Operation::label, labels[command.addr]);
				break;
			}
			case Operation::jump:
			case Operation::jpos:
			case Operation::jneg:
			case Operation::jzero: {
				assert(command.addr);
				spdlog::debug("jump ({}) -> {}", command.op, labels[command.addr]);
				lgl(command.op, labels[command.addr]);
				break;
			}
			case Operation::store: {
				assert(command.addr);
				if(not command.offset) {
					lgl(Operation::store, addresses[command.addr], command.addr->name);
				}
				else {
					lgl(Operation::store, addresses[t1], "@t1 - indirect store");
					lgl(Operation::load, addresses[command.addr], command.addr->name);
					lgl(Operation::add, addresses[command.offset], command.offset->name);
					lgl(Operation::store, addresses[t2], "@t2");
					lgl(Operation::load, addresses[t1], "@t1");
					lgl(Operation::store_ind, addresses[t2], "@t2");
				}
				break;
			}
			case Operation::load: {
				if(not command.offset) {
					lgl(Operation::load, addresses[command.addr], command.addr ? command.addr->name : "");
				}
				else {
					lgl(Operation::load, addresses[command.addr], command.addr ? command.addr->name : "");
					lgl(Operation::add, addresses[command.offset]);
					lgl(Operation::load_ind, 0);
					lgl(Operation::load, 0);
				}
				break;
			};
			default: {
				assert(not command.offset);
				lgl(command.op, addresses[command.addr], command.addr ? command.addr->name : "");
			}
		}
	}
	for(size_t i = 0; i<code.size(); i++) {
		if(code[i].op == Operation::label) {
			spdlog::debug("## set label {} to @{}", code[i].addr, i);
			label_translation[code[i].addr] = i;
			code.erase(code.begin() + i);
			i--;
		}
	}
	for(auto& c: code) {
		assert(c.op != Operation::label);
		switch(c.op) {
			case Operation::jump:
			case Operation::jpos:
			case Operation::jneg:
			case Operation::jzero: {
				spdlog::debug("jump ({}) -> {}", c.op, label_translation[c.addr]);
				c.addr = label_translation.at(c.addr);
				break;
			}
			default: break;
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
			spdlog::debug("## put label {} at {}", command.addr->name, next_label-1);
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
	spdlog::info("Generating constants...");
	generateConst(addresses[cp1], cp1->value);
	generateConst(addresses[cn1], cn1->value);

	std::vector<std::pair<taddr, long long>> constants;
	for(auto addr: addresses) {
		if(addr.first == cp1 or addr.first == cn1) continue;
		if(addr.first->type == Address::Type::constant) {
			constants.push_back({addr.second, addr.first->value});
		}
		if(addr.first->type == Address::Type::table) {
			constants.push_back({addr.second, addr.first->value + addr.second});
		}
	}
	std::sort(constants.begin(), constants.end());
	for(auto constant: constants) {
		generateConst(constant.first, constant.second);
	}
	spdlog::info("Generating constants done");
}

void Machinecode::generateConst(Machinecode::taddr addr, long long value) {
	assert(addr);
	spdlog::info("Generating const {}", value);

	lgl(Operation::sub, 0, "start generation of " + std::to_string(value));
	lgl(Operation::store, addr, "@val");
	if(value == 0) {
		return;
	}
	auto iop = value > 0 ? Operation::inc : Operation::dec;
	lgl(iop);

	if(abs(value) == 1) {
		lgl(Operation::store, addr, "@c_1");
		return;
	}

	unsigned long long x = static_cast<unsigned long long>(value);
	long long ac = value > 0 ? 1 : -1;
	long long tt = 0;
	const unsigned long long goodBit = value > 0 ? 1 : 0;
	do {
		unsigned long long bit = x & 1;
		if(bit == goodBit) {
			lgl(Operation::store, addresses[t1], "@t1");
			lgl(Operation::add, addr, "@val");
			lgl(Operation::store, addr, "@val");
			lgl(Operation::load, addresses[t1], "@t1");
			spdlog::debug(".. + {}", ac);
			tt += ac;
		}
		x >>= 1;
		lgl(Operation::shift, addresses[cp1], "@cp1");
		ac *= 2;
	} while(x != 0);
	if(value < 0) {
		lgl(Operation::load, addr, "@val decrement, cause val is negative");
		lgl(Operation::dec);
		lgl(Operation::store, addr, "@val store val");
		tt--;
	}
	spdlog::debug(tt);
	assert(tt == value);
}

void Machinecode::save(std::ofstream& file) {
	assert(file.is_open());
	for(auto c: code) {
		switch(c.op) {
			case Operation::label: assert(false);
			case Operation::get: {
				file << "GET  \t";
				break;
			}
			case Operation::put: {
				file << "PUT  \t";
				break;
			}
			case Operation::load: {
				file << "LOAD \t" << c.addr;
				break;
			}
			case Operation::store: {
				file << "STORE\t" << c.addr;
				break;
			}
			case Operation::load_ind: {
				file << "LOADI\t" << c.addr;
				break;
			}
			case Operation::store_ind: {
				file << "STOREI\t" << c.addr;
				break;
			}
			case Operation::add: {
				file << "ADD  \t" << c.addr;
				break;
			}
			case Operation::sub: {
				file << "SUB  \t" << c.addr;
				break;
			}
			case Operation::shift: {
				file << "SHIFT\t" << c.addr;
				break;
			}
			case Operation::inc: {
				file << "INC  \t";
				break;
			}
			case Operation::dec: {
				file << "DEC  \t";
				break;
			}
			case Operation::jump: {
				file << "JUMP \t" << c.addr;
				break;
			}
			case Operation::jpos: {
				file << "JPOS \t" << c.addr;
				break;
			}
			case Operation::jneg: {
				file << "JNEG \t" << c.addr;
				break;
			}
			case Operation::jzero: {
				file << "JZERO\t" << c.addr;
				break;
			}
			case Operation::halt: {
				file << "HALT \t";
				break;
			}
		}
		if(c.comment != "") {
			file << "\t# " << c.comment;
		}
		file << std::endl;
	}
};