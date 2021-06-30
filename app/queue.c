#include "queue.h"

void HIL_QUEUE_Init( QUEUE_HandleTypeDef *hqueue );

uint8_t HIL_QUEUE_Write( QUEUE_HandleTypeDef *hqueue, void *data );

uint8_t HIL_QUEUE_Read( QUEUE_HandleTypeDef *hqueue, void *data );

uint8_t HIL_QUEUE_IsEmpty( QUEUE_HandleTypeDef *hqueue );