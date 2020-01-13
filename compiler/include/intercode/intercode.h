#pragma once
#include <vector>
#include <map>
#include <string>

#include "intercode/address.h"

class Intercode {
	public:
		enum class Operation {
			assign,
			get,
			put,

			add,
			sub,
			mul,
			div,
			mod,

			inc,
			dec,

			jump,
			jump_pos,
			jump_neg,
			jump_zero,

			remember,
			label
		};

		static const size_t not_an_addr;
		static const size_t temp_addr;

		Intercode();
		
		void declare(size_t, Address::Type, size_t = 1);
		size_t constant( long long);

		void add(Intercode::Operation, size_t = not_an_addr, size_t = not_an_addr, size_t = not_an_addr);
		size_t generateLabel();
		void putLabel(size_t l);
	protected:
		size_t next_label = 0;

		struct Command {
			Operation op;
			Address* a0 = nullptr;
			Address* a1 = nullptr;
			Address* a2 = nullptr;
		};

		std::map<size_t, Address*> vars;
		std::vector<Command> commands;
};