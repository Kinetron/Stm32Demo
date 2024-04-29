#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands_decoder.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"
#include "ssd1306_fonts.h"

#define USART_BUFFER_SIZE 25
#define UART huart1

uint8_t usartBuffer[USART_BUFFER_SIZE];

uint8_t usartRx;
uint8_t usartBufferPos = 0;

void commands_decoder_init(void)
{
   HAL_UART_Receive_DMA(&huart1, &usartRx, 1);   
}

void receive_byte(void)
{
   if(usartRx != 0x0D)
   {
       commands_decoder_add_to_buff();
       ssd1306_Fill(Black);
       ssd1306_SetCursor(2,0);
       ssd1306_WriteString((const char *)usartBuffer, Font_7x10, White);
       ssd1306_UpdateScreen();

       return;
   }

   if(strcmp((const char *)usartBuffer, "reset") == 0)
   {
       HAL_GPIO_TogglePin( USER_LED_GPIO_Port, USER_LED_Pin);
       HAL_NVIC_SystemReset();
   }
}

void commands_decoder_add_to_buff(void)
{
   if(usartBufferPos < USART_BUFFER_SIZE)
   {
      usartBuffer[usartBufferPos] = usartRx;
      usartBufferPos ++;
   }
   else
   {
     usartBufferPos = 0;
     usartBuffer[usartBufferPos] = usartRx;
   }
}