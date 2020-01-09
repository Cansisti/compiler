#pragma once
#include <vector>
#include <variant>
#include "declaration.h"
#include "commands/assign.h"

class Program {
	public:
		typedef std::variant<
			Assign
		> AnyCommand;

		struct AnyCommandVisitor {
			const Command& operator()(const Command& command) const {
				return command;
			}
		};

		std::vector<Declaration*> declarations;
		std::vector<AnyCommand> commands;

		bool validate();
	protected:
		struct CommandValidateVisitor;
		struct AnyValidateVisitor;
		struct ExpressionValidateVisitor;
		struct ValueValidateVisitor;

		bool checkForPresence(const Identifier* id);
};
