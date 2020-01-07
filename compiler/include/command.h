#pragma once
#include "spdlog/fmt/ostr.h"

class Command {
	public:
		virtual void translate() = 0;
};
