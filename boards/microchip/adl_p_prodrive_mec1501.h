/*
 * Copyright (c) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <soc.h>
#include "mec150x_pin.h"
#include "common_mec1501.h"

#ifndef __ADL_P_PRODRIVE_MEC1501_H__
#define __ADL_P_PRODRIVE_MEC1501_H__

extern uint8_t platformskutype;

#define KSC_PLAT_NAME                   "PDVATLAS"

#define PLATFORM_DATA(x, y)  ((x) | ((y) << 8))

/* In ADL board id is 6-bits */
#define BOARD_ID_MASK        0x003Fu
#define BOM_ID_MASK          0x01C0u
#define FAB_ID_MASK          0x0600u
#define HW_STRAP_MASK        0xF800u
#define HW_ID_MASK           (FAB_ID_MASK|BOM_ID_MASK|BOARD_ID_MASK)
#define BOARD_ID_OFFSET      0u
#define BOM_ID_OFFSET        6u
#define FAB_ID_OFFSET        9u
#define HW_STRAP_OFFSET      11u

/* Support board ids */
#define BRD_ID_ATLAS_TYPE6	0x10u
#define BRD_ID_ATLAS_TYPE10	0x10u


/* I2C addresses */
#define EEPROM_DRIVER_I2C_ADDR          0x50

/* Signal to gpio mapping for MEC1501 based ADL-P is described here */

#define PM_SLP_SUS			EC_GPIO_000
#define EC_SPI_CS1_N			EC_GPIO_002
#define RSMRST_PWRGD_G3SAF_P		EC_GPIO_012
#define RSMRST_PWRGD_MAF_P		EC_GPIO_227
#define RSMRST_PWRGD			RSMRST_PWRGD_MAF_P
#define G3_SAF_DETECT			EC_GPIO_013
#define CPU_C10_GATE			EC_GPIO_022
#define EC_PCH_SPI_OE_N			EC_GPIO_024
#define PECI_MUX_CTRL			EC_GPIO_025
#define SMC_LID				EC_GPIO_033
#define PEG_PLI_N_DG2			EC_GPIO_036
#define SYS_PWROK			EC_GPIO_043
#define PCA9555_0_R_INT_N		EC_GPIO_051
#define STD_ADP_PRSNT			EC_GPIO_052

#define PM_RSMRST_G3SAF_P		EC_GPIO_054
#define PM_RSMRST_MAF_P			EC_GPIO_055
#define PM_RSMRST			((boot_mode_maf == 1) ? \
					 PM_RSMRST_MAF_P : \
					 PM_RSMRST_G3SAF_P)

#define ALL_SYS_PWRGD			EC_GPIO_057
/* We poll this GPIO in MAF mode in order to sense the input signal040_076.
 * This pin was already configured in pinmux as ALT mode 1 NOT GPIO
 */
#define ESPI_RESET_MAF			EC_GPIO_061

#define PM_PWRBTN			EC_GPIO_101
#define EC_SMI				EC_GPIO_102
#define PCH_PWROK			EC_GPIO_106
#define WAKE_SCI			EC_GPIO_114
#define DNX_FORCE_RELOAD_EC		EC_GPIO_115
#define PM_BATLOW			EC_GPIO_140
#define CATERR_LED_DRV			EC_GPIO_153
#define HB_NVDC_SEL			EC_GPIO_161
#define BATT_ID_N			EC_GPIO_162
#define PWRBTN_EC_IN_N			EC_GPIO_163
#define BC_ACOK				EC_GPIO_172
#define SX_EXIT_HOLDOFF_N		EC_GPIO_175

#define PM_SLP_S0_CS			EC_GPIO_221
#define RETIMER_FORCE_PWR_BTP_EC_R	EC_GPIO_222
#define PM_DS3				EC_GPIO_226
#define WAKE_CLK			EC_GPIO_241

#define TYPEC_EC_SMBUS_ALERT_1_R	EC_GPIO_245

#define EC_PG3_EXIT			EC_GPIO_250
#define PROCHOT				EC_GPIO_253
#define EC_PWRBTN_LED			EC_GPIO_156
#define PWR_OK				EC_GPIO_121

#define EC_HSID_3			EC_GPIO_107
#define EC_HSID_2			EC_GPIO_112
#define EC_HSID_1			EC_GPIO_113
#define EC_HSID_0			EC_GPIO_120

#define FAN_PWR_DISABLE_N		EC_DUMMY_GPIO_LOW
#define VIRTUAL_BAT			EC_DUMMY_GPIO_LOW
#define VIRTUAL_DOCK			EC_DUMMY_GPIO_LOW
#define HOME_BUTTON			EC_DUMMY_GPIO_HIGH
#define VOL_UP				EC_DUMMY_GPIO_LOW
#define VOL_DOWN			EC_DUMMY_GPIO_LOW
#define DG2_PRESENT			EC_DUMMY_GPIO_LOW
#define PEG_RTD3_COLD_MOD_SW_R		EC_DUMMY_GPIO_LOW
#define THERM_STRAP			EC_DUMMY_GPIO_HIGH
#define TIMEOUT_DISABLE			EC_DUMMY_GPIO_HIGH

/* Device instance names */
#define I2C_BUS_0			DT_LABEL(DT_NODELABEL(i2c_smb_0))
#define I2C_BUS_1			DT_LABEL(DT_NODELABEL(i2c_smb_1))
#if DT_NODE_HAS_STATUS(DT_INST(2, microchip_xec_i2c), okay)
#define I2C_BUS_2			DT_LABEL(DT_NODELABEL(i2c_smb_2))
#endif
#define ESPI_0				DT_LABEL(DT_NODELABEL(espi0))
#define ESPI_SAF_0			DT_LABEL(DT_NODELABEL(espi_saf0))
#define SPI_0				DT_LABEL(DT_NODELABEL(spi0))
#define ADC_CH_BASE			DT_LABEL(DT_NODELABEL(adc0))
#define PECI_0_INST			DT_LABEL(DT_NODELABEL(peci0))
#define WDT_0				DT_LABEL(DT_NODELABEL(wdog))

/* Button/Switch Initial positions */
#define PWR_BTN_INIT_POS		1
#define VOL_UP_INIT_POS			1
#define VOL_DN_INIT_POS			1
#define LID_INIT_POS			1
#define HOME_INIT_POS			1
#define SLATEMODE_INIT_POS		1
#define IOEXP_INIT_POS			1
#define VIRTUAL_BAT_INIT_POS		1
#define VIRTUAL_DOCK_INIT_POS		1

#endif /* __ADL_P_PRODRIVE_MEC1501_H__ */
