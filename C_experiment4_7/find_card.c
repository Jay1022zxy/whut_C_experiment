#include <stdio.h>
#include <string.h>
#include "find_card.h"
#include "card.h"
#include "billing.h"
#include "computer.h"

Billing *find_billing_by_id(const char *cardID)
{
    Billing *p = billing_head;
    while (p != NULL)
    {
        if (strcmp(p->cardID, cardID) == 0)
        {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

Card *find_card_by_id(const char *id)
{
    Card *p = card_head;
    while (p != NULL)
    {
        if (strcmp(p->cardID, id) == 0)
        {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

Login *find_login_by_id(const char *cardID)
{
    Login *p = login_head;
    while (p != NULL)
    {
        if (strcmp(p->cardID, cardID) == 0)
        {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

Settle *find_settle_by_id(const char *cardID)
{
    Settle *p = settle_head;
    while (p != NULL)
    {
        if (strcmp(p->cardID, cardID) == 0)
        {
            return p;
        }
        p = p->next;
    }
    return NULL;
}