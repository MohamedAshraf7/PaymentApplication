#include "generic.h"


boolean isDigit(uint8_t digit)
{
	if (digit < '0' || digit > '9')
		return FALSE;
	return TRUE;
}

boolean isLetter(uint8_t letter)
{
	if ((letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z'))
		return TRUE;
	return FALSE;
}