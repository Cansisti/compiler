#pragma once
#include <string>
#include <stdexcept>
#include <spdlog/fmt/ostr.h>
#include "intercode/intercode.h"

class Program;

class Command {
	public:
		virtual const std::string describe() const {
			throw std::runtime_error("Describe called on virtual Command");
		}

		virtual void translate(const Program*, Intercode*) const = 0;

		template<typename OStream>
		friend OStream &operator<<(OStream &os, const Command& it)
		{
			auto x = it.describe();
			return os << x;
		}
};
