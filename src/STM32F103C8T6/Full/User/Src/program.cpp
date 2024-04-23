#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpio.h"
#include "usart.h"
#include "main.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"
#include "ssd1306_fonts.h"



//Using usart
#define UART huart1
extern ADC_HandleTypeDef hadc1;

/// Период сигнала, управляющего светодиодом LED_USER, мсек.
#define LED_USER_PERIOD_MSEC    ( 500 )

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

const char Text1[] = "Привет из STM32! (def)\n";
const char Text2[] = "Привет из STM32! (int)\n";
const char Text3[] = "Привет из STM32! (dma)\n";

char adcStr[7];

uint32_t adcData[6]; 

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
      // Устанавливаем состояние по умолчанию.
    if ( HAL_GPIO_ReadPin( LED_USER_GPIO_Port, LED_USER_Pin ) == GPIO_PIN_SET )
    {
        HAL_GPIO_WritePin( LED_USER_GPIO_Port, LED_USER_Pin, GPIO_PIN_RESET );
    }

    HAL_GPIO_WritePin( USART1_DE_GPIO_Port, USART1_DE_Pin, GPIO_PIN_SET );

    HAL_ADC_Start_DMA(&hadc1, adcData, 6); // start adc in DMA mode

    ssd1306_Init();
}


/**
 * \brief   Выполняется периодически в теле основного цикла.
 *
 */
void loop( void )
{
    HAL_Delay( 500 );

    // Ожидаем окончания передачи пакета.
    if ( UART.gState != HAL_UART_STATE_READY ) return;
/*
    // Передача строки utf-8 через последовательный порт.
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
  //%d.%02dV 
  sprintf(adcStr, "%d \n", adcData[0]);
  HAL_UART_Transmit( & UART, ( uint8_t * ) adcStr, sizeof( adcStr ) - 1, 50 );

  ssd1306_Fill(Black);
  ssd1306_SetCursor(2,0);
  ssd1306_WriteString("Got bear?", Font_11x18, White);
  ssd1306_UpdateScreen();
}


/**
 * \brief   Callback-функция периодического таймера SysTick.
 *
 */
void HAL_SYSTICK_Callback( void )
{
    TimeTickMs++;

    if ( TimeTickMs - oldTimeTickHSec > LED_USER_PERIOD_MSEC )
    {
        oldTimeTickHSec = TimeTickMs;

        // Индикация работы основного цикла.
        HAL_GPIO_TogglePin( LED_USER_GPIO_Port, LED_USER_Pin );
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