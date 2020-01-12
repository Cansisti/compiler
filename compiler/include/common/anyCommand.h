#pragma once
#include <variant>
#include <vector>
#include <assert.h>

#include "commands/assign.h"
#include "commands/forLoop.h"
#include "commands/read.h"
#include "commands/write.h"
#include "commands/conditionalLoop.h"
#include "commands/ifStatement.h"

typedef std::variant<
	Assign*,
	ForLoop*,
	Read*,
	Write*,
	ConditionalLoop*,
	IfStatement*
> AnyCommand;

typedef std::vector<AnyCommand*> Commands;

struct AnyCommandVisitor {
	const Command* operator()(const Command* command) const {
		return command;
	}
};
