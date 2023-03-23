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
#include <logging/log_core.h>

/* Enable debug logging when programming the EEPROM for debugging purposes */
#if defined(CONFIG_VPD_PROGRAM_EEPROM) && CONFIG_VPD_PROGRAM_EEPROM == 1
LOG_MODULE_REGISTER(vpd, LOG_LEVEL_DBG);
#else
LOG_MODULE_REGISTER(vpd, LOG_LEVEL_ERR);
#endif

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

#if defined(CONFIG_VPD_PROGRAM_EEPROM) && CONFIG_VPD_PROGRAM_EEPROM == 1

static int _write_vpd(const uint8_t *data, uint16_t offset, uint16_t length)
{
	for (uint16_t i = 0; i < length; i++) {
		if (eeprom_write_byte(EEPROM_VPD_OFFSET + offset + i, data[offset + i])) {
			LOG_ERR("Could not write byte %u", offset + i);
			return -1;
		}
	}
	return 0;
}

#define write_vpd(_prog_vpd, _member)	_write_vpd(_prog_vpd.raw,	\
		offsetof(union emi_eeprom_vpd, _member), sizeof(_prog_vpd._member))

#endif

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

	/* TODO: Handle programming failures? */
	const union emi_eeprom_vpd prog_vpd = {
		.header = {
			.magic = VPD_MAGIC,
			.revision = VPD_LATEST_REVISION,
		},
		.serial_number = CONFIG_VPD_SERIAL_NUMBER,
		.part_number = CONFIG_VPD_PART_NUMBER,
		.profile = CONFIG_VPD_PROFILE,
	};

#if defined(CONFIG_VPD_PROGRAM_EVERYTHING) && CONFIG_VPD_PROGRAM_EVERYTHING == 1

	if (write_vpd(prog_vpd, raw))
		return;

#else

	if (CONFIG_VPD_SERIAL_NUMBER[0] != '\0')
		if (write_vpd(prog_vpd, serial_number))
			return;

	if (CONFIG_VPD_PART_NUMBER[0] != '\0')
		if (write_vpd(prog_vpd, part_number))
			return;

	if (CONFIG_VPD_PROFILE != 0)
		if (write_vpd(prog_vpd, profile))
			return;

#endif	/* VPD_PROGRAM_EVERYTHING */

#endif	/* VPD_PROGRAM_EEPROM */

	for (uint16_t i = 0; i < sizeof(vpd_shadow); i++) {
		if (eeprom_read_byte(EEPROM_VPD_OFFSET + i, &vpd_shadow.raw[i])) {
			LOG_ERR("Could not read byte %u", i);
			/* Invalidate the magic */
			vpd_shadow.header.magic = 0;
			return;
		}
	}

	LOG_HEXDUMP_DBG(vpd_shadow.raw, sizeof(vpd_shadow.raw), "New VPD:");
}
