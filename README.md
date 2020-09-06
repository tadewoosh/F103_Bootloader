# F103 Bootloader
This repo contains a Atollic True Studio project containing a bootloader program boilerplate for STM32F103 microcontroller.

## What is a Bootloader?
It is a regular embedded program like any other, but with a special ingredient: a function, that swithes the context to the main application.

## Partitioning
First of all, the bootloader project always consists of two code projects: the bootloader and the app. Since the FLASH memory for code on the MCU is limited in size (in this example to 128K) one need to adopt a strict partitionin scheme. For this purpose I propose:
* 16K for the bootloader
* the rest (121K) for the app

From this simple partiniotning come some consequences:
* the bootloader size must not exceed its partitnion size
* the app must be configured, so it runs not from the start of FLASH but from its partitnion start point.

## Configuring the Bootloader project
In the code editor one might find a *.ld* file, being the linker script file. It contains, among other stuff the addresses of the FLASH memory and its size. In order to prevent from building a bootloader that is too large one can edit the linker file and correct the size of the dedicated partition:
```C
/* Specify the memory areas */
MEMORY
{
  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 128K
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 20K
  MEMORY_B1 (rx)  : ORIGIN = 0x60000000, LENGTH = 0K
}
```
and change the FLASH length to required size:
```C
FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 16K
```
This is a safeguard. If your bootloader size would exceed this limit your program is not goin to crash, instad you get a nice and understandable linker error.

## Code explanation
See [main.c](/src/main.c) file to get all the code needed to understand the bootloader.

### Defining the app start address
In the definitions section of the code you find:
```C
/* Flash starting adress for STM32 devices: */
#define FLASH_START					0x08000000
/* Application start address at 16kb (byte 16384) from the flash start*/
#define APPLICATION_ADDRESS        	(FLASH_START + 0x4000)
```
This is the calculation of the app starting address. It is quite simple: to the flash start address you add the size of the bootloader partition (16K = 0x4000). This value is then passed to the magic ingredient.

### The magic ingredient
Here we go. Once the bootloader does all it needs to do, like read, compare, reflash or reject the new app binary image we call the `jump_to_app` function.
```C
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
```
What it does is calculates the addresses of the `appStack`, the `appEntry`, configures the vector table offset for interrupts, configures the new stack pointer and launches the app. This function never returns, so no code after it in the bootloader will execute. It is a good practise to leave an infinite loop afterwards in case something went wrong.
