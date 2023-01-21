#include "terminal.h"
#include "string.h"
#include <stdlib.h>

EN_terminalError_t getTransactionDate(ST_terminalData_t* termData)
{
	uint8_t transDate[DATE_LENGTH+2];
	printf("Please Enter the transaction date\n");
	fgets(transDate, DATE_LENGTH+2, stdin);

	//to remove the new line from the string entered by user
	transDate[strcspn(transDate, "\n")] = 0;
	uint8_t month = atoi(transDate + 3);
	uint8_t day = atoi(transDate);
	if (strlen(transDate) != DATE_LENGTH)
	{
		termData->transactionDate[0] = '\0';
		return WRONG_DATE;
	}
	else if (day > 31|| day==0 || month > 12||month==0)
	{
		termData->transactionDate[0] = '\0';
		return WRONG_DATE;
	}

	for (uint8_t i = 0; i < DATE_LENGTH; i++)
	{
		if ((i == 2 || i == 5) && transDate[i] != '/')
		{
			termData->transactionDate[0] = '\0';
			return WRONG_DATE;
		}
		else if ((i == 2 || i == 5) && transDate[i] == '/')
			continue;
		else if (!isDigit(transDate[i]))
		{
			termData->transactionDate[0] = '\0';
			return WRONG_DATE;
		}
	}
	strcpy_s(termData->transactionDate, sizeof(termData->transactionDate), transDate);
	return TERMINAL_OK;
}
EN_terminalError_t isCardExpired(ST_cardData_t* cardData, ST_terminalData_t* termData)
{
	uint8_t cardExpDateMonth = atoi(cardData->cardExpirationDate);
	uint8_t cardExpDateYear = atoi(cardData->cardExpirationDate + 3) + 2000;

	uint8_t transactionDateMonth = atoi(termData->transactionDate + 3);
	uint8_t transactionDateYear = atoi(termData->transactionDate + 6);
	//printf("%d %d %d %d", cardExpDateMonth, cardExpDateYear, transactionDateMonth, transactionDateYear);

	if (transactionDateYear > cardExpDateYear)
		return EXPIRED_CARD;
	else if (transactionDateYear == cardExpDateYear)
	{
		if (transactionDateMonth > cardExpDateMonth)
			return EXPIRED_CARD;
	}
	return TERMINAL_OK;
}
EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData)
{
	float32_t transAmount;
	printf("Please Enter the transaction amount\n");
	scanf_s("%f", &transAmount);

	if (transAmount <= 0)
		return INVALID_AMOUNT;

	termData->transAmount = transAmount;
	return TERMINAL_OK;
}
EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData)
{
	return termData->transAmount > termData->maxTransAmount ? EXCEED_MAX_AMOUNT : TERMINAL_OK;
}
EN_terminalError_t setMaxAmount(ST_terminalData_t* termData, float maxAmount)
{
	if (maxAmount <= 0)
		return INVALID_MAX_AMOUNT;

	termData->maxTransAmount = maxAmount;
	return TERMINAL_OK;
}
//EN_terminalError_t isValidCardPAN(ST_cardData_t* cardData) // Optional
//{
//
//}

/*******************************TESTING*******************************/
typedef struct {
	char* date;
	EN_terminalError_t terminalError;
}GetTransactionDateTestCase;

typedef struct {
	char* cardExpirationDate;
	char* transactionDate;
	EN_terminalError_t terminalError;
}IsCardExpiredTestCase;

typedef struct {
	float transactionAmount;
	EN_terminalError_t terminalError;
}GetTransactionAmountTestCase;

typedef struct {
	float transactionAmount;
	float MaxAmount;
	EN_terminalError_t terminalError;
}IsBelowMaxAmountTestCase;

typedef struct {
	float maxAmount;
	EN_terminalError_t terminalError;
}SetMaxAmountTestCase;


GetTransactionDateTestCase transDateTests[] = {
	{"13/01/2022",TERMINAL_OK},
	{"13/20/2022",WRONG_DATE},
	{"55/01/2022",WRONG_DATE},
	{"13/01/20221231",WRONG_DATE},
	{"2101/202/2",WRONG_DATE},
	{"1701/2022",WRONG_DATE},
	{"15/01/",WRONG_DATE},
	{"17/09/2021",TERMINAL_OK},
};

IsCardExpiredTestCase cardExpiredTests[] = {
	{"02/22","13/01/2021",TERMINAL_OK},
	{"03/21","10/07/2022",EXPIRED_CARD},
	{"02/22","01/01/2022", TERMINAL_OK},
	{"07/22","15/08/2022", EXPIRED_CARD},
};



GetTransactionAmountTestCase transactionAmountTests[] = {
	{1233.123,TERMINAL_OK},
	{0,INVALID_AMOUNT},
	{123124,TERMINAL_OK},
	{-21428,INVALID_AMOUNT},
	{-12313.2,INVALID_AMOUNT},
};


IsBelowMaxAmountTestCase belowMaxAmountTests[] = {
	{123132.1,123314124.212,TERMINAL_OK},
	{1231,780,EXCEED_MAX_AMOUNT},
	{12313,123198312.2,TERMINAL_OK}
};

SetMaxAmountTestCase maxAmountTests[] = {
	{1231312.2,TERMINAL_OK},
	{0,INVALID_MAX_AMOUNT},
	{-1829131.1231,INVALID_MAX_AMOUNT}
};

void getTransactionDateTest(void)
{
	printf("Tester Name: Mohamed Ashraf\n");
	printf("Function Name: getTransactionDate\n");

	for (uint8_t i = 0; i < sizeof(transDateTests) / sizeof(transDateTests[0]); i++)
	{
		ST_terminalData_t terminalData;
		EN_terminalError_t terminalError;
		
		uint32_t position = ftell(stdin);
		fputs(transDateTests[i].date, stdin);
		uint32_t last_position = ftell(stdin);
		fseek(stdin, position, SEEK_SET);
		terminalError = getTransactionDate(&terminalData);
		fseek(stdin, last_position, SEEK_SET);

		printf("Test Case %d:\n", i + 1);
		printf("Input Data: %s\n", transDateTests[i].date);
		if (transDateTests[i].terminalError == TERMINAL_OK)
			printf("Expected Result: TERMINAL_OK\n");
		else
			printf("Expected Result: WRONG_DATE\n");
		if (terminalError == TERMINAL_OK)
			printf("Actual Result: TERMINAL_OK\n\n");
		else
			printf("Actual Result: WRONG_DATE\n\n");
	}
}

void isCardExpriedTest(void)
{
	printf("Tester Name: Mohamed Ashraf\n");
	printf("Function Name: isCardExpried\n\n");
	for (uint8_t i = 0; i < sizeof(cardExpiredTests) / sizeof(cardExpiredTests[0]); i++)
	{
		ST_terminalData_t terminalData;
		ST_cardData_t cardData;
		EN_terminalError_t terminalError;
		strcpy_s(terminalData.transactionDate, sizeof(terminalData.transactionDate), cardExpiredTests[i].transactionDate);
		strcpy_s(cardData.cardExpirationDate, sizeof(cardData.cardExpirationDate), cardExpiredTests[i].cardExpirationDate);

		terminalError = isCardExpired(&cardData, &terminalData);

		printf("Test Case %d:\n", i + 1);
		printf("Input Data: Expiry Date: %s\tTransaction Date: %s\n", cardExpiredTests[i].cardExpirationDate, cardExpiredTests[i].transactionDate);
		if (cardExpiredTests[i].terminalError == TERMINAL_OK)
			printf("Expected Result: TERMINAL_OK\n");
		else
			printf("Expected Result: EXPIRED_CARD\n");
		if (terminalError == TERMINAL_OK)
			printf("Actual Result: TERMINAL_OK\n\n");
		else
			printf("Actual Result: EXPIRED_CARD\n\n");
	}

}



void getTransactionAmountTest(void)
{
	printf("Tester Name: Mohamed Ashraf\n");
	printf("Function Name: getTransactionAmount\n\n");

	for (uint8_t i = 0; i < sizeof(transactionAmountTests) / sizeof(transactionAmountTests[0]); i++)
	{
		ST_terminalData_t terminalData;
		EN_terminalError_t terminalError;

		uint32_t position = ftell(stdin);
		//fputs(transactionAmountTests[i].transactionAmount, stdin);
		fprintf(stdin, "%f", transactionAmountTests[i].transactionAmount);
		uint32_t last_position = ftell(stdin);
		fseek(stdin, position, SEEK_SET);
		terminalError = getTransactionAmount(&terminalData);
		fseek(stdin, last_position, SEEK_SET);

		printf("Test Case %d:\n", i + 1);
		printf("Input Data: %f\n", transactionAmountTests[i].transactionAmount);
		if (transactionAmountTests[i].terminalError == TERMINAL_OK)
			printf("Expected Result: TERMINAL_OK\n");
		else
			printf("Expected Result: INVALID_AMOUNT\n");
		if (terminalError == TERMINAL_OK)
			printf("Actual Result: TERMINAL_OK\n\n");
		else
			printf("Actual Result: INVALID_AMOUNT\n\n");
	}
}


void isBelowMaxAmountTest(void)
{
	printf("Tester Name: Mohamed Ashraf\n");
	printf("Function Name: isBelowMaxAmount\n\n");
	for (uint8_t i = 0; i < sizeof(belowMaxAmountTests) / sizeof(belowMaxAmountTests[0]); i++)
	{
		ST_terminalData_t terminalData;
		EN_terminalError_t terminalError;
		terminalData.transAmount = belowMaxAmountTests[i].transactionAmount;
		terminalData.maxTransAmount = belowMaxAmountTests[i].MaxAmount;

		terminalError = isBelowMaxAmount( &terminalData);

		printf("Test Case %d:\n", i + 1);
		printf("Input Data: Transaction Amount: %f\tMaximum Amount: %f\n", belowMaxAmountTests[i].transactionAmount, belowMaxAmountTests[i].MaxAmount);
		if (belowMaxAmountTests[i].terminalError == TERMINAL_OK)
			printf("Expected Result: TERMINAL_OK\n");
		else
			printf("Expected Result: EXCEED_MAX_AMOUNT \n");
		if (terminalError == TERMINAL_OK)
			printf("Actual Result: TERMINAL_OK\n\n");
		else
			printf("Actual Result: EXCEED_MAX_AMOUNT\n\n");
	}
}

void setMaxAmountTest(void)
{
	printf("Tester Name: Mohamed Ashraf\n");
	printf("Function Name: setMaxAmount\n\n");
	for (uint8_t i = 0; i < sizeof(maxAmountTests) / sizeof(maxAmountTests[0]); i++)
	{
		ST_terminalData_t terminalData;
		EN_terminalError_t terminalError;
		terminalError = setMaxAmount(&terminalData,maxAmountTests[i].maxAmount);

		printf("Test Case %d:\n", i + 1);
		printf("Input Data: %f\n", maxAmountTests[i].maxAmount);
		if (maxAmountTests[i].terminalError == TERMINAL_OK)
			printf("Expected Result: TERMINAL_OK\n");
		else
			printf("Expected Result: INVALID_MAX_AMOUNT \n");
		if (terminalError == TERMINAL_OK)
			printf("Actual Result: TERMINAL_OK\n\n");
		else
			printf("Actual Result: INVALID_MAX_AMOUNT\n\n");
	}
}
