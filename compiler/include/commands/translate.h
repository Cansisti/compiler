#pragma once
#include "common/program.h"

#define TranslateVisitorConstructor(name) const Program* program; Intercode* code; name(const Program* program, Intercode* code): program(program), code(code) {}

struct Addresses {
	size_t a0 = Intercode::not_an_addr;
	size_t a1 = Intercode::not_an_addr;
	size_t a2 = Intercode::not_an_addr;
};

struct IdentifierTranslateVisitor {
	TranslateVisitorConstructor(IdentifierTranslateVisitor)

	Addresses operator()(const ConstantTableIdentifier& id) {
		auto addr = Declaration::next_address++;
		code->constant(addr, id.n);
		return {
			program->findDeclaration(id.name)->address,
			addr
		};
	}

	Addresses operator()(const LabeledTableIdentifier& id) {
		return {
			program->findDeclaration(id.name)->address,
			program->findDeclaration(id.p)->address
		};
	}

	Addresses operator()(const VariableIdentifier& id) {
		return { program->findDeclaration(id.name)->address };
	}
};

struct ValueTranslateVisitor {
	TranslateVisitorConstructor(ValueTranslateVisitor)

	Addresses operator()(const Identifier* id) {
		return std::visit(IdentifierTranslateVisitor(program, code), *id);
	}

	Addresses operator()(const Num& num) {
		auto addr = Declaration::next_address++;
		code->constant(addr, num);
		return { addr };
	}
};