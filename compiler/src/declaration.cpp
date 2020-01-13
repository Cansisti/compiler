#include "common/declaration.h"

size_t Declaration::next_address = 1;

Declaration::Declaration() :
	address(next_address++)
{

}
