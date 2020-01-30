#pragma once
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <functional>

#include "intercode/address.h"
#include "machinecode/machinecode.h"

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
		
		void declare(size_t, Address::Type, std::string, size_t = 1, long long offset = 0);
		size_t constant( long long);

		void add(Intercode::Operation, size_t = not_an_addr, size_t = not_an_addr, size_t = not_an_addr);
		size_t generateLabel(std::string = "");
		void putLabel(size_t l);

		void translate(Machinecode*);

		void save(std::ofstream&);
	protected:
		size_t next_label = -10;

		struct Command {
			Intercode::Operation op;
			Address* a0 = nullptr;
			Address* a1 = nullptr;
			Address* a2 = nullptr;
		};

		std::map<size_t, Address*> vars;
		std::vector<Command> commands;

		Address* rt = new Address(Address::Type::variable, 1, "*rt");
		Address* r0 = new Address(Address::Type::variable, 1, "*r0");
		Address* r1 = new Address(Address::Type::variable, 1, "*r1");
		Address* r2 = new Address(Address::Type::variable, 1, "*r2");
		Address* r3 = new Address(Address::Type::variable, 1, "*r3");
		Address* r4 = new Address(Address::Type::variable, 1, "*r4");
		Address* r5 = new Address(Address::Type::variable, 1, "*r5");
		Address* r6 = new Address(Address::Type::variable, 1, "*r6");
		Address* acc = new Address(Address::Type::variable, 1, "*acc");

		void translateMul(Machinecode*, Address*, Address*, Address*);
		void translateMul_v2(Machinecode*, Address*, Address*, Address*);
		void translateDiv(Machinecode*, Address*, Address*, Address*);
		void translateDiv_v2(Machinecode*, Address*, Address*, Address*);

		/**
		 * Leaves in r5:
		 *  0 - if sign is different
		 *  1 - if both are positive
		 * -1 - if both are negative
		 **/
		void theTrickOfSign(Machinecode*, Address*, Address*);

		void getRidOfSign(Machinecode*, Address*);

		/**
		 * Factorize num into power of to and remainder.
		 * Uses r0.
		 * Does not modify num.
		 * Sets power_of_to.
		 * Afterwards, num = 2^power_of_to - remainder.
		 * Remainder is left at p0.
		 **/
		void factorize(Machinecode*, Address* num, Address* power_of_to);

		void loadPerformStore(Machinecode*, Machinecode::Operation, Address*);
		void magic(
			Machinecode* code,
			Machinecode::Operation exceeded_policy,
			Machinecode::Operation exceed_operation,
			Machinecode::Operation acc_operation,
			std::function<void()> on_remainder_callback = [](){}
		);
};