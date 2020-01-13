#pragma once

class Address {
	public:
		enum class Type {
			constant,
			variable,
			table
		};

		Address(Address::Type type, size_t size) :
			type(type),
			size(size)
		{}
		
		const Type type;
		const size_t size;
		long long value;
};