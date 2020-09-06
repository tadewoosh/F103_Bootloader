#include <stddef.h>
#include <stdio.h>
#include "stm32f10x.h"

/* Flash starting adress for STM32 devices: */
#define FLASH_START					0x08000000
/* Application start address at 16kb (byte 16384) from the flash start*/
#define APPLICATION_ADDRESS        	(FLASH_START + 0x4000)

/* function declarations */
void hw_init(void);
void jump_to_app(const int address);

int main(void) {

	// Init hardware, i.e the serial port in order to send debug messages.
	hw_init();
	printf("\r\nBootloader starting!\r\n");

	/*
	 * Do some bootloadery things here, like download, check, load and recheck the app;
	 */

	printf("Handing over to main app... \n");

	jump_to_app(APPLICATION_ADDRESS);

	printf("\r\nError! Bootloader Hang!\r\n");
	while (1) {

	}
}

void jump_to_app(const int ADDRESS)
{

	typedef  void (*pFunction)(void);
	pFunction appEntry;
	uint32_t appStack;

	/* Get the application stack pointer (First entry in the application vector table) */
	appStack = (uint32_t) *((__IO uint32_t*)ADDRESS);

	/* Get the application entry point (Second entry in the application vector table) */
	appEntry = (pFunction) *(__IO uint32_t*) (ADDRESS + 4);

	/* Reconfigure vector table offset register to match the application location */
	SCB->VTOR = ADDRESS;

	/* Set the application stack pointer */
	__set_MSP(appStack);

	/* Start the application */
	appEntry();
}


void hw_init(void) {

	// Enable clocks for used peripherials
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	// Configure GPIOs
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	// GPIO for UASART use: TX pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// GPIO for UASART use: RX pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Config USART
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;

	// Init and enable USART
	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);

}
