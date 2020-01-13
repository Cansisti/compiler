#pragma once
#include "common/command.h"
#include "common/identifier.h"
#include "common/expression.h"

class Assign: public virtual Command {
	friend class Program;
	public:
		Assign(Identifier*, Expression*);
		virtual const std::string describe() const override;
		virtual void translate(const Program*, Intercode*) const override;
	protected:
		const Identifier* id;
		const Expression* expr;
};