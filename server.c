#include "server.h"
#include "string.h"

#define ACCOUNTS_MAX_LIMIT		255
#define TRANSACTIONS_MAX_LIMIT  255

static ST_accountsDB_t accountsDB[ACCOUNTS_MAX_LIMIT] = {
	{2000.0,RUNNING, "8989374615436851"},
	{100000.0, BLOCKED, "5807007076043875"},
	{1002130.0, RUNNING, "12345678910111213"},
	{1212331.0, RUNNING, "123123123123123123"},
	{864536.0, RUNNING, "456456456456456456"},
	{15782.0, RUNNING, "7897897897897897897"},
	{98745.0, RUNNING, "1471471471471471474"},
	{1896516.0, BLOCKED, "2582582582582582582"},
	{8118965.0, RUNNING, "369369369369369369"},
	{14857894.0, BLOCKED, "95195195195195189"}
};

static ST_transaction_t transactionsDB[TRANSACTIONS_MAX_LIMIT];
	
static uint8_t transactionSeqNumber = 0;

static uint8_t accountRefIndex;

EN_transState_t receiveTransactionData(ST_transaction_t* transData)
{
	ST_accountsDB_t* accountRef =NULL;
	ST_cardData_t cardData = transData->cardHolderData;
	ST_terminalData_t terminalData = transData->terminalData;
	if (isValidAccount(&cardData, accountRef) == ACCOUNT_NOT_FOUND)
	{
		transData->transState = FRAUD_CARD;
		saveTransaction(transData);
		return FRAUD_CARD;
	}
	else
	{
		accountRef = &accountsDB[accountRefIndex];
	}
	if (isBlockedAccount(accountRef) == BLOCKED_ACCOUNT)
	{
		transData->transState = DECLINED_STOLEN_CARD;
		saveTransaction(transData);
		return DECLINED_STOLEN_CARD;
	}
	else if (isAmountAvailable(&terminalData, accountRef) == LOW_BALANCE)
	{
		transData->transState = DECLINED_INSUFFECIENT_FUND;
		saveTransaction(transData);
		return DECLINED_INSUFFECIENT_FUND;
	}
	else if (saveTransaction(transData) == SAVING_FAILED)
	{
		return INTERNAL_SERVER_ERROR;
	}
	//assert(accountRef != NULL);
	if (accountRef != NULL)
	{
		accountRef->balance -= transData->terminalData.transAmount;
	}
	return APPROVED;
}

EN_serverError_t isValidAccount(ST_cardData_t* cardData, ST_accountsDB_t* accountRefrence)
{
	//printf("%s", accountsDB[10].primaryAccountNumber);
	for (uint8_t idx = 0; idx < ACCOUNTS_MAX_LIMIT; idx++)
	{
		if (accountsDB[idx].primaryAccountNumber == "")
		{
			accountRefrence = NULL;
			return ACCOUNT_NOT_FOUND;
		}
		else if (strcmp(accountsDB[idx].primaryAccountNumber, cardData->primaryAccountNumber) == 0)
		{
			accountRefIndex=idx;
			return SERVER_OK;
		}
	}
	accountRefrence = NULL;
	return ACCOUNT_NOT_FOUND;
}

EN_serverError_t isBlockedAccount(ST_accountsDB_t* accountRefrence)
{
	if (accountRefrence->state == BLOCKED)
		return BLOCKED_ACCOUNT;
	return SERVER_OK;
}

EN_serverError_t isAmountAvailable(ST_terminalData_t* termData, ST_accountsDB_t* accountRefrence)
{
	if (termData->transAmount > accountRefrence->balance)
		return LOW_BALANCE;
	return SERVER_OK;
}

EN_serverError_t saveTransaction(ST_transaction_t* transData)
{
	if (transactionSeqNumber < 255)
	{
		transactionsDB[transactionSeqNumber].cardHolderData = transData->cardHolderData;
		transactionsDB[transactionSeqNumber].terminalData = transData->terminalData;
		transactionsDB[transactionSeqNumber].transState = transData->transState;
		transactionsDB[transactionSeqNumber].transactionSequenceNumber = transactionSeqNumber+1;
		transactionSeqNumber++;
	}
	else
		return SAVING_FAILED;
	
	listSavedTransactions();

	return SERVER_OK;
	
	
}

void listSavedTransactions(void)
{
	
	for (uint8_t idx=0; idx < TRANSACTIONS_MAX_LIMIT; idx++)
	{
		if (transactionsDB[idx].transactionSequenceNumber == 0)
			break;
		printf("#########################\n");
		printf("Transaction Sequence Number: %d\n", transactionsDB[idx].transactionSequenceNumber);
		printf("Transaction Date: %s\n", transactionsDB[idx].terminalData.transactionDate);
		printf("Transaction Amount: %f\n", transactionsDB[idx].terminalData.transAmount);
		
		if (transactionsDB[idx].transState == APPROVED)
			printf("Transaction State: APPROVED\n");
		else if(transactionsDB[idx].transState == DECLINED_INSUFFECIENT_FUND)
			printf("Transaction State: DECLINED_INSUFFECIENT_FUND\n");
		else if (transactionsDB[idx].transState == DECLINED_STOLEN_CARD)
			printf("Transaction State: DECLINED_STOLEN_CARD\n");
		else if (transactionsDB[idx].transState == FRAUD_CARD)
			printf("Transaction State: FRAUD_CARD\n");
		else if (transactionsDB[idx].transState == INTERNAL_SERVER_ERROR)
			printf("Transaction State: INTERNAL_SERVER_ERROR\n");

		printf("Terminal Max Amount: %f\n", transactionsDB[idx].terminalData.maxTransAmount);
		printf("Cardholder Name: %s\n", transactionsDB[idx].cardHolderData.cardHolderName);
		printf("PAN: %s\n", transactionsDB[idx].cardHolderData.primaryAccountNumber);
		printf("Card Expiration Date: %s\n", transactionsDB[idx].cardHolderData.cardExpirationDate);
		printf("#########################\n\n");
	}
}

/****************************Testing********************************/

typedef struct {
	char* PAN;
	EN_serverError_t serverError;
}IsValidAccountTestCase;

typedef struct {
	ST_accountsDB_t account;
	EN_serverError_t serverError;
}IsBlockedAccountTestCase;

typedef struct {
	ST_accountsDB_t account;
	float transactionAmount;
	EN_serverError_t serverError;
}IsAmountAvailableTestCase;


typedef struct {
	ST_transaction_t transaction;
	EN_transState_t state;
}ReceiveTransactionTestCase;

typedef struct {
	ST_transaction_t transaction;
	EN_serverError_t serverError;
}SaveTransactionTestCase;

IsValidAccountTestCase validAccountTests[] = {
	{"8989374615436851",SERVER_OK},
	{"951951231795189",FRAUD_CARD},
	{"456456456456456456",SERVER_OK}

};

IsBlockedAccountTestCase blockedAccountTests[] = {
	{{(float32_t)98745.0, RUNNING, "1471471471471471474"},SERVER_OK},
	{{(float32_t)1896516.0, BLOCKED, "2582582582582582582"},BLOCKED_ACCOUNT},
	{{(float32_t)8118965.0, RUNNING, "369369369369369369"},SERVER_OK},
	{{(float32_t)14857894.0, BLOCKED, "95195195195195189"},BLOCKED_ACCOUNT},

};


IsAmountAvailableTestCase amountAvailableTests[] = {
	{{(float32_t)98745.0, RUNNING, "1471471471471471474"},(float32_t)1999.0,SERVER_OK},
	{{(float32_t)10021.6, RUNNING, "2582582582582582582"},(float32_t)20122.0,LOW_BALANCE},
	{{(float32_t)8118965.0, RUNNING, "369369369369369369"},(float32_t)30980.0,SERVER_OK},
	{{(float32_t)1220.7, RUNNING, "95195195195195189"},(float32_t)2500.0,LOW_BALANCE},
};
SaveTransactionTestCase saveTransactionTests[] = {
	{"MOHAMED ASHRAF YOUSSEF","8989374615436851","02/25",1000.0,10000.0,"19/01/2023",APPROVED,1,SERVER_OK},
	{"MENNA MAMDOUH GASSER","12319817249812402","09/23",3200.0,10000.0,"20/07/2023",FRAUD_CARD,2,SERVER_OK},
	{"MOHAMED OSAMA SHAHEEN","5807007076043875","05/25",5000,10000.0,"19/01/2023",DECLINED_STOLEN_CARD,3,SERVER_OK},
	{"AHMED ESSAM ELDIN MOH","8989374615436851","03/24",11000.0,10000.0,"30/07/2023",DECLINED_INSUFFECIENT_FUND,4,SERVER_OK},

};

ReceiveTransactionTestCase receiveTransactionTests[] = {
	{"MOHAMED ASHRAF YOUSSEF","8989374615436851","02/25",1000.0,10000.0,"19/01/2023",APPROVED,1,APPROVED},
	{"MENNA MAMDOUH GASSER","12319817249812402","09/23",3200.0,10000.0,"20/07/2023",FRAUD_CARD,2,FRAUD_CARD},
	{"MOHAMED OSAMA SHAHEEN","5807007076043875","05/25",5000,10000.0,"19/01/2023",DECLINED_STOLEN_CARD,3,DECLINED_STOLEN_CARD},
	{"AHMED ESSAM ELDIN MOH","8989374615436851","03/24",11000.0,10000.0,"30/07/2023",DECLINED_INSUFFECIENT_FUND,4,DECLINED_INSUFFECIENT_FUND},
	
};

void isValidAccountTest(void)
{
	printf("Tester Name: Mohamed Ashraf\n");
	printf("Function Name: isValidAccount\n\n");
	
	for (uint8_t i = 0; i < sizeof(validAccountTests) / sizeof(validAccountTests[0]); i++)
	{
		ST_cardData_t cardData;
		EN_serverError_t serverError;
		ST_accountsDB_t accountRefrence;
		strcpy_s(cardData.primaryAccountNumber, sizeof(cardData.primaryAccountNumber), validAccountTests[i].PAN);

		serverError = isValidAccount(&cardData, &accountRefrence);


		printf("Test Case %d:\n", i + 1);
		printf("Input Data: %s\n", validAccountTests[i].PAN);
		//printf("CARD PAN: %s\n", cardData.primaryAccountNumber);
		if (validAccountTests[i].serverError == SERVER_OK)
			printf("Expected Result: SERVER_OK\n");
		else
			printf("Expected Result: FRAUD_CARD\n");
		if (serverError == SERVER_OK)
			printf("Actual Result: SERVER_OK\n\n");
		else
			printf("Actual Result: FRAUD_CARD\n\n");
	}


}

void isBlockedAccountTest(void)
{
	printf("Tester Name: Mohamed Ashraf\n");
	printf("Function Name: isBlockedAccount\n\n");

	for (uint8_t i = 0; i < sizeof(blockedAccountTests) / sizeof(blockedAccountTests[0]); i++)
	{
		EN_serverError_t serverError;
		ST_accountsDB_t* accountRefrence = &blockedAccountTests[i].account;

		serverError = isBlockedAccount(accountRefrence);


		printf("Test Case %d:\n", i + 1);
		if(blockedAccountTests[i].account.state==SERVER_OK)
			printf("Input Data: RUNNING\n");
		else
			printf("Input Data: BLOCKED\n");
		//printf("CARD PAN: %s\n", cardData.primaryAccountNumber);
		if (blockedAccountTests[i].serverError == SERVER_OK)
			printf("Expected Result: SERVER_OK\n");
		else
			printf("Expected Result: BLOCKED_ACCOUNT\n");
		if (serverError == SERVER_OK)
			printf("Actual Result: SERVER_OK\n\n");
		else
			printf("Actual Result: BLOCKED_ACCOUNT\n\n");
	}

}

void isAmountAvailableTest(void)
{
	printf("Tester Name: Mohamed Ashraf\n");
	printf("Function Name: isAmountAvailable\n\n");

	for (uint8_t i = 0; i < sizeof(amountAvailableTests) / sizeof(amountAvailableTests[0]); i++)
	{
		EN_serverError_t serverError;
		ST_accountsDB_t* accountRefrence = &amountAvailableTests[i].account;
		ST_terminalData_t terminalData;
		terminalData.transAmount = amountAvailableTests[i].transactionAmount;
		serverError = isAmountAvailable(&terminalData,accountRefrence);


		printf("Test Case %d:\n", i + 1);
		printf("Input Data: Transaction Amount=%f\tBalance=%f\n", 
			amountAvailableTests[i].transactionAmount, amountAvailableTests[i].account.balance);
		//printf("CARD PAN: %s\n", cardData.primaryAccountNumber);
		if (amountAvailableTests[i].serverError == SERVER_OK)
			printf("Expected Result: SERVER_OK\n");
		else
			printf("Expected Result: LOW_BALANCE\n");
		if (serverError == SERVER_OK)
			printf("Actual Result: SERVER_OK\n\n");
		else
			printf("Actual Result: LOW_BALANCE\n\n");
	}
}

void saveTransactionTest(void)
{
	printf("Tester Name: Mohamed Ashraf\n");
	printf("Function Name: saveTransaction\n\n");
	EN_serverError_t serverError;
	for (uint8_t i = 0; i < sizeof(saveTransactionTests) / sizeof(saveTransactionTests[0]); i++)
	{
		ST_transaction_t transData = saveTransactionTests[i].transaction;
		serverError=saveTransaction(&transData);
		printf("Test Case %d:\n", i + 1);
		printf("Input Data:CardHolderName: %s\nPAN: %s\nCard Expiry Date:%s\nTerminal Max Transaction Amount: %f\nTerminal Transaction Date: %s\nTerminal Max Transaction Amount:%f\nTransaction Sequence Nubmer:%d\n \n"
			, transData.cardHolderData.cardHolderName, transData.cardHolderData.primaryAccountNumber, transData.cardHolderData.cardExpirationDate

			, transData.terminalData.maxTransAmount, transData.terminalData.transactionDate, transData.terminalData.maxTransAmount,
			transData.transactionSequenceNumber);
		if (saveTransactionTests[i].serverError == SERVER_OK)
			printf("Expected Result: SERVER_OK\n");
		else
			printf("Expected Result: INTERNAL_SERVER_ERROR\n");
		if (serverError == SERVER_OK)
			printf("Actual Result: SERVER_OK\n");
		else
			printf("Actual Result: INTERNAL_SERVER_ERROR\n");
	}

}

void receiveTransactionDataTest(void)
{
	printf("Tester Name: Mohamed Ashraf\n");
	printf("Function Name: receiveTransaction\n\n");
	EN_transState_t state;
	for (uint8_t i = 0; i < sizeof(receiveTransactionTests) / sizeof(receiveTransactionTests[0]); i++)
	{
		ST_transaction_t transData = receiveTransactionTests[i].transaction;
		
		printf("Test Case %d:\n", i + 1);
		printf("Input Data: CardHolderName: %s\nPAN: %s\nCard Expiry Date:%s\nTerminal Max Transaction Amount: %f\nTerminal Transaction Date: %s\nTerminal Transaction Amount:%f\nTransaction Sequence Nubmer:%d\n \n"
			, transData.cardHolderData.cardHolderName, transData.cardHolderData.primaryAccountNumber, transData.cardHolderData.cardExpirationDate

			, transData.terminalData.maxTransAmount, transData.terminalData.transactionDate, transData.terminalData.transAmount,
			transData.transactionSequenceNumber);

		state = receiveTransactionData(&transData);

		if (receiveTransactionTests[i].state == APPROVED)
			printf("Expected Result: APPROVED\n");
		else if(receiveTransactionTests[i].state == DECLINED_INSUFFECIENT_FUND)
			printf("Expected Result: DECLINED_INSUFFECIENT_FUND\n");
		else if(receiveTransactionTests[i].state == DECLINED_STOLEN_CARD)
			printf("Expected Result: DECLINED_STOLEN_CARD\n");
		else if(receiveTransactionTests[i].state == INTERNAL_SERVER_ERROR)
			printf("Expected Result: INTERNAL_SERVER_ERROR\n");
		else if(receiveTransactionTests[i].state == FRAUD_CARD)
			printf("Expected Result: FRAUD_CARD\n");
		
		if (state == APPROVED)
			printf("Actual Result: APPROVED\n\n");
		else if (state == DECLINED_INSUFFECIENT_FUND)
			printf("Actual Result: DECLINED_INSUFFECIENT_FUND\n\n");
		else if (state == DECLINED_STOLEN_CARD)
			printf("Actual Result: DECLINED_STOLEN_CARD\n\n");
		else if (state == FRAUD_CARD)
			printf("Actual Result: FRAUD_CARD\n\n");
		else if (state == INTERNAL_SERVER_ERROR)
			printf("Actual Result: INTERNAL_SERVER_ERROR\n\n");
	}

}

void listSavedTransactionsTest(void)
{
	printf("Tester Name: Mohamed Ashraf\n");
	printf("Function Name: listSavedTransactions\n\n");

	listSavedTransactions();

}