/**************************************************************************
 *  Copyright (C) 2008 - 2010 by Simon Qian                               *
 *  SimonQian@SimonQian.com                                               *
 *                                                                        *
 *  Project:    Versaloon                                                 *
 *  File:       BDM.c                                                     *
 *  Author:     SimonQian                                                 *
 *  Versaion:   See changelog                                             *
 *  Purpose:    BDM interface implementation file                         *
 *  License:    See license                                               *
 *------------------------------------------------------------------------*
 *  Change Log:                                                           *
 *      YYYY-MM-DD:     What(by Who)                                      *
 *      2011-05-09:     created(by SimonQian)                             *
 **************************************************************************/

#include "app_type.h"
#include "compiler.h"
#include "interfaces.h"

// TODO: remove MACROs below to nuc505_reg.h
#define NUC505_CLOCK_AHBCLK_UDC20_EN		((uint32_t)1 << 10)
#define NUC505_USBD_PHYCTL_DP_PULLUP		((uint32_t)1 << 8)
#define NUC505_USBD_PHYCTL_SUSPEND			((uint32_t)1 << 9)

// CEPCTL
#define USB_CEPCTL_NAKCLR					((uint32_t)0x00000000)
#define USB_CEPCTL_STALL					((uint32_t)0x00000002)
#define USB_CEPCTL_ZEROLEN					((uint32_t)0x00000004)
#define USB_CEPCTL_FLUSH					((uint32_t)0x00000008)

// EPxCFG
#define USB_EP_CFG_VALID					((uint32_t)0x00000001)
#define USB_EP_CFG_TYPE_BULK				((uint32_t)0x00000002)
#define USB_EP_CFG_TYPE_INT					((uint32_t)0x00000004)
#define USB_EP_CFG_TYPE_ISO					((uint32_t)0x00000006)
#define USB_EP_CFG_TYPE_MASK				((uint32_t)0x00000006)
#define USB_EP_CFG_DIR_OUT					((uint32_t)0x00000000)
#define USB_EP_CFG_DIR_IN					((uint32_t)0x00000008)

// EPxRSPCTL
#define USB_EP_RSPCTL_FLUSH					((uint32_t)0x00000001)
#define USB_EP_RSPCTL_MODE_AUTO				((uint32_t)0x00000000)
#define USB_EP_RSPCTL_MODE_MANUAL			((uint32_t)0x00000002)
#define USB_EP_RSPCTL_MODE_FLY				((uint32_t)0x00000004)
#define USB_EP_RSPCTL_MODE_MASK				((uint32_t)0x00000006)
#define USB_EP_RSPCTL_TOGGLE				((uint32_t)0x00000008)
#define USB_EP_RSPCTL_HALT					((uint32_t)0x00000010)
#define USB_EP_RSPCTL_ZEROLEN				((uint32_t)0x00000020)
#define USB_EP_RSPCTL_SHORTTXEN				((uint32_t)0x00000040)
#define USB_EP_RSPCTL_DISBUF				((uint32_t)0x00000080)

//#define NUC505_USBD_EP_REG(ep, reg)			\
//	*((__IO uint32_t *)((uint32_t)&USBD->reg + (uint32_t)((uint8_t)(ep) * 0x28)))
//#define NUC505_USBD_EP_REG8(ep, reg)		\
//	*((__IO uint8_t *)((uint32_t)&USBD->reg + (uint32_t)((uint8_t)(ep) * 0x28)))
#define NUC505_USBD_EP_REG(ep, reg)			(USBD->EP[ep].reg)

#if IFS_USBD_EN

#include "NUC505_USBD.h"
#include "NUC505Series.h"

#define NUC505_USBD_EP_NUM					12
const uint8_t nuc505_usbd_ep_num = NUC505_USBD_EP_NUM;
struct interface_usbd_callback_t nuc505_usbd_callback;
static uint16_t EP_Cfg_Ptr = 0x1000;
static uint16_t max_ctl_ep_size = 64;

// true if data direction in setup packet is device to host
static bool nuc505_setup_status_IN, nuc505_status_out = false;

#define NUC505_USBD_EPIN					0x10
#define NUC505_USBD_EPOUT					0x00
static int8_t nuc505_usbd_epaddr[NUC505_USBD_EP_NUM];

vsf_err_t nuc505_usbd_init(uint32_t int_priority)
{
	// Set IP clock
	// HXT 12M
	CLK->CLKDIV0 &= ~(CLK_CLKDIV0_USBDSEL_Msk | CLK_CLKDIV0_USBDDIV_Msk);
	// PLL 48
	//CLK->CLKDIV0 |= CLK_CLKDIV0_USBDSEL_Msk | (9ul << CLK_CLKDIV0_USBDDIV_Pos);
	// PLL 480
	//CLK->CLKDIV0 |= CLK_CLKDIV0_USBDSEL_Msk;
	CLK->AHBCLK |= CLK_AHBCLK_USBDCKEN_Msk;

	USBD->PHYCTL |= USBD_PHYCTL_PHYEN_Msk;
	while (USBD->EP[0].EPMPS != 0x20)
	{
		USBD->EP[0].EPMPS = 0x20;
	}
	while (USBD->EP[0].EPMPS != 0)
	{
		USBD->EP[0].EPMPS= 0;
	}

	if (1)
	{
		// Enable USB FULL SPEED
		USBD->OPER = 0;
	}
	else
	{
		// Enable USB HIGH SPEED
		USBD->OPER = 2;
		while (!USBD->OPER & 4);
	}

	// 8 nop for reg sync
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	// Enable USB interrupt
	USBD->GINTEN |= USBD_GINTEN_USBIEN_Msk | USBD_GINTEN_CEPIEN_Msk;
	// Enable BUS interrupt
	USBD->BUSINTEN = USBD_BUSINTEN_RSTIEN_Msk;
	USBD->CEPINTEN = USBD_CEPINTEN_SETUPPKIEN_Msk | USBD_CEPINTEN_RXPKIEN_Msk |
					USBD_CEPINTEN_TXPKIEN_Msk | USBD_CEPINTEN_STSDONEIEN_Msk;
	NVIC_EnableIRQ(USBD_IRQn);
	return VSFERR_NONE;
}

vsf_err_t nuc505_usbd_fini(void)
{
	USBD->PHYCTL &= ~USBD_PHYCTL_PHYEN_Msk;
	CLK->AHBCLK &= ~CLK_AHBCLK_USBDCKEN_Msk;
	return VSFERR_NONE;
}

vsf_err_t nuc505_usbd_reset(void)
{
	return VSFERR_NONE;
}

void USB_Istr(void);
vsf_err_t nuc505_usbd_poll(void)
{
	//USB_Istr();
	return VSFERR_NONE;
}

vsf_err_t nuc505_usbd_connect(void)
{
	USBD->PHYCTL |= USBD_PHYCTL_DPPUEN_Msk;
	return VSFERR_NONE;
}

vsf_err_t nuc505_usbd_disconnect(void)
{
	USBD->PHYCTL &= ~USBD_PHYCTL_DPPUEN_Msk;
	return VSFERR_NONE;
}

vsf_err_t nuc505_usbd_set_address(uint8_t address)
{
	USBD->FADDR = address;
	return VSFERR_NONE;
}

uint8_t nuc505_usbd_get_address(void)
{
	return USBD->FADDR;
}

vsf_err_t nuc505_usbd_suspend(void)
{
	return VSFERR_NONE;
}

vsf_err_t nuc505_usbd_resume(void)
{
	return VSFERR_NONE;
}

vsf_err_t nuc505_usbd_lowpower(uint8_t level)
{
	return VSFERR_NONE;
}

uint32_t nuc505_usbd_get_frame_number(void)
{
	return USBD->FRAMECNT >> 3;
}

vsf_err_t nuc505_usbd_get_setup(uint8_t *buffer)
{
	buffer[0] = USBD->SETUP1_0 & 0xFF;
	nuc505_setup_status_IN = (buffer[0] & 0x80) > 0;
	buffer[1] = (USBD->SETUP1_0 >> 8) & 0xFF;
	buffer[2] = USBD->SETUP3_2 & 0xFF;
	buffer[3] = (USBD->SETUP3_2 >> 8) & 0xFF;
	buffer[4] = USBD->SETUP5_4 & 0xFF;
	buffer[5] = (USBD->SETUP5_4 >> 8) & 0xFF;
	buffer[6] = USBD->SETUP7_6 & 0xFF;
	buffer[7] = (USBD->SETUP7_6 >> 8) & 0xFF;
	return VSFERR_NONE;
}

vsf_err_t nuc505_usbd_prepare_buffer(void)
{
	EP_Cfg_Ptr = 0x1000;
	memset(nuc505_usbd_epaddr, -1, sizeof(nuc505_usbd_epaddr));
	return VSFERR_NONE;
}

vsf_err_t nuc505_usbd_ep_reset(uint8_t idx)
{
	return VSFERR_NONE;
}

static int8_t nuc505_usbd_ep(uint8_t idx)
{
	uint8_t i;

	for (i = 0; i < sizeof(nuc505_usbd_epaddr); i++)
	{
		if ((int8_t)idx == nuc505_usbd_epaddr[i])
		{
			return (int8_t)i;
		}
	}
	return -1;
}

static int8_t nuc505_usbd_get_free_ep(uint8_t idx)
{
	uint8_t i;

	for (i = 0; i < sizeof(nuc505_usbd_epaddr); i++)
	{
		if (-1 == nuc505_usbd_epaddr[i])
		{
			nuc505_usbd_epaddr[i] = (int8_t)idx;
			return i;
		}
	}
	return -1;
}

static void nuc505_usbd_set_eptype(uint8_t ep, uint32_t type)
{
	NUC505_USBD_EP_REG(ep, EPRSPCTL) =
				USB_EP_RSPCTL_FLUSH | USB_EP_RSPCTL_MODE_MANUAL;
	NUC505_USBD_EP_REG(ep, EPCFG) &= ~USB_EP_CFG_TYPE_MASK;
	NUC505_USBD_EP_REG(ep, EPCFG) |= type | USB_EP_CFG_VALID;
}

vsf_err_t nuc505_usbd_ep_set_type(uint8_t idx, enum interface_usbd_eptype_t type)
{
	int8_t index_in = nuc505_usbd_ep(idx | NUC505_USBD_EPIN);
	int8_t index_out = nuc505_usbd_ep(idx | NUC505_USBD_EPOUT);
	uint32_t eptype;

	if ((index_in < 0) && (index_out < 0))
	{
		return VSFERR_FAIL;
	}

	switch (type)
	{
	case USB_EP_TYPE_CONTROL:
		return VSFERR_NONE;
	case USB_EP_TYPE_INTERRUPT:
		eptype = USB_EP_CFG_TYPE_INT;
		break;
	case USB_EP_TYPE_BULK:
		eptype = USB_EP_CFG_TYPE_BULK;
		break;
	case USB_EP_TYPE_ISO:
		eptype = USB_EP_CFG_TYPE_ISO;
		break;
	default:
		return VSFERR_INVALID_PARAMETER;
	}
	if (index_in > 1)
	{
		index_in -= 2;
		nuc505_usbd_set_eptype(index_in, eptype);
		USBD->GINTEN |= USBD_GINTEN_EPAIEN_Msk << index_in;
		NUC505_USBD_EP_REG(index_in, EPINTEN) = USBD_EPINTEN_TXPKIEN_Msk;
	}
	if (index_out > 1)
	{
		index_out -= 2;
		nuc505_usbd_set_eptype(index_out, eptype);
		USBD->GINTEN |= USBD_GINTEN_EPAIEN_Msk << index_out;
		NUC505_USBD_EP_REG(index_out, EPINTEN) = USBD_EPINTEN_RXPKIEN_Msk;
	}
	return VSFERR_NONE;
}

vsf_err_t nuc505_usbd_ep_set_IN_dbuffer(uint8_t idx)
{
	return VSFERR_NONE;
}

bool nuc505_usbd_ep_is_IN_dbuffer(uint8_t idx)
{
	return false;
}

vsf_err_t nuc505_usbd_ep_switch_IN_buffer(uint8_t idx)
{
	return VSFERR_NONE;
}

vsf_err_t nuc505_usbd_ep_set_IN_epsize(uint8_t idx, uint16_t epsize)
{
	int8_t index = nuc505_usbd_get_free_ep(idx | NUC505_USBD_EPIN);
	if (index < 0)
	{
		return VSFERR_FAIL;
	}

	if (0 == idx)
	{
		if ((EP_Cfg_Ptr - epsize) > 0x1000)
		{
			return VSFERR_NOT_ENOUGH_RESOURCES;
		}

		EP_Cfg_Ptr -= epsize & 1 ? epsize + 1 : epsize;
		USBD->CEPBUFSTART = EP_Cfg_Ptr;
		USBD->CEPBUFEND = EP_Cfg_Ptr + epsize - 1;
		max_ctl_ep_size = epsize;
		return VSFERR_NONE;
	}
	else if (index > 1)
	{
		int8_t index_out = nuc505_usbd_ep(idx | NUC505_USBD_EPOUT);

		index -= 2;
		if (index_out < 0)
		{
			if ((EP_Cfg_Ptr - epsize) > 0x1000)
			{
				return VSFERR_NOT_ENOUGH_RESOURCES;
			}

			EP_Cfg_Ptr -= epsize & 1 ? epsize + 1 : epsize;
			NUC505_USBD_EP_REG(index, EPBUFSTART) = EP_Cfg_Ptr;
			NUC505_USBD_EP_REG(index, EPBUFEND) = EP_Cfg_Ptr + epsize - 1;
		}
		else if (index_out > 1)
		{
			index_out -= 2;
			NUC505_USBD_EP_REG(index, EPBUFSTART) =\
									NUC505_USBD_EP_REG(index_out, EPBUFSTART);
			NUC505_USBD_EP_REG(index, EPBUFEND) =\
									NUC505_USBD_EP_REG(index_out, EPBUFEND);
		}
		else
		{
			return VSFERR_BUG;
		}
		NUC505_USBD_EP_REG(index, EPMPS) = epsize;
		NUC505_USBD_EP_REG(index, EPCFG) |= USB_EP_CFG_DIR_IN;
		NUC505_USBD_EP_REG(index, EPCFG) &= ~(0xF << 4);
		NUC505_USBD_EP_REG(index, EPCFG) |= (idx & 0x0F) << 4;
		return VSFERR_NONE;
	}
	return VSFERR_BUG;
}

uint16_t nuc505_usbd_ep_get_IN_epsize(uint8_t idx)
{
	int8_t index = nuc505_usbd_ep(idx | NUC505_USBD_EPIN);
	if ((index < 0) || (index >= NUC505_USBD_EP_NUM))
	{
		return 0;
	}

	if (0 == idx)
	{
		return max_ctl_ep_size;
	}
	else if (index > 1)
	{
		index -= 2;
		return NUC505_USBD_EP_REG(index, EPMPS);
	}
	return 0;
}

vsf_err_t nuc505_usbd_ep_set_IN_stall(uint8_t idx)
{
	int8_t index = nuc505_usbd_ep(idx | NUC505_USBD_EPIN);
	if ((index < 0) || (index >= NUC505_USBD_EP_NUM))
	{
		return VSFERR_FAIL;
	}

	if (0 == idx)
	{
		USBD->CEPCTL = 2;
		USBD->CEPCTL |= USB_CEPCTL_FLUSH;
		return VSFERR_NONE;
	}
	else if (index > 1)
	{
		index -= 2;
		NUC505_USBD_EP_REG(index, EPRSPCTL) =
			(NUC505_USBD_EP_REG(index, EPRSPCTL) & 0xF7) | USB_EP_RSPCTL_HALT;
		NUC505_USBD_EP_REG(index, EPRSPCTL) |= USB_EP_RSPCTL_FLUSH;
		return VSFERR_NONE;
	}
	return VSFERR_BUG;
}

vsf_err_t nuc505_usbd_ep_clear_IN_stall(uint8_t idx)
{
	int8_t index = nuc505_usbd_ep(idx | NUC505_USBD_EPIN);
	if ((index < 0) || (index >= NUC505_USBD_EP_NUM))
	{
		return VSFERR_FAIL;
	}

	if (0 == idx)
	{
		USBD->CEPCTL &= ~3;
		return VSFERR_NONE;
	}
	else if (index > 1)
	{
		index -= 2;
		NUC505_USBD_EP_REG(index, EPRSPCTL) |= USB_EP_RSPCTL_FLUSH;
		NUC505_USBD_EP_REG(index, EPRSPCTL) &= ~USB_EP_RSPCTL_HALT;
		NUC505_USBD_EP_REG(index, EPRSPCTL) |= USB_EP_RSPCTL_TOGGLE;
		return VSFERR_NONE;
	}
	return VSFERR_BUG;
}

bool nuc505_usbd_ep_is_IN_stall(uint8_t idx)
{
	int8_t index = nuc505_usbd_ep(idx | NUC505_USBD_EPIN);
	if ((index < 0) || (index >= NUC505_USBD_EP_NUM))
	{
		return true;
	}

	if (0 == idx)
	{
		return (USBD->CEPCTL & USB_CEPCTL_STALL) > 0;
	}
	else if (index > 1)
	{
		index -= 2;
		return (NUC505_USBD_EP_REG(index, EPRSPCTL) & USB_EP_RSPCTL_HALT) > 0;
	}
	return true;
}

vsf_err_t nuc505_usbd_ep_reset_IN_toggle(uint8_t idx)
{
	int8_t index = nuc505_usbd_ep(idx | NUC505_USBD_EPIN);
	if ((index < 0) || (index >= NUC505_USBD_EP_NUM))
	{
		return VSFERR_FAIL;
	}

	if (0 == idx)
	{
		return VSFERR_NOT_SUPPORT;
	}
	else if (index > 1)
	{
		index -= 2;
		NUC505_USBD_EP_REG(index, EPRSPCTL) |= USB_EP_RSPCTL_TOGGLE;
		return VSFERR_NONE;
	}
	return VSFERR_BUG;
}

vsf_err_t nuc505_usbd_ep_toggle_IN_toggle(uint8_t idx)
{
	return VSFERR_NOT_SUPPORT;
}

vsf_err_t nuc505_usbd_ep_set_IN_count(uint8_t idx, uint16_t size)
{
	int8_t index = nuc505_usbd_ep(idx | NUC505_USBD_EPIN);
	if ((index < 0) || (index >= NUC505_USBD_EP_NUM))
	{
		return VSFERR_FAIL;
	}

	if (0 == idx)
	{
		if (!nuc505_setup_status_IN && (0 == size))
		{
			USBD->CEPCTL = USB_CEPCTL_NAKCLR;
		}
		else
		{
			USBD->CEPTXCNT = size;
		}
		return VSFERR_NONE;
	}
	else if (index > 1)
	{
		index -= 2;
		NUC505_USBD_EP_REG(index, EPTXCNT) = size;
		return VSFERR_NONE;
	}
	return VSFERR_BUG;
}

vsf_err_t nuc505_usbd_ep_write_IN_buffer(uint8_t idx, uint8_t *buffer,
											uint16_t size)
{
	int8_t index = nuc505_usbd_ep(idx | NUC505_USBD_EPIN);
	uint32_t i;

	if ((index < 0) || (index >= NUC505_USBD_EP_NUM))
	{
		return VSFERR_FAIL;
	}

	if (0 == idx)
	{
		for (i = 0; i < size; i++)
		{
			USBD->CEPDAT_BYTE = buffer[i];
		}
		return VSFERR_NONE;
	}
	else if (index > 1)
	{
		index -= 2;
		for (i = 0; i < size; i++)
		{
			NUC505_USBD_EP_REG(index, EPDAT_BYTE) = buffer[i];
		}
		return VSFERR_NONE;
	}
	return VSFERR_BUG;
}

vsf_err_t nuc505_usbd_ep_set_OUT_dbuffer(uint8_t idx)
{
	return VSFERR_NONE;
}

bool nuc505_usbd_ep_is_OUT_dbuffer(uint8_t idx)
{
	return false;
}

vsf_err_t nuc505_usbd_ep_switch_OUT_buffer(uint8_t idx)
{
	return VSFERR_NONE;
}

vsf_err_t nuc505_usbd_ep_set_OUT_epsize(uint8_t idx, uint16_t epsize)
{
	int8_t index = nuc505_usbd_get_free_ep(idx | NUC505_USBD_EPOUT);
	if (index < 0)
	{
		return VSFERR_FAIL;
	}

	if (0 == idx)
	{
		// has already been(will be) allocated in set_IN_epsize
		return VSFERR_NONE;
	}
	else if (index > 1)
	{
		int8_t index_in = nuc505_usbd_ep(idx | NUC505_USBD_EPIN);

		index -= 2;
		if (index_in < 0)
		{
			if ((EP_Cfg_Ptr - epsize) > 0x1000)
			{
				return VSFERR_NOT_ENOUGH_RESOURCES;
			}

			EP_Cfg_Ptr -= epsize & 1 ? epsize + 1 : epsize;
			NUC505_USBD_EP_REG(index, EPBUFSTART) = EP_Cfg_Ptr;
			NUC505_USBD_EP_REG(index, EPBUFEND) = EP_Cfg_Ptr + epsize - 1;
		}
		else if (index_in > 1)
		{
			index_in -= 2;
			NUC505_USBD_EP_REG(index, EPBUFSTART) =\
									NUC505_USBD_EP_REG(index_in, EPBUFSTART);
			NUC505_USBD_EP_REG(index, EPBUFEND) =\
									NUC505_USBD_EP_REG(index_in, EPBUFEND);
		}
		else
		{
			return VSFERR_BUG;
		}
		NUC505_USBD_EP_REG(index, EPMPS) = epsize;
		NUC505_USBD_EP_REG(index, EPCFG) &= ~USB_EP_CFG_DIR_IN;
		NUC505_USBD_EP_REG(index, EPCFG) &= ~(0xF << 4);
		NUC505_USBD_EP_REG(index, EPCFG) |= (idx & 0x0F) << 4;
		return VSFERR_NONE;
	}
	return VSFERR_BUG;
}

uint16_t nuc505_usbd_ep_get_OUT_epsize(uint8_t idx)
{
	int8_t index = nuc505_usbd_ep(idx | NUC505_USBD_EPOUT);
	if ((index < 0) || (index >= NUC505_USBD_EP_NUM))
	{
		return 0;
	}

	if (0 == idx)
	{
		return max_ctl_ep_size;
	}
	else if (index > 1)
	{
		index -= 2;
		return NUC505_USBD_EP_REG(index, EPMPS);
	}
	return 0;
}

vsf_err_t nuc505_usbd_ep_set_OUT_stall(uint8_t idx)
{
	int8_t index = nuc505_usbd_ep(idx | NUC505_USBD_EPOUT);
	if ((index < 0) || (index >= NUC505_USBD_EP_NUM))
	{
		return VSFERR_FAIL;
	}

	if (0 == idx)
	{
		USBD->CEPCTL = 2;
		USBD->CEPCTL |= USB_CEPCTL_FLUSH;
		return VSFERR_NONE;
	}
	else if (index > 1)
	{
		index -= 2;
		NUC505_USBD_EP_REG(index, EPRSPCTL) =
			(NUC505_USBD_EP_REG(index, EPRSPCTL) & 0xF7) | USB_EP_RSPCTL_HALT;
		NUC505_USBD_EP_REG(index, EPRSPCTL) |= USB_EP_RSPCTL_FLUSH;
		return VSFERR_NONE;
	}
	return VSFERR_BUG;
}

vsf_err_t nuc505_usbd_ep_clear_OUT_stall(uint8_t idx)
{
	int8_t index = nuc505_usbd_ep(idx | NUC505_USBD_EPOUT);
	if ((index < 0) || (index >= NUC505_USBD_EP_NUM))
	{
		return VSFERR_FAIL;
	}

	if (0 == idx)
	{
		USBD->CEPCTL = USBD->CEPCTL & ~3;
		return VSFERR_NONE;
	}
	else if (index > 1)
	{
		index -= 2;
		NUC505_USBD_EP_REG(index, EPRSPCTL) |= USB_EP_RSPCTL_FLUSH;
		NUC505_USBD_EP_REG(index, EPRSPCTL) &= ~USB_EP_RSPCTL_HALT;
		NUC505_USBD_EP_REG(index, EPRSPCTL) |= USB_EP_RSPCTL_TOGGLE;
		return VSFERR_NONE;
	}
	return VSFERR_BUG;
}

bool nuc505_usbd_ep_is_OUT_stall(uint8_t idx)
{
	int8_t index = nuc505_usbd_ep(idx | NUC505_USBD_EPOUT);
	if ((index < 0) || (index >= NUC505_USBD_EP_NUM))
	{
		return true;
	}

	if (0 == idx)
	{
		return (USBD->CEPCTL & USB_CEPCTL_STALL) > 0;
	}
	else if (index > 1)
	{
		index -= 2;
		return (NUC505_USBD_EP_REG(index, EPRSPCTL) & USB_EP_RSPCTL_HALT) > 0;
	}
	return VSFERR_BUG;
}

vsf_err_t nuc505_usbd_ep_reset_OUT_toggle(uint8_t idx)
{
	int8_t index = nuc505_usbd_ep(idx | NUC505_USBD_EPOUT);
	if ((index < 0) || (index >= NUC505_USBD_EP_NUM))
	{
		return VSFERR_FAIL;
	}

	if (0 == idx)
	{
		return VSFERR_NOT_SUPPORT;
	}
	else if (index > 1)
	{
		index -= 2;
		NUC505_USBD_EP_REG(index, EPRSPCTL) |= USB_EP_RSPCTL_TOGGLE;
		return VSFERR_NONE;
	}
	return VSFERR_BUG;
}

vsf_err_t nuc505_usbd_ep_toggle_OUT_toggle(uint8_t idx)
{
	return VSFERR_NOT_SUPPORT;
}

uint16_t nuc505_usbd_ep_get_OUT_count(uint8_t idx)
{
	int8_t index = nuc505_usbd_ep(idx | NUC505_USBD_EPOUT);
	if ((index < 0) || (index >= NUC505_USBD_EP_NUM))
	{
		return 0;
	}

	if (0 == idx)
	{
		return nuc505_status_out ? 0 : USBD->CEPRXCNT;
	}
	else if (index > 1)
	{
		index -= 2;
		return NUC505_USBD_EP_REG(index, EPDATCNT) & 0xFF;
	}
	return 0;
}

vsf_err_t nuc505_usbd_ep_read_OUT_buffer(uint8_t idx, uint8_t *buffer,
											uint16_t size)
{
	int8_t index = nuc505_usbd_ep(idx | NUC505_USBD_EPOUT);
	uint32_t i;

	if ((index < 0) || (index >= NUC505_USBD_EP_NUM))
	{
		return VSFERR_FAIL;
	}

	if (0 == idx)
	{
		for (i = 0; i < size; i++)
		{
			buffer[i] = USBD->CEPDAT_BYTE;
		}
		return VSFERR_NONE;
	}
	else if (index > 1)
	{
		index -= 2;
		size = min(size, NUC505_USBD_EP_REG(index, EPMPS));
		for (i = 0; i < size; i++)
		{
			buffer[i] = NUC505_USBD_EP_REG(index, EPDAT_BYTE);
		}
		return VSFERR_NONE;
	}
	return VSFERR_BUG;
}

vsf_err_t nuc505_usbd_ep_enable_OUT(uint8_t idx)
{
	int8_t index = nuc505_usbd_ep(idx | NUC505_USBD_EPOUT);
	if ((index < 0) || (index >= NUC505_USBD_EP_NUM))
	{
		return VSFERR_FAIL;
	}

	if (0 == idx)
	{
		if (nuc505_setup_status_IN)
		{
			USBD->CEPCTL = USB_CEPCTL_NAKCLR;
		}
		return VSFERR_NONE;
	}
	else if (index > 1)
	{
		index -= 2;
		NUC505_USBD_EP_REG(index, EPCFG) |= USB_EP_CFG_VALID;
		return VSFERR_NONE;
	}
	return VSFERR_BUG;
}

void USB_Istr(void)
{
	static uint32_t IrqStL, IrqSt;

	IrqStL = USBD->GINTSTS;
	IrqStL &= USBD->GINTEN;

	if (!IrqStL)
	{
		return;
	}

	// USB interrupt
	if (IrqStL & USBD_GINTSTS_USBIF_Msk) {
		IrqSt = USBD->BUSINTSTS;
		IrqSt &= USBD->BUSINTEN;

		if (IrqSt & USBD_BUSINTSTS_RSTIF_Msk) {
			if (nuc505_usbd_callback.on_reset != NULL)
			{
				nuc505_usbd_callback.on_reset(\
						nuc505_usbd_callback.param);
			}
			USBD->BUSINTSTS = USBD_BUSINTSTS_RSTIF_Msk;
			USBD->CEPINTSTS = 0x1ffc;
		}
	}

	// CEP interrupt
	if (IrqStL & USBD_GINTSTS_CEPIF_Msk) {
		IrqSt = USBD->CEPINTSTS;
		IrqSt &= USBD->CEPINTEN;

		if (IrqSt & USBD_CEPINTSTS_TXPKIF_Msk) {
			USBD->CEPINTSTS = USBD_CEPINTSTS_TXPKIF_Msk;

			if (nuc505_usbd_callback.on_in != NULL)
			{
				nuc505_usbd_callback.on_in(nuc505_usbd_callback.param, 0);
			}
		}
		if (IrqSt & USBD_CEPINTSTS_RXPKIF_Msk) {
			USBD->CEPINTSTS = USBD_CEPINTSTS_RXPKIF_Msk;

			nuc505_status_out = false;
			if (nuc505_usbd_callback.on_out != NULL)
			{
				nuc505_usbd_callback.on_out(nuc505_usbd_callback.param, 0);
			}
		}

		if (IrqSt & USBD_CEPINTSTS_STSDONEIF_Msk) {
			USBD->CEPINTSTS = USBD_CEPINTSTS_STSDONEIF_Msk;

			if (!nuc505_setup_status_IN)
			{
				if (nuc505_usbd_callback.on_in != NULL)
				{
					nuc505_usbd_callback.on_in(nuc505_usbd_callback.param, 0);
				}
			}
			else
			{
				nuc505_status_out = true;
				if (nuc505_usbd_callback.on_out != NULL)
				{
					nuc505_usbd_callback.on_out(nuc505_usbd_callback.param, 0);
				}
			}
		}

		if (IrqSt & USBD_CEPINTSTS_SETUPPKIF_Msk) {
			USBD->CEPINTSTS = USBD_CEPINTSTS_SETUPPKIF_Msk;

			if (nuc505_usbd_callback.on_setup != NULL)
			{
				nuc505_usbd_callback.on_setup(\
						nuc505_usbd_callback.param);
			}
		}
	}

	// EP interrupt
	if (IrqStL & (~3))
	{
		int i;
		for (i = 0; i < 12; i++)
		{
			if (IrqStL & (1 << (i + 2)))
			{
				IrqSt = NUC505_USBD_EP_REG(i, EPINTSTS);

				if (IrqSt & USBD_EPINTSTS_TXPKIF_Msk)
				{
					NUC505_USBD_EP_REG(i, EPINTSTS) = USBD_EPINTSTS_TXPKIF_Msk;
					if (nuc505_usbd_callback.on_in != NULL)
					{
						nuc505_usbd_callback.on_in(\
								nuc505_usbd_callback.param,\
								nuc505_usbd_epaddr[i + 2] & 0x0F);
					}
				}
				if (IrqSt & USBD_EPINTSTS_RXPKIF_Msk)
				{
					NUC505_USBD_EP_REG(i, EPINTSTS) = USBD_EPINTSTS_RXPKIF_Msk;
					if (nuc505_usbd_callback.on_out != NULL)
					{
						nuc505_usbd_callback.on_out(\
								nuc505_usbd_callback.param,\
								nuc505_usbd_epaddr[i + 2] & 0x0F);
					}
				}
			}
		}
	}
}

ROOTFUNC void USBD_IRQHandler(void)
{
	USB_Istr();
}

#endif

