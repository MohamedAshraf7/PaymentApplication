#ifndef CARD_H
#define CARD_H

#include "generic.h"

#define MAX_LIMIT_NAME_LENGTH      24
#define MIN_LIMIT_NAME_LENGTH      20
#define EXPDATE_LENGTH             5
#define MAX_LIMIT_PAN_LENGTH       19
#define MIN_LIMIT_PAN_LENGTH       16

/*************************Structs************************/

typedef struct ST_cardData_t
{
    uint8_t cardHolderName[25];
    uint8_t primaryAccountNumber[20];
    uint8_t cardExpirationDate[6];
}ST_cardData_t;

/*************************ENUMS************************/

typedef enum EN_cardError_t
{
    CARD_OK, WRONG_NAME, WRONG_EXP_DATE, WRONG_PAN
}EN_cardError_t;



/*******************Functions Prototypes******************/

EN_cardError_t getCardHolderName(ST_cardData_t* cardData);
EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData);
EN_cardError_t getCardPAN(ST_cardData_t* cardData);
void getCardHolderNameTest(void);
void getCardExpiryDateTest(void);
void getCardPANTest(void);



#endif