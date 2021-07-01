#include "buffer.h"

void HIL_BUFFER_Init( BUFFER_HandleTypeDef *hbuffer )
{
    hbuffer->Head   = 0;
    hbuffer->Tail   = 0;
    hbuffer->Empty  = 1;
    hbuffer->Full   = 0;
}

void HIL_BUFFER_Write( BUFFER_HandleTypeDef *hbuffer, uint8_t data )
{
    if (hbuffer->Full == 0)
    {
        hbuffer->Empty = 0;
        hbuffer->Buffer[hbuffer->Head] = data;
        hbuffer->Head++; 
        if (hbuffer->Head == (hbuffer->Elements-1))
        {
            hbuffer->Full = 1;
        }
    }
}

uint8_t HIL_BUFFER_IsEmpty( BUFFER_HandleTypeDef *hbuffer )
{
    return hbuffer->Empty;
}

uint8_t HIL_BUFFER_Read( BUFFER_HandleTypeDef *hbuffer )
{
    uint8_t temp = 0;
    if (hbuffer->Empty == 0)
    {
        temp = hbuffer->Buffer[hbuffer->Tail];
        hbuffer->Tail++;
        if (hbuffer->Tail == hbuffer->Head)
        {
            hbuffer->Empty = 1;
            hbuffer->Full = 0;
            hbuffer->Head   = 0;
            hbuffer->Tail   = 0;
        }
        
    }
    else
    {
        temp = INVALID_CHARACTER;
    }
    
    return temp;
}
