#include "app.h"

void appStart(void)
{
	ST_cardData_t cardData;
	ST_terminalData_t terminalData;
	ST_transaction_t transactionData;
	EN_transState_t transState;
	
	//Card operations
	while (getCardHolderName(&cardData)!=CARD_OK)
		printf("Please try again and enter a valid card holder Name\n");
	
	while (getCardExpiryDate(&cardData) != CARD_OK)
		printf("Please try again and enter a valid Expiry Date\n");
	
	while (getCardPAN(&cardData) != CARD_OK)
		printf("Please try again and enter a valid Primary Account Number\n");

	//terminal operations
	while (getTransactionDate(&terminalData) != TERMINAL_OK)
		printf("Please try again and enter a valid Transaction Date\n");

	setMaxAmount(&terminalData, (float32_t)10000.0);

	if (isCardExpired(&cardData, &terminalData) == TERMINAL_OK)
	{
		while(getTransactionAmount(&terminalData) != TERMINAL_OK)
			printf("Please try again and enter a valid Transaction Amount\n");
		
		if (isBelowMaxAmount(&terminalData) == TERMINAL_OK)
		{
			//server operations
			transactionData.cardHolderData = cardData;
			transactionData.terminalData = terminalData;
			transState=receiveTransactionData(&transactionData);
			switch (transState) {
				case APPROVED:
					printf("Transaction approved!\n");
					break;
				case DECLINED_INSUFFECIENT_FUND:
					printf("Transaction declined due to insuffecient fund!\n");
					break;
				case DECLINED_STOLEN_CARD:
					printf("Transaction declined, THIS IS A STOLEN CARD!!\n");
					break;
				case FRAUD_CARD:
					printf("Transaction declined, This card doesn't exist!\n");
					break;
				case INTERNAL_SERVER_ERROR:
					printf("Transaction declined due to internal server error!");
					break;
			}
		}
		else
		{
			printf("Transaction couldn't proceed because Amount exceeded the Maximum limit!\n");
		}
	}
	else
	{
		printf("Transaction couldn't proceed, this card is expired!\n");
	}
}
