#pragma once
#include <variant>
#include <vector>
#include <assert.h>

#include "commands/assign.h"

typedef std::variant<
	Assign*,
	void*
> AnyCommand;

typedef std::vector<AnyCommand*> Commands;

struct AnyCommandVisitor {
	const Command* operator()(const Command* command) const {
		return command;
	}

	template<class T>
	const Command* operator()(const T t) {
		// todo
		// assert(false);
		return nullptr;
	}
};
