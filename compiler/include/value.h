#pragma once
#include <variant>

#include "identifier.h"

typedef std::variant<Identifier*, Num> Value;

struct ValueVisitor {
	const Num operator()(const Num& num) const {
		return num;
	}

	const Identifier* operator()(const Identifier* id) const {
		return id;
	}
};