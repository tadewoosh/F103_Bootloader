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

'''
/* Specify the memory areas */
MEMORY
{
  FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 128K
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 20K
  MEMORY_B1 (rx)  : ORIGIN = 0x60000000, LENGTH = 0K
}
'''

and change the FLASH length to:
'''
FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 16K
'''
