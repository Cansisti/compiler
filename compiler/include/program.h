#pragma once
#include <vector>
#include <variant>
#include "declaration.h"
#include "anyCommand.h"

class Program {
	public:

		std::vector<Declaration*> declarations;
		Commands* commands;

		bool validate();
	protected:
		struct CommandValidateVisitor;
		struct ExpressionValidateVisitor;
		struct AnyExpressionValidateVisitor;
		struct ValueValidateVisitor;

		bool checkForPresence(const Identifier* id);
};
