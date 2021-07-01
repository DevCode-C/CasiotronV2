#include "queue.h"

void HIL_QUEUE_Init( QUEUE_HandleTypeDef *hqueue )
{
    hqueue->Head    = 0;
    hqueue->Tail    = 0;
    hqueue->Empty   = 1;
    hqueue->Full    = 0;
}

uint8_t HIL_QUEUE_Write( QUEUE_HandleTypeDef *hqueue, void *data )
{
    uint8_t flag = 0;
    if (hqueue->Full == 0)
    {
        if ((hqueue->Elements-hqueue->Head) >= hqueue->Size)
        {
            flag = 1;
            hqueue->Empty = 0;
            memcpy(hqueue->Buffer + (hqueue->Head),data,hqueue->Size);
            hqueue->Head += hqueue->Size;
        }

        if (hqueue->Head >= (hqueue->Elements-1))
        {
            hqueue->Full = 1;
            flag = 0;
        }
    }
    return flag;
}

uint8_t HIL_QUEUE_Read( QUEUE_HandleTypeDef *hqueue, void *data )
{
    uint8_t flag = 0;
    if (hqueue->Empty == 0)
    {
        flag = 1;
        memcpy(data,hqueue->Buffer + (hqueue->Tail),hqueue->Size);
        hqueue->Tail += hqueue->Size;
        if (hqueue->Tail == hqueue->Head)
        {
            hqueue->Head    = 0;
            hqueue->Tail    = 0;
            hqueue->Empty   = 1;
            hqueue->Full    = 0;
        }
    }
    
    return flag;
}

uint8_t HIL_QUEUE_IsEmpty( QUEUE_HandleTypeDef *hqueue )
{
    return hqueue->Empty;
}