#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpio.h"
#include "usart.h"
#include "main.h"
#include "tim.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"
#include "ssd1306_fonts.h"

#define ADC_REFERENCE_VOLTAGE 3.3
#define ADC_MAX 0xFFF //Max adc value.
#define VOLTAGE_STR_LEN 5
#define ADC_NUMBER_OF_CHANNELS 8 //Use 8 channels to measure parameters.
#define USART_STRING_SIZE 70
//Using usart
#define UART huart1

extern ADC_HandleTypeDef hadc1;
extern IWDG_HandleTypeDef hiwdg;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

#define LED_USER_PERIOD_MSEC    ( 500 ) //delete this
#define SEND_DATA_TO_USART_PERIOD_MSEC 10000

/// Счётчики.
volatile uint32_t TimeTickMs = 0;
uint32_t oldTimeTickHSec = 0;

// Options for the status of the transmission method.
enum TxState
{ 
  Default = 0, ///< synchronous transmission
  Interrupt, ///< asynchronous transmission (interrupts)
  DMA ///< asynchronous transfer (DMA)
};

// Status of the transmission method.
TxState State = TxState::Default;

char usartString[USART_STRING_SIZE]; //The string that will be sent via usart.

uint32_t adcData[ADC_NUMBER_OF_CHANNELS]; //Measured adc values. 

/**
 * \brief   Выполняет инициализацию.
 *
 */
void init( void )
{
   
}


/**
 * \brief   Выполняет дополнительные настройки.
 *
 */
void setup( void )
{
    HAL_TIM_Base_Start_IT(&htim2);

      // Устанавливаем состояние по умолчанию.
    if ( HAL_GPIO_ReadPin( LED_USER_GPIO_Port, LED_USER_Pin ) == GPIO_PIN_SET )
    {
        HAL_GPIO_WritePin( LED_USER_GPIO_Port, LED_USER_Pin, GPIO_PIN_RESET );
    }

    HAL_GPIO_WritePin( USART1_DE_GPIO_Port, USART1_DE_Pin, GPIO_PIN_SET );

    HAL_ADC_Start_DMA(&hadc1, adcData, ADC_NUMBER_OF_CHANNELS); // start adc in DMA mode

    ssd1306_Init();
    //HAL_TIM_Base_Start_IT(&htim3);
}

void voltageToString(uint32_t adc, char* str)
{
   float voltage = adc  * ADC_REFERENCE_VOLTAGE / ADC_MAX;
   sprintf(str,"%d.%02d ", (uint32_t)voltage, (uint16_t)((voltage - (uint32_t)voltage) * 100.));
}

/**
 * \brief   Выполняется периодически в теле основного цикла.
 *
 */

void loop( void )
{
    HAL_Delay( 500 );

    // We are waiting for the end of the packet transmission.
    if ( UART.gState != HAL_UART_STATE_READY ) return;
/*
    //Utf-8 string transmission via serial port.
    switch ( State )
    {
        case TxState::Interrupt:
            HAL_UART_Transmit_IT( & UART, ( uint8_t * ) Text2, sizeof( Text2 ) - 1 );
            break;

        case TxState::DMA:
            HAL_UART_Transmit_DMA( & UART, ( uint8_t * ) Text3, sizeof( Text3 ) - 1 );
            break;

        default:
            HAL_UART_Transmit( & UART, ( uint8_t * ) Text1, sizeof( Text1 ) - 1, 50 );
            State = TxState::Interrupt;
            break;
    } 
*/
  int strPos = 0;
  for(int i = 0; i < ADC_NUMBER_OF_CHANNELS; i++)
  {
    voltageToString(adcData[i], usartString + i * VOLTAGE_STR_LEN + 1);
    strPos+= VOLTAGE_STR_LEN;
  }

  sprintf(usartString + strPos + 1, "u\n");
  HAL_UART_Transmit( & UART, ( uint8_t * ) usartString, sizeof( usartString ), 50 );
  HAL_Delay(500) ;

  ssd1306_Fill(Black);
  ssd1306_SetCursor(2,0);
  ssd1306_WriteString("Got bear?", Font_11x18, White);
  ssd1306_UpdateScreen();
  HAL_IWDG_Refresh(&hiwdg);
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
/**
 * \brief   Callback-функция периодического таймера SysTick.
 *
 */
int beep = 20;
bool alarm = false;
void HAL_SYSTICK_Callback( void )
{
    TimeTickMs++;

    if ( TimeTickMs - oldTimeTickHSec > LED_USER_PERIOD_MSEC )
    {
        oldTimeTickHSec = TimeTickMs;

        // Indication of the main cycle operation.
       // HAL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );

    if(beep > 30) beep = 10;

    //beep++;

      __HAL_TIM_SET_AUTORELOAD(&htim3, beep*2);
      __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4, beep);

    enableBeeper(alarm);
    alarm = !alarm;


    }    
}

/**
 * \brief   Callback-функция завершения передачи данных.
 *
 * \param   huart   указатель на структуру параметров конфигурации типа UART_HandleTypeDef.
 *
 */
void HAL_UART_TxCpltCallback( UART_HandleTypeDef * huart )
{
    switch ( State )
    {
        case TxState::Interrupt: State = TxState::DMA; break;

        case TxState::DMA: State = TxState::Default; break;

        default: State = TxState::Interrupt; break;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{   
   if (htim->Instance == TIM2)
   {
       HAL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin ); 
   }
}
