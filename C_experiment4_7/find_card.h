#ifndef FIND_CARD_H
#define FIND_CARD_H
#include "card.h"
#include "billing.h"
#include "computer.h"

Billing *find_billing_by_id(const char *cardID);
Card *find_card_by_id(const char *id);
Login *find_login_by_id(const char *cardID);
Settle *find_settle_by_id(const char *cardID);

#endif // FIND_CARD_H

