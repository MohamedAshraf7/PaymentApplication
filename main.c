#include "Application/app.h"

#define TESTING 0

int main()
{
#if TESTING
	FILE* stdin_val = stdin;
	freopen_s(&stdin_val, "tests.txt", "wb+", stdin);
	if (stdin_val != 0) {
		
		//receiveTransactionDataTest();

		//isValidAccountTest();

		//isBlockedAccountTest();
		
		//isAmountAvailableTest();

		saveTransactionTest();

		//listSavedTransactionsTest();

		fclose(stdin_val);
	}
	else
		printf("ERROR hijacking stdin");
#else
	appStart();
#endif

	return 0;
}