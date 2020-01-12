#pragma once
#include <variant>
#include <vector>
#include <assert.h>

#include "commands/assign.h"
#include "commands/forLoop.h"
#include "commands/read.h"
#include "commands/write.h"
#include "commands/conditionalLoop.h"

class NotACommand: public virtual Command {
	const std::string describe() const override {
		return "nac";
	}
};

typedef std::variant<
	Assign*,
	ForLoop*,
	Read*,
	Write*,
	ConditionalLoop*,
	NotACommand*
> AnyCommand;

typedef std::vector<AnyCommand*> Commands;

struct AnyCommandVisitor {
	const Command* operator()(const Command* command) const {
		return command;
	}
};
