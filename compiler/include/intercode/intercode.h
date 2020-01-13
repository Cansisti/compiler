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
			jump_zero
		};

		Intercode();
		void declare(size_t, Address::Type, size_t = 1);
		void constant(size_t, long long);
		void add(Intercode::Operation, size_t = 0, size_t = 0, size_t = 0);
	protected:
		struct Command {
			Operation op;
			Address* a0 = nullptr;
			Address* a1 = nullptr;
			Address* a2 = nullptr;
		};

		std::map<size_t, Address*> vars;
		std::vector<Command> commands;
};