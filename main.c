#include "card.h"
#include "terminal.h"
#include "server.h"
#include "app.h"

#define TESTING 1

int main()
{
#if TESTING
	FILE* stdin_val = stdin;
	freopen_s(&stdin_val, "tests.txt", "wb+", stdin);
	if (stdin_val != 0) {
		
		getCardHolderNameTest();
		
		//getCardExpiryDateTest();

		//getCardPANTest();

		fclose(stdin_val);
	}
	else 
		printf("ERROR hijacking stdin");
#else
	appStart();
#endif

	return 0;
}