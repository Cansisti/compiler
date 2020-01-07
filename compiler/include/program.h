#pragma once
#include <vector>
#include "command.h"
#include "declaration.h"

class Program {
	public:
		std::vector<Declaration*> declarations;
		std::vector<Command*> commands;
};
