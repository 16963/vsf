#ifndef __STM32_CORE_H_INCLUDED__
#define __STM32_CORE_H_INCLUDED__

enum stm32_clksrc_t
{
	STM32_CLKSRC_HSE = 1,
	STM32_CLKSRC_HSI = 0,
	STM32_CLKSRC_PLL = 2
};
enum stm32_pllsrc_t
{
	STM32_PLLSRC_HSE,
	STM32_PLLSRC_HSEd2,
	STM32_PLLSRC_HSId2
};
enum stm32_rtcsrc_t
{
	STM32_RTCSRC_HSEd128,
	STM32_RTCSRC_LSE,
	STM32_RTCSRC_LSI
};
enum stm32_hse_type_t
{
	STM32_HSE_TYPE_CLOCK,
	STM32_HSE_TYPE_CRYSTAL
};
enum debug_setting_t
{
	STM32_DBG_JTAG_SWD = 0,
	STM32_DBG_SWD = 2,
	STM32_DBG_NONE = 4
};
struct stm32_info_t
{
	uint8_t priority_group;
	uint32_t vector_table;
	
	enum stm32_clksrc_t clksrc;
	enum stm32_pllsrc_t pllsrc;
	enum stm32_rtcsrc_t rtcsrc;
	enum stm32_hse_type_t hse_type;
	
	uint32_t osc_freq_hz;
	uint32_t pll_freq_hz;
	uint32_t ahb_freq_hz;
	uint32_t apb1_freq_hz;
	uint32_t apb2_freq_hz;
	
	uint8_t flash_latency;
	enum debug_setting_t debug_setting;
	
	// calculated internally
	uint32_t sys_freq_hz;
};

vsf_err_t stm32_interface_init(void *p);
vsf_err_t stm32_interface_fini(void *p);
vsf_err_t stm32_interface_reset(void *p);
uint32_t stm32_interface_get_stack(void);
vsf_err_t stm32_interface_set_stack(uint32_t sp);
void stm32_interface_sleep(uint32_t mode);
vsf_err_t stm32_interface_get_info(struct stm32_info_t **info);
vsf_err_t stm32_interface_pendsv_config(void (*on_pendsv)(void *), void *param);
vsf_err_t stm32_interface_pendsv_trigger(void);

uint32_t stm32_uid_get(uint8_t *buffer, uint32_t size);

vsf_err_t stm32_delay_delayms(uint16_t ms);
vsf_err_t stm32_delay_delayus(uint16_t us);

vsf_err_t stm32_tickclk_init(void);
vsf_err_t stm32_tickclk_fini(void);
vsf_err_t stm32_tickclk_start(void);
vsf_err_t stm32_tickclk_stop(void);
uint32_t stm32_tickclk_get_count(void);
vsf_err_t stm32_tickclk_config_cb(void (*callback)(void*), void *param);

#endif	// __STM32_CORE_H_INCLUDED__
