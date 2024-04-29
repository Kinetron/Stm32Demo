#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpio.h"
#include "main.h"

#define LED_USER_PERIOD_MSEC  ( 500 )

volatile uint32_t TimeTickMs = 0;
uint32_t oldTimeTickHSec = 0;

extern TIM_HandleTypeDef htim3;

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
    HAL_Delay( 250 );
     //HAL_GPIO_TogglePin( USER_LED_GPIO_Port, USER_LED_Pin);
}

void enableBeeper(bool enabled)
{
   GPIO_InitTypeDef GPIO_InitStruct = {0};
   GPIO_InitStruct.Pin = GPIO_PIN_1;
   
   if(enabled)
   {
     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
   }
   else
   {
    GPIO_InitStruct.Mode = GPIO_MODE_AF_INPUT;
   }
   
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

int beep = 20;
bool alarm = false;
void HAL_SYSTICK_Callback( void )
{
    TimeTickMs++;

    if ( TimeTickMs - oldTimeTickHSec > LED_USER_PERIOD_MSEC )
    {
        oldTimeTickHSec = TimeTickMs;

        // Indication of the main cycle operation.
      HAL_GPIO_TogglePin( USER_LED_GPIO_Port, USER_LED_Pin);
    //beep++;
   
    }    
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{   
   if (htim->Instance == TIM2)
   {
    // HAL_GPIO_TogglePin( USER_LED_GPIO_Port, USER_LED_Pin);
    // Indication of the main cycle operation.
         if(beep > 30) beep = 10;

    beep++;
//HAL_GPIO_TogglePin( USER_LED_GPIO_Port, USER_LED_Pin);
      __HAL_TIM_SET_AUTORELOAD(&htim3, beep*2);
      __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4, beep);

    enableBeeper(alarm);
    alarm = !alarm;
   }
}