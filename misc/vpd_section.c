/* VPD Section Setup */

#include <zephyr.h>

#include "emi.h"
#include "eeprom.h"
#include "vpd_section.h"

#define EEPROM_VPD_OFFSET 0x10
#define PAGESIZE          16

/*
 * Note: The following implementation assumes that the size of
 * the vpd section is a multiple of an EEPROM pagesize,
 * i.e. sizeof(vpd_section_t) = n * 16. Furthermore, it assumes,
 * that the vpd section is mapped at address 0x10 within the EEPROM.
 * */

/* <--- structures, enums, etc. ---> */

typedef struct __attribute__((__packed__)) vpd_section {

    uint8_t part_number  [PAGESIZE];
    uint8_t serial_number[PAGESIZE];
    uint8_t profile      [PAGESIZE];

} vpd_section_t;

/* <--- globals ---> */

static uint8_t shared_section[ sizeof(vpd_section_t) ] = { 0x0 };

/* <--- Exposed Functionality ---> */

void expose_vpd_section(void) {

    emi_t emi;
    emi_get(&emi, EMI_INSTANCE_0);

    emi_region_config_t rconf = {

        .base        = (uint32_t) shared_section,
        .read_limit  = sizeof(vpd_section_t),
        .write_limit = 0
    };

    emi_configure_region(&emi, EMI_REGION_0, &rconf);
    uint8_t *base = (uint8_t*) emi.config->region_0_base;

#ifdef CONFIG_WRITE_VPD

    eeprom_write_block(EEPROM_VPD_OFFSET + offsetof(vpd_section_t, part_number), sizeof(CONFIG_VPD_PN)      - 1, CONFIG_VPD_PN);
    eeprom_write_block(EEPROM_VPD_OFFSET + offsetof(vpd_section_t, profile),     sizeof(CONFIG_VPD_PROFILE) - 1, CONFIG_VPD_PROFILE);

#endif

    for(uint16_t i = 0; i < sizeof(vpd_section_t); i += PAGESIZE)
        eeprom_read_block(EEPROM_VPD_OFFSET + i, PAGESIZE, base + i);
}
