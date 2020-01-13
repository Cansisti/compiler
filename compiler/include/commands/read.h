#pragma once
#include "common/command.h"
#include "common/identifier.h"

class Read: public virtual Command {
	friend class Program;
	public:
		Read(Identifier*);
		virtual const std::string describe() const override;
		virtual void translate(const Program*, Intercode*) const override;
		bool modifies(PId) const;
	protected:
		const Identifier* id;
};