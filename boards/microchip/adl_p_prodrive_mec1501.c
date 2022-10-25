/*
 * Copyright (c) 2022 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <soc.h>
#include <drivers/gpio.h>
#include "i2c_hub.h"
#include <logging/log.h>
#include "gpio_ec.h"
#include "espi_hub.h"
#include "board.h"
#include "board_config.h"
#include "adl_p_prodrive_mec1501.h"
#include "vci.h"

LOG_MODULE_DECLARE(board, CONFIG_BOARD_LOG_LEVEL);

uint8_t platformskutype;

/** @brief EC FW app owned gpios list.
 *
 * This list is not exhaustive, it do not include driver-owned pins,
 * the initialization is done as part of corresponding Zephyr pinmux driver.
 * BSP drivers are responsible to control gpios in soc power transitions and
 * system transitions.
 *
 * Note: Pins not assigned to any app function are identified with their
 * original pin number instead of signal
 *
 */

/* APP-owned gpios */
struct gpio_ec_config mecc1501_cfg[] = {
	{ PM_SLP_SUS,		GPIO_INPUT },
	{ EC_SPI_CS1_N,		GPIO_OUTPUT_HIGH},
	/* MIC privacy switch, not used */
	{ EC_GPIO_011,		GPIO_DISCONNECTED },
	{ RSMRST_PWRGD_MAF_P,	GPIO_INPUT },
	{ CPU_C10_GATE,		GPIO_INPUT },
	/* EC control considered security issue. Savings 0.5 mA */
	{ EC_PCH_SPI_OE_N,	GPIO_DISCONNECTED },
	/* Not used. It's connected to 3.3K pull-up */
	{ PECI_MUX_CTRL,	GPIO_DISCONNECTED },
	{ SMC_LID,		GPIO_INPUT  | GPIO_INT_EDGE_BOTH },
	/* PCH SYS_PWROK is connected to Silego. Savings 0.100 mA */
	{ EC_GPIO_043,		GPIO_DISCONNECTED },
	{ EC_GPIO_050,		GPIO_INPUT },
	/* IO expander 0, has nothing to be handled dynamically */
	{ PCA9555_0_R_INT_N,	GPIO_DISCONNECTED },
	{ STD_ADP_PRSNT,	GPIO_INPUT },
	{ ALL_SYS_PWRGD,	GPIO_INPUT },
	//{ ALL_SYS_PWRGD,	GPIO_OUTPUT_LOW | GPIO_OPEN_DRAIN },
	/* VREF2_ADC not connected, floating. Savings 0.6 mA */
	{ EC_GPIO_067,		GPIO_DISCONNECTED },
	/* MIC privacy ec, not used */
	{ EC_GPIO_100,		GPIO_DISCONNECTED },
	{ PM_PWRBTN,		GPIO_OUTPUT_HIGH | GPIO_OPEN_DRAIN },
	{ EC_SMI,			GPIO_DISCONNECTED },
	/* VTR2 strap not used by EC FW */
	{ EC_GPIO_104,		GPIO_DISCONNECTED },
	{ PCH_PWROK,		GPIO_OUTPUT_LOW | GPIO_OPEN_DRAIN },
	{ WAKE_SCI,		GPIO_OUTPUT_HIGH | GPIO_OPEN_DRAIN },
#ifdef CONFIG_DNX_EC_ASSISTED_TRIGGER
	{ DNX_FORCE_RELOAD_EC,	GPIO_OUTPUT_LOW},
#else
	{ DNX_FORCE_RELOAD_EC,	GPIO_DISCONNECTED},
#endif
	{ PM_BATLOW,		GPIO_OUTPUT_LOW | GPIO_OPEN_DRAIN },
	{ CATERR_LED_DRV,	GPIO_INPUT },
	{ HB_NVDC_SEL,		GPIO_INPUT },
	{ BATT_ID_N,		GPIO_INPUT },
	{ PWRBTN_EC_IN_N,	GPIO_INPUT | GPIO_INT_EDGE_BOTH },

	{ BC_ACOK,		GPIO_INPUT },
	/* Path not connected. Savings 0.5 mA) */
	{ SX_EXIT_HOLDOFF_N,	GPIO_DISCONNECTED },
	/* Not used */
	{ PM_SLP_S0_CS,		GPIO_DISCONNECTED },
	{ RETIMER_FORCE_PWR_BTP_EC_R, GPIO_INPUT },
	{ PM_DS3,		GPIO_OUTPUT_LOW },
	/* Not used in new LPM design */
	{ WAKE_CLK,		GPIO_DISCONNECTED },
	{ EC_PG3_EXIT,		GPIO_OUTPUT_LOW },
	{ EC_PWRBTN_LED,	GPIO_OUTPUT_LOW },
	{ PROCHOT,		GPIO_OUTPUT_HIGH | GPIO_OPEN_DRAIN },

	{ EC_GPIO_131, GPIO_DISCONNECTED },
	{ EC_GPIO_130, GPIO_DISCONNECTED },
	{ EC_GPIO_154, GPIO_DISCONNECTED },
	{ EC_GPIO_144, GPIO_DISCONNECTED },
	{ EC_GPIO_143, GPIO_DISCONNECTED },
	{ EC_GPIO_142, GPIO_DISCONNECTED },
	{ EC_GPIO_141, GPIO_DISCONNECTED },

	{ EC_GPIO_014, GPIO_DISCONNECTED },
	{ EC_GPIO_062, GPIO_DISCONNECTED },
	{ EC_GPIO_015, GPIO_DISCONNECTED },
	{ EC_GPIO_035, GPIO_DISCONNECTED },
	{ EC_GPIO_105, GPIO_DISCONNECTED },
	{ EC_GPIO_255, GPIO_DISCONNECTED },
	{ EC_GPIO_254, GPIO_DISCONNECTED },
	{ EC_GPIO_244, GPIO_DISCONNECTED },
	{ EC_GPIO_175, GPIO_DISCONNECTED },
	{ EC_GPIO_060, GPIO_DISCONNECTED },
	{ EC_GPIO_100, GPIO_DISCONNECTED },
	{ EC_GPIO_011, GPIO_DISCONNECTED },
	{ EC_GPIO_127, GPIO_DISCONNECTED },

	{ EC_GPIO_157, GPIO_DISCONNECTED },
	{ EC_GPIO_107, GPIO_DISCONNECTED },
	{ EC_GPIO_112, GPIO_DISCONNECTED },
	{ EC_GPIO_113, GPIO_DISCONNECTED },
	{ EC_GPIO_120, GPIO_DISCONNECTED },

	{ PWR_OK, GPIO_INPUT },
	{ EC_GPIO_122, GPIO_DISCONNECTED },
	{ EC_GPIO_123, GPIO_DISCONNECTED },
	{ EC_GPIO_124, GPIO_DISCONNECTED },
	{ EC_GPIO_124, GPIO_DISCONNECTED },
	{ EC_GPIO_125, GPIO_DISCONNECTED },
	{ EC_GPIO_126, GPIO_DISCONNECTED },
	{ EC_GPIO_151, GPIO_DISCONNECTED },
	{ EC_GPIO_152, GPIO_DISCONNECTED },
	{ EC_GPIO_132, GPIO_DISCONNECTED },

	/* Both PD controller share same interrupt line. Savings 0.2 mA*/
	{ TYPEC_EC_SMBUS_ALERT_1_R, GPIO_DISCONNECTED },

	{ EC_GPIO_034, GPIO_DISCONNECTED },
	{ PEG_PLI_N_DG2,	GPIO_DISCONNECTED },
};

struct gpio_ec_config mecc1501_cfg_sus[] =  {
};

struct gpio_ec_config mecc1501_cfg_res[] =  {
};

#ifdef CONFIG_THERMAL_MANAGEMENT
/**
 * @brief Fan device table.
 *
 * This table lists the supported fan devices for board. By default, each
 * board is assigned one fan for CPU.
 */
static struct fan_dev fan_tbl[] = {
/*	PWM_CH_##	TACH_CH_##  */
	{ PWM_CH_00,	TACH_CH_00 }, /* CPU Fan */
};

void board_fan_dev_tbl_init(uint8_t *pmax_fan, struct fan_dev **pfan_tbl)
{
	*pfan_tbl = fan_tbl;
	*pmax_fan = ARRAY_SIZE(fan_tbl);
}

void board_therm_sensor_list_init(uint8_t therm_sensors[])
{
	//TODO check for thermal sensors
}
#endif

int board_init(void)
{
	int ret;

	bgpo_disable();

	ret = gpio_init();
	if (ret) {
		LOG_ERR("Failed to initialize gpio devs: %d", ret);
		return ret;
	}

	ret = i2c_hub_config(I2C_0);
	if (ret) {
		return ret;
	}

	ret = i2c_hub_config(I2C_1);
	if (ret) {
		return ret;
	}

#if DT_NODE_HAS_STATUS(DT_INST(2, microchip_xec_i2c), okay)
	ret = i2c_hub_config(I2C_2);
	if (ret) {
		LOG_ERR("i2c port not configured, Enable i2c port 5 in dts.");
	} else {
	}
#endif
	/*
	ret = read_board_id();
	if (ret) {
		LOG_ERR("Failed to fetch brd id: %d", ret);
		return ret;
	}
	*/

	/* MEC15xx has by default GPIO input disabled.
	 * Need to configure strap prior to decide boot mode
	 */
	gpio_configure_pin(G3_SAF_DETECT, GPIO_INPUT);

	detect_boot_mode();

	ret = gpio_configure_array(mecc1501_cfg, ARRAY_SIZE(mecc1501_cfg));
	if (ret) {
		LOG_ERR("%s: %d", __func__, ret);
		return ret;
	}

	/* In MAF, boot ROM already made this pin output and high, so we must
	 * keep it like that during the boot phase in order to avoid espi reset
	 */
	if (espihub_boot_mode() == FLASH_BOOT_MODE_MAF) {
		/* Ensure GPIO mode for pins reconfigure due to QMSPI device */
		gpio_force_configure_pin(RSMRST_PWRGD_MAF_P,
					 GPIO_INPUT | GPIO_PULL_UP);
		gpio_force_configure_pin(PM_RSMRST_MAF_P, GPIO_OUTPUT_HIGH);

		/* LPM optimizations */
		gpio_force_configure_pin(G3_SAF_DETECT, GPIO_DISCONNECTED);
		gpio_force_configure_pin(PM_RSMRST_G3SAF_P, GPIO_DISCONNECTED);
		gpio_force_configure_pin(EC_GPIO_002, GPIO_DISCONNECTED);
		gpio_force_configure_pin(EC_GPIO_056, GPIO_DISCONNECTED);
		gpio_force_configure_pin(EC_GPIO_223, GPIO_DISCONNECTED);
		gpio_force_configure_pin(EC_GPIO_224, GPIO_DISCONNECTED);
		gpio_force_configure_pin(EC_GPIO_016, GPIO_DISCONNECTED);
	} else {
		gpio_configure_pin(RSMRST_PWRGD_G3SAF_P, GPIO_INPUT);
		gpio_configure_pin(PM_RSMRST_G3SAF_P, GPIO_OUTPUT_LOW);
	}
	return 0;
}

int board_suspend(void)
{
	int ret;

	ret = gpio_configure_array(mecc1501_cfg_sus,
				   ARRAY_SIZE(mecc1501_cfg_sus));
	if (ret) {
		LOG_ERR("%s: %d", __func__, ret);
		return ret;
	}

	return 0;
}

int board_resume(void)
{
	int ret;

	ret = gpio_configure_array(mecc1501_cfg_res,
				   ARRAY_SIZE(mecc1501_cfg_res));
	if (ret) {
		LOG_ERR("%s: %d", __func__, ret);
		return ret;
	}

	return 0;
}
