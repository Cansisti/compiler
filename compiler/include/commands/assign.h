#pragma once
#include "command.h"
#include "identifier.h"
#include "expression.h"

class Assign: public virtual Command {
	friend class Program;
	public:
		Assign(Identifier*, Expression*);
		virtual const std::string describe() const override;
	protected:
		const Identifier* id;
		const Expression* expr;
};