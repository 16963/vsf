/**************************************************************************
 *  Copyright (C) 2008 - 2012 by Simon Qian                               *
 *  SimonQian@SimonQian.com                                               *
 *                                                                        *
 *  Project:    Versaloon                                                 *
 *  File:       hw_cfg_VSFCore_STM32.h                                    *
 *  Author:     SimonQian                                                 *
 *  Versaion:   See changelog                                             *
 *  Purpose:    hardware configuration file for VSFCore_STM32             *
 *  License:    See license                                               *
 *------------------------------------------------------------------------*
 *  Change Log:                                                           *
 *      YYYY-MM-DD:     What(by Who)                                      *
 *      2008-11-07:     created(by SimonQian)                             *
 *      2008-11-22:     rewrite GPIO_Dir(by SimonQian)                    *
 **************************************************************************/

#ifndef HSE_VALUE
#define HSE_VALUE						((uint32_t)12000000)
#endif

#define OSC_HZ							HSE_VALUE

#define SYS_MAIN_ADDR					0x08000000
// system locates in the first 64KB area, and followed by application
#define APP_MAIN_ADDR					(SYS_MAIN_ADDR + 512 * 1024)

#define KEY_PORT						0
#define KEY_PIN							4

#define USB_PULLUP_PORT					2
#define USB_PULLUP_PIN					13

// bcm wifi
#define BCM_PORT						BCM_BUS_TYPE_SPI
#define BCM_SPI_PORT					1
#define BCM_SPI_CS_PORT					1
#define BCM_SPI_CS_PIN					10
#define BCM_SPI_FREQ					9000
#define BCM_RST_PORT					1
#define BCM_RST_PIN						11
#define BCM_EINT_PORT					0
#define BCM_EINT_PIN					8
#define BCM_EINT						0x08
#define BCM_PWRCTRL_PORT				0
#define BCM_PWRCTRL_PIN					3

// LED24
#define LED24							{3, 4, 3, 6},\
										{4, 1, 4, 0},\
										{3, 5, 3, 3},\
										{3, 9, 3, 10},\
										{4, 15, 3, 8},\
										{4, 13, 4, 14},\
										{4, 11, 4, 12},\
										{4, 9, 4, 10},\
										{4, 7, 4, 8},\
										{3, 0, 3, 1},\
										{3, 14, 3, 15},\
										{6, 13, 6, 14},\
										{4, 6, 4, 2},\
										{4, 4, 4, 5},\
										{3, 13, 4, 3},\
										{3, 11, 3, 12},\
										{6, 4, 6, 5},\
										{6, 2, 6, 3},\
										{6, 0, 6, 1},\
										{5, 14, 5, 15},\
										{5, 12, 6, 13},\
										{5, 4, 5, 5},\
										{5, 2, 5, 3},\
										{5, 0, 5, 1},
