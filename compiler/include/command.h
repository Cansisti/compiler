#pragma once
#include <string>
#include <stdexcept>
#include <spdlog/fmt/ostr.h>

class Command {
	public:
		virtual const std::string describe() const {
			throw std::runtime_error("Describe called on virtual Command");
		}

		template<typename OStream>
		friend OStream &operator<<(OStream &os, const Command& it)
		{
			auto x = it.describe();
			return os << x;
		}
};
