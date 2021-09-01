#include "app_bsp.h"
#include "queue.h"


void HIL_QUEUE_Init( QUEUE_HandleTypeDef *hqueue )
{
    hqueue->Head    = 0;                        //0 
    hqueue->Tail    = 0;                        //0
    hqueue->Empty   = DATA_NO_AVAILABLE;        //1
    hqueue->Full    = NO_FULL;                  //0
}

uint8_t HIL_QUEUE_Write( QUEUE_HandleTypeDef *hqueue, void *data )
{
    uint8_t flag = 0;
    if (hqueue->Full == NO_FULL)
    {
        flag = 1;
        hqueue->Empty = DATA_AVAILABLE;
        /*The supretion is nedded because */
        (void) memcpy(hqueue->Buffer + (hqueue->Head * hqueue->Size),data,hqueue->Size); /* cppcheck-suppress misra-c2012-18.4 */
        hqueue->Head = (hqueue->Head + 1UL) % hqueue->Elements;

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
        /*The supretion is nedded because */
        (void) memcpy(data,hqueue->Buffer + (hqueue->Tail * hqueue->Size),hqueue->Size); /* cppcheck-suppress misra-c2012-18.4 */

        hqueue->Tail = (hqueue->Tail + 1UL) % hqueue->Elements;

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