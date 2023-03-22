/*
 * Copyright (c) 2022-2023 9elements Cyber Security
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>

#include "emi.h"
#include "eeprom.h"
#include "vpd_section.h"

#include <logging/log.h>
LOG_MODULE_REGISTER(vpd, CONFIG_EEPROM_LOG_LEVEL);

#define VPD_MAGIC	0x56504453  /* 'VPDS' */

/*
 * Increment this value whenever new fields are added to the structures.
 * Furthermore, adapt the `get_emi_eeprom_vpd()` function accordingly to
 * provide fallback values for newly-added fields.
 */
#define VPD_LATEST_REVISION	1

struct __packed emi_eeprom_vpd_header {
	uint32_t magic;
	uint8_t revision;
	uint8_t _rfu[15];	/* Reserved for Future Use */
};

/* For backwards compatibility reasons, do NOT reuse enum values! */
enum atlas_profile {
	ATLAS_PROF_UNPROGRAMMED		= 0,	/* EEPROM not initialised */
	ATLAS_PROF_DEFAULT		= 1,
	ATLAS_PROF_REALTIME_PERFORMANCE	= 2,
	ATLAS_PROF_THEMIS_LED_CONFIG	= 3,
};

#define ATLAS_SN_PN_LENGTH	20

#define EMI_EEPROM_LAYOUT_LENGTH (			\
		sizeof(struct emi_eeprom_vpd_header) +	\
		ATLAS_SN_PN_LENGTH +			\
		ATLAS_SN_PN_LENGTH +			\
		sizeof(uint16_t)			\
	)

union emi_eeprom_vpd {
	struct __packed {
		struct emi_eeprom_vpd_header header;
		char serial_number[ATLAS_SN_PN_LENGTH];	/* xx-xx-xxx-xxx */
		char part_number[ATLAS_SN_PN_LENGTH];	/* xxx-xxxx-xxxx.Rxx */
		uint16_t profile;
	};
	uint8_t raw[EMI_EEPROM_LAYOUT_LENGTH];
};

#define EEPROM_VPD_OFFSET	0x10

void expose_vpd_section(void)
{
	static __attribute__((aligned(4))) union emi_eeprom_vpd vpd_shadow = { 0 };

	const uint16_t vpd_shadow_aligned_size = ROUND_UP(sizeof(vpd_shadow), sizeof(uint32_t));

	emi_t emi;
	emi_get(&emi, EMI_INSTANCE_0);

	/*
	 * Ensure EMI region base and size are aligned to a DWORD
	 * boundary, as the registers only have DWORD granularity.
	 */
	emi_region_config_t rconf = {
		.base        = (uint32_t)(uintptr_t)&vpd_shadow,
		.read_limit  = vpd_shadow_aligned_size,
		.write_limit = 0
	};

	emi_configure_region(&emi, EMI_REGION_0, &rconf);

#if defined(CONFIG_VPD_PROGRAM_EEPROM) && CONFIG_VPD_PROGRAM_EEPROM == 1

	const union emi_eeprom_vpd example_vpd = {
		.header = {
			.magic = VPD_MAGIC,
			.revision = VPD_LATEST_REVISION,
		},
		.serial_number = CONFIG_VPD_SERIAL_NUMBER,
		.part_number = CONFIG_VPD_PART_NUMBER,
		.profile = CONFIG_VPD_PROFILE,
	};

	LOG_HEXDUMP_DBG(example_vpd.raw, sizeof(example_vpd.raw), "Programmed VPD:");

	for (uint16_t i = 0; i < sizeof(example_vpd); i++) {
		if (eeprom_write_byte(EEPROM_VPD_OFFSET + i, example_vpd.raw[i])) {
			LOG_ERR("Could not write byte %u", i);
			return;
		}
	}
#endif

	for (uint16_t i = 0; i < sizeof(vpd_shadow); i++) {
		if (eeprom_read_byte(EEPROM_VPD_OFFSET + i, &vpd_shadow.raw[i])) {
			LOG_ERR("Could not read byte %u", i);
			return;
		}
	}

	LOG_HEXDUMP_DBG(vpd_shadow.raw, sizeof(vpd_shadow.raw), "Read VPD:");
}
