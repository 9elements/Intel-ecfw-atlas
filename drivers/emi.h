/* Embedded Memory Interface (EMI) Driver */

/**
 * @brief EMI APIs.
 */

#ifndef __EMI_H__
#define __EMI_H__

#define EMI_SUCCESS 0
#define EMI_FAILURE 1

/*
 * <=== API usage ===>
 *
 * -> retrieve the EMI config space of a specific EMI instance via emi_get()
 * -> the config space can be accessed via the .config field within the emi_t structure
 *
 * */

typedef enum {

    EMI_INSTANCE_0,                                 ///< emi instance 0
    EMI_INSTANCE_1,                                 ///< emi instance 1

    EMI_INSTANCE_END                                ///< emi instance end marker

} EMI_INSTANCE;

typedef enum {

    EMI_REGION_0,                                   ///< emi region 0
    EMI_REGION_1,                                   ///< emi region 1

    EMI_REGION_END                                  ///< emi region end marker

} EMI_REGION;

/*
 * see MEC152x-Data-Sheet-DS00003427A page 228, table 14-6
 * */
typedef struct emi_config_space {

    /* === runtime registers === */

    uint8_t  host_ec_mailbox_rt;                    ///< host-to-ec mailbox (runtime register)
    uint8_t  ec_host_mailbox_rt;                    ///< ec-to-host mailbox (runtime register)

    uint8_t  ec_address_lsb;                        ///< ec address register (LSB)
    uint8_t  ec_address_msb;                        ///< ec address register (MSB)

    uint8_t  ec_data_byte_0;                        ///< ec data byte 0 (LSB)
    uint8_t  ec_data_byte_1;                        ///< ec data byte 1
    uint8_t  ec_data_byte_2;                        ///< ec data byte 2
    uint8_t  ec_data_byte_3;                        ///< ec data byte 3 (MSB)

    uint8_t  interrupt_source_lsb;                  ///< interrupt source (LSB)
    uint8_t  interrupt_source_msb;                  ///< interrupt source (MSB)

    uint8_t  interrupt_mask_lsb;                    ///< interrupt mask (LSB)
    uint8_t  interrupt_mask_msb;                    ///< interrupt mask (MSB)

    uint8_t  application_id;                        ///< application ID

    /* === ec-only registers === */

    uint8_t  reserved[0xf3];                        ///< reserved/unknown memory

    uint8_t  host_ec_mailbox;                       ///< host-to-ec mailbox
    uint8_t  ec_host_mailbox;                       ///< ec-to-host mailbox

    uint8_t  reserved_2[0x2];                       ///< reserved/unknown memory (2)

    uint32_t region_0_base;                         ///< EMI region 0 base address register
    uint16_t region_0_read_limit;                   ///< EMI region 0 read limit register
    uint16_t region_0_write_limit;                  ///< EMI region 0 write limit register

    uint32_t region_1_base;                         ///< EMI region 1 base address register
    uint16_t region_1_read_limit;                   ///< EMI region 1 read limit register
    uint16_t region_1_write_limit;                  ///< EMI region 1 write limit register

    uint16_t interrupt_set_register;                ///< interrupt set register
    uint16_t host_clear_enable_register;            ///< host clear enable register

} emi_config_space_t;

typedef struct emi {

    EMI_INSTANCE      instance;                     ///< emi instance identifier
    emi_config_space_t *config;                     ///< emi configuration space pointer

} emi_t;

typedef struct emi_region_config {

    uint32_t base;                                  ///< region base
    uint16_t read_limit;                            ///< region read limit
    uint16_t write_limit;                           ///< region write limit

} emi_region_config_t;

/*
 * emi_get - request a specific EMI instance and its config space
 * @emi:      a valid emi buffer that this function can populate
 * @instance: the desired EMI instance (0 or 1 in MEC152x ECs)
 * =>         returns EMI_SUCCESS on success and EMI_FAILURE on failure
 * */
extern int emi_get(emi_t *emi, const EMI_INSTANCE instance);

/*
 * emi_configure_region - configure a region within a specific EMI instance
 * @emi:      a valid emi instance, previously initialized by emi_get
 * @region:   the desired EMI region (0 or 1 in MEC152x ECs)
 * @rconf:    pointer to a region config structure
 * =>         returns EMI_SUCCESS on success and EMI_FAILURE on failure
 * */
extern int emi_configure_region(emi_t *emi, const EMI_REGION region, const emi_region_config_t *rconf);

/*
 * emi_reset_config - reset configuration of a specific EMI instance
 * @emi:      a valid emi instance, previously initialized by emi_get
 * */
extern void emi_reset_config(emi_t *emi);

#endif