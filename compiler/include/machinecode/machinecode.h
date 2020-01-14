#pragma once
#include <vector>
#include <map>
#include "intercode/address.h"
#include <fstream>

class Machinecode {
	public:
		enum class Operation {
			get,
			put,

			load,
			store,
			load_ind,
			store_ind,

			add,
			sub,
			shift,
			inc,
			dec,

			jump,
			jpos,
			jneg,
			jzero,
			halt,

			label
		};

		void add(Machinecode::Operation, const Address* = nullptr, const Address* = nullptr);
		void legalize();
		void save(std::ofstream&);

		const Address* cp1 = new Address(Address::Type::constant, 1, 1);
		const Address* cn1 = new Address(Address::Type::constant, 1, -1);

		const Address* t1 = new Address(Address::Type::variable, 1);
		const Address* t2 = new Address(Address::Type::variable, 1);
	protected:
		typedef unsigned long long taddr;
		taddr next_addr = 1;
		taddr next_label = 0;

		struct Command {
			Machinecode::Operation op;
			const Address* addr;
			const Address* offset;
		};

		struct LegalCode {
			Machinecode::Operation op;
			taddr addr;
		};

		void lgl(Machinecode::Operation, Machinecode::taddr = 0);

		std::vector<Command> commands;
		std::vector<LegalCode> code;
		
		std::map<const Address*, taddr> labels;
		std::map<const Address*, taddr> addresses;
		std::map<taddr, taddr> label_translation;

		void setAddr(const Address*);
		void setAddresses();
		void generateConst(const Address*);
		void generateConstants();
};