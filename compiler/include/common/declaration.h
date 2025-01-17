#pragma once
#include <string>
#include <spdlog/fmt/ostr.h>
#include "anything.h"

class Declaration {
	public:
		PId id;
		size_t line;

		const size_t address;
		static size_t next_address;

		virtual const int type() const = 0;
		virtual const std::string type_string() const = 0;
		virtual bool validate() const = 0;

		template<typename OStream>
		friend OStream &operator<<(OStream &os, const Declaration* declaration)
		{
			return os << declaration->type_string() << " '" << declaration->id << "' at " << declaration->line;
		}

		bool initiated;
		Declaration();
};
