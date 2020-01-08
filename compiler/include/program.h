#pragma once
#include <vector>
#include <variant>
#include "declaration.h"
#include "commands/assign.h"

class Program {
	public:
		typedef std::variant<
			Assign*
		> AnyCommand;
		
		struct AnyCommandVisitor {
			// why they can't be const for spdlog?
			Command* operator()(Command* command) const {
				return command;
			}
		};

		std::vector<Declaration*> declarations;
		std::vector<AnyCommand> commands;

		bool validate();
	protected:
		struct CommandValidateVisitor;
};
