#include "queue.h"

void HIL_QUEUE_Init( QUEUE_HandleTypeDef *hqueue )
{
    hqueue->Head    = 0;                //0 
    hqueue->Tail    = 0;                //0
    hqueue->Empty   = DATA_NO_AVAILABLE;    //1
    hqueue->Full    = NO_FULL;                //0
}

uint8_t HIL_QUEUE_Write( QUEUE_HandleTypeDef *hqueue, void *data )
{
    uint8_t flag = 0;
    if (hqueue->Full == NO_FULL)
    {
        flag = 1;
        hqueue->Empty = DATA_AVAILABLE;
        memcpy(hqueue->Buffer + (hqueue->Head * hqueue->Size),data,hqueue->Size);
        hqueue->Head = (hqueue->Head + 1) % hqueue->Elements;

        if (hqueue->Head == hqueue->Tail)
        {
            hqueue->Full = FULL;
        }
    }
    return flag;
}

uint8_t HIL_QUEUE_Read( QUEUE_HandleTypeDef *hqueue, void *data )
{
    uint8_t flag = 0;
    if (hqueue->Empty == DATA_AVAILABLE)
    {
        flag = 1;
        hqueue->Full = NO_FULL;
        memcpy(data,hqueue->Buffer + (hqueue->Tail * hqueue->Size),hqueue->Size);

        hqueue->Tail = (hqueue->Tail + 1) % hqueue->Elements;

        if (hqueue->Tail == hqueue->Head)
        {
            hqueue->Head    = 0;
            hqueue->Tail    = 0;
            hqueue->Empty   = DATA_NO_AVAILABLE;
            hqueue->Full    = NO_FULL;
        }
    }
    
    return flag;
}

uint8_t HIL_QUEUE_IsEmpty( QUEUE_HandleTypeDef *hqueue )
{
    return hqueue->Empty;
}