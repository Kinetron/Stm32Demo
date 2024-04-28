#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpio.h"
#include "main.h"

#define LED_USER_PERIOD_MSEC  ( 500 )

volatile uint32_t TimeTickMs = 0;
uint32_t oldTimeTickHSec = 0;

/**
 * \brief  Performs initialization. 
 *
 */
void init( void )
{
   
}

/**
 * \brief  Performs additional settings.
 *
 */
void setup( void )
{
    // Setting the default state.  
    if ( HAL_GPIO_ReadPin( USER_LED_GPIO_Port, USER_LED_Pin ) == GPIO_PIN_SET )
    {
        HAL_GPIO_WritePin( USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET );
    }
}

/**
 * \brief   It is performed periodically in the body of the main loop.
 *
 */

void loop( void )
{
    HAL_Delay( 100 );
      HAL_GPIO_TogglePin( USER_LED_GPIO_Port, USER_LED_Pin);
}

void HAL_SYSTICK_Callback( void )
{
    TimeTickMs++;

    if ( TimeTickMs - oldTimeTickHSec > LED_USER_PERIOD_MSEC )
    {
        oldTimeTickHSec = TimeTickMs;

        // Indication of the main cycle operation.
     

    }    
}