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
    if (hbuffer->Full == 0) //
    {
        hbuffer->Empty = 0;
        hbuffer->Buffer[hbuffer->Head] = data;
        hbuffer->Head++; 
        if (hbuffer->Head == (hbuffer->Elements))
        {
            hbuffer->Head = 0;
        }
        if (hbuffer->Head == hbuffer->Tail)
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
        hbuffer->Full = 0;
        temp = hbuffer->Buffer[hbuffer->Tail];
        hbuffer->Tail++;
        if (hbuffer->Tail == (hbuffer->Elements)) //Check if the Tail value are equal to elements value
        {
            hbuffer->Tail = 0; //f are equal, set tail value to Zero
        }
        if (hbuffer->Tail == hbuffer->Head) //Check if tail value are equal to head value
        {
            //If are equal, set the members at initial values
            hbuffer->Empty = 1;
            hbuffer->Full = 0;
            hbuffer->Head   = 0;
            hbuffer->Tail   = 0;
        }
    }
    else
    {
        //If circular buufer is empty but if you try to read, return a invalid character ->'~'
        temp = INVALID_CHARACTER;
    }
    
    return temp;
}
