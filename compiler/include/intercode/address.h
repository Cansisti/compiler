#pragma once

class Address {
	public:
		enum class Type {
			constant,
			variable,
			table,
			label
		};

		Address(Address::Type type, size_t size, std::string name = "", long long value = 0) :
			type(type),
			size(size),
			name(name),
			value(value)
		{}
		
		const Type type;
		const size_t size;
 		std::string name;
		long long value;
};