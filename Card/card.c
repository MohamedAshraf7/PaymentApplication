#include "card.h"
#include <string.h>
#include <stdlib.h>

EN_cardError_t getCardHolderName(ST_cardData_t* cardData)
{
	char name[MAX_LIMIT_NAME_LENGTH + 2];

	printf("Please enter your card holder Name\n");

	fgets(name, MAX_LIMIT_NAME_LENGTH + 2, stdin);
	//to remove the new line from the string entered by user
	name[strcspn(name, "\n")] = 0;

	for (uint8_t idx = 0; idx < strlen(name); idx++)
	{
		if (isLetter(name[idx]) || name[idx] == ' ')
			continue;
		else
		{
			cardData->cardHolderName[0] = '\0';
			return WRONG_NAME;
		}
	}
	//printf("%llu", strlen(name));
	if (strlen(name) < MIN_LIMIT_NAME_LENGTH || (strlen(name) > MAX_LIMIT_NAME_LENGTH))
	{
		cardData->cardHolderName[0] = '\0';
		return WRONG_NAME;
	}
	else
	{
		strcpy_s(cardData->cardHolderName, sizeof(cardData->cardHolderName), name);
		return CARD_OK;
	}
}
EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData)
{
	char exp[EXPDATE_LENGTH + 2];
	printf("Please enter your card expiry date\n");
	fgets(exp, EXPDATE_LENGTH + 2, stdin);
	//to remove the new line from the string entered by user
	exp[strcspn(exp, "\n")] = 0;

	if (strlen(exp) != EXPDATE_LENGTH || atoi(exp) > 12 || atoi(exp) == 0)
	{
		cardData->cardExpirationDate[0] = '\0';
		return WRONG_EXP_DATE;
	}
	for (uint8_t i = 0; i < EXPDATE_LENGTH; i++)
	{
		if (i == 2 && exp[i] != '/')
		{
			cardData->cardExpirationDate[0] = '\0';
			return WRONG_EXP_DATE;
		}
		else if (i == 2 && exp[i] == '/')
			continue;
		else if (!isDigit(exp[i]))
		{
			cardData->cardExpirationDate[0] = '\0';
			return WRONG_EXP_DATE;
		}
	}
	strcpy_s(cardData->cardExpirationDate, sizeof(cardData->cardExpirationDate), exp);
	return CARD_OK;

}
EN_cardError_t getCardPAN(ST_cardData_t* cardData)
{
	char PAN[MAX_LIMIT_PAN_LENGTH + 2];
	printf("Please enter your card PAN\n");
	fgets(PAN, MAX_LIMIT_PAN_LENGTH + 2, stdin);

	//printf("%llu", strlen(PAN));

	//to remove the new line from the string entered by user
	PAN[strcspn(PAN, "\n")] = 0;

	for (uint8_t i = 0; i < strlen(PAN); i++)
	{
		if (!isDigit(PAN[i]))
		{
			cardData->primaryAccountNumber[0] = '\0';
			return WRONG_PAN;
		}
	}
	if (strlen(PAN) < MIN_LIMIT_PAN_LENGTH || (strlen(PAN) > MAX_LIMIT_PAN_LENGTH))
	{
		cardData->primaryAccountNumber[0] = '\0';
		return WRONG_PAN;
	}
	else
	{
		strcpy_s(cardData->primaryAccountNumber, sizeof(cardData->primaryAccountNumber), PAN);
		return CARD_OK;
	}
}

/**********************Card Testing*******************************/
typedef struct {
	char* name;
	EN_cardError_t cardError;
} GetCardHolderNameTestCase;

typedef struct {
	char* expDate;
	EN_cardError_t cardError;
} GetCardExpiryDateTestCase;

typedef struct {
	char* PAN;
	EN_cardError_t cardError;
}GetCardPANTestCase;


GetCardHolderNameTestCase nameTests[] = {
	{"Mohamed Ashraf\n", WRONG_NAME},
	{"Mohamed Ashraf Youssef\n", CARD_OK},
	{"AHMED MOHAMED ABDELAAL IBRAHIM\n",WRONG_NAME},
	{"MENNA MOHAMED SHAHIN\n",CARD_OK},
	{"AHMED MOHAMED EID SHAHIN\n",CARD_OK},
	{"AHMED MOHAMED ABDELAAL 123\n",WRONG_NAME}
};


GetCardExpiryDateTestCase expDateTests[] = {
	{"1/25",WRONG_EXP_DATE},
	{"13/23",WRONG_EXP_DATE},
	{"11/22",CARD_OK},
	{"12125",WRONG_EXP_DATE},
	{"12/2512",WRONG_EXP_DATE},
	{"12/1/",WRONG_EXP_DATE},
	{"12/a1",WRONG_EXP_DATE}
};


GetCardPANTestCase PANTests[] = {
	{"123123123",WRONG_PAN},
	{"95195195195195189",CARD_OK},
	{"123123123123123123",CARD_OK},
	{"asdjh1232asdahjsdg1",WRONG_PAN},
	{"123183123//12312312",WRONG_PAN},
	{"1231231231298313122",CARD_OK},
};

void getCardHolderNameTest(void)
{
	printf("Tester Name: Mohamed Ashraf\n");
	printf("Function Name: getCardHolderName\n\n");
	for (uint8_t i = 0; i < sizeof(nameTests) / sizeof(nameTests[0]); i++)
	{
		ST_cardData_t cardData;
		EN_cardError_t cardError;
		uint32_t position = ftell(stdin);
		fputs(nameTests[i].name, stdin);
		uint32_t last_position = ftell(stdin);
		fseek(stdin, position, SEEK_SET);
		cardError = getCardHolderName(&cardData);
		fseek(stdin, last_position, SEEK_SET);
		printf("Test Case %d:\n", i + 1);
		printf("Input Data: %s", nameTests[i].name);
		printf("Card Holder Name: %s\n", cardData.cardHolderName);
		if (nameTests[i].cardError == CARD_OK)
			printf("Expected Result: CARD_OK\n");
		else
			printf("Expected Result: WRONG_NAME\n");
		if (cardError == CARD_OK)
			printf("Actual Result: CARD_OK\n\n");
		else
			printf("Actual Result: WRONG_NAME\n\n");
	}
}

void getCardExpiryDateTest(void)
{
	printf("Tester Name: Mohamed Ashraf\n");
	printf("Function Name: getCardExpiryDate\n\n");
	for (uint8_t i = 0; i < sizeof(expDateTests) / sizeof(expDateTests[0]); i++)
	{
		ST_cardData_t cardData;
		EN_cardError_t cardError;
		uint32_t position = ftell(stdin);
		fputs(expDateTests[i].expDate, stdin);
		uint32_t last_position = ftell(stdin);
		fseek(stdin, position, SEEK_SET);
		cardError = getCardExpiryDate(&cardData);
		fseek(stdin, last_position, SEEK_SET);
		printf("Test Case %d:\n", i + 1);
		printf("Input Data: %s\n", expDateTests[i].expDate);
		if (expDateTests[i].cardError == CARD_OK)
			printf("Expected Result: CARD_OK\n");
		else
			printf("Expected Result: WRONG_EXP_DATE\n");
		if (cardError == CARD_OK)
			printf("Actual Result: CARD_OK\n\n");
		else
			printf("Actual Result: WRONG_EXP_DATE\n\n");
	}
}

void getCardPANTest(void)
{
	printf("Tester Name: Mohamed Ashraf\n");
	printf("Function Name: getCardPAN\n\n");
	for (uint8_t i = 0; i < sizeof(PANTests) / sizeof(PANTests[0]); i++)
	{
		ST_cardData_t cardData;
		EN_cardError_t cardError;
		uint32_t position = ftell(stdin);
		fputs(PANTests[i].PAN, stdin);
		uint32_t last_position = ftell(stdin);
		fseek(stdin, position, SEEK_SET);
		cardError = getCardPAN(&cardData);
		fseek(stdin, last_position, SEEK_SET);

		printf("Test Case %d:\n", i + 1);
		printf("Input Data: %s\n", PANTests[i].PAN);
		if (PANTests[i].cardError == CARD_OK)
			printf("Expected Result: CARD_OK\n");
		else
			printf("Expected Result: WRONG_PAN\n");
		if (cardError == CARD_OK)
			printf("Actual Result: CARD_OK\n\n");
		else
			printf("Actual Result: WRONG_PAN\n\n");
	}
}