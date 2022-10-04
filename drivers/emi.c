/* Embedded Memory Interface (EMI) Driver */

#include <zephyr.h>
#include "emi.h"

#define EC_EMI_0_BASE 0x400f4000
#define EC_EMI_1_BASE 0x400f4400

/* <--- Forward Declaration of internal Functionality ---> */

static emi_config_space_t* emi_config_space(const EMI_INSTANCE instance);

/* <--- Exposed Functionality ---> */

int emi_get(emi_t *emi, const EMI_INSTANCE instance) {

    if((emi->config = emi_config_space(instance)) == NULL)
        return EMI_FAILURE;

    emi->instance = instance;
    return EMI_SUCCESS;
}

int emi_configure_region(emi_t *emi, const EMI_REGION region, const emi_region_config_t *rconf) {

    if(region >= EMI_REGION_END)
        return EMI_FAILURE;

    switch(region) {

        case EMI_REGION_0:

            emi->config->region_0_base        |= (rconf->base & 0xfffffffc);
            emi->config->region_0_read_limit  |= (rconf->read_limit & 0x7ffc);
            emi->config->region_0_write_limit |= (rconf->write_limit & 0x7ffc);

        break;

        case EMI_REGION_1:

            emi->config->region_1_base        |= (rconf->base & 0xfffffffc);
            emi->config->region_1_read_limit  |= (rconf->read_limit & 0x7ffc);
            emi->config->region_1_write_limit |= (rconf->write_limit & 0x7ffc);

        break;

        default: /* never reached */ break;
    }

    return EMI_SUCCESS;
}

void emi_reset_config(emi_t *emi) {

    emi->config->host_ec_mailbox_rt          = 0x0;
    emi->config->ec_host_mailbox_rt          = 0x0;
    emi->config->ec_address_lsb              = 0x0;
    emi->config->ec_address_msb              = 0x0;
    emi->config->ec_data_byte_0              = 0x0;
    emi->config->ec_data_byte_1              = 0x0;
    emi->config->ec_data_byte_2              = 0x0;
    emi->config->ec_data_byte_3              = 0x0;
    emi->config->interrupt_source_lsb       &= 0x1;
    emi->config->interrupt_source_msb        = 0x0;
    emi->config->interrupt_mask_lsb          = 0x0;
    emi->config->interrupt_mask_msb          = 0x0;
    emi->config->application_id              = 0x0;

    emi->config->host_ec_mailbox             = 0x0;
    emi->config->ec_host_mailbox             = 0x0;
    emi->config->region_0_base              &= 0x3;
    emi->config->region_0_read_limit        &= 0x8003;
    emi->config->region_0_write_limit       &= 0x8003;
    emi->config->region_1_base              &= 0x3;
    emi->config->region_1_read_limit        &= 0x8003;
    emi->config->region_1_write_limit       &= 0x8003;
    emi->config->interrupt_set_register     &= 0x1;
    emi->config->host_clear_enable_register &= 0x1;
}

/* <--- Internal Functionality ---> */

/*
 * emi_config_space - sets up an emi config space pointer
 * @instance: the desired EMI instance (0 or 1 in MEC152x ECs)
 * =>         returns a pointer to the config space and NULL on failure
 * */
emi_config_space_t* emi_config_space(const EMI_INSTANCE instance) {

    emi_config_space_t *config_space;

    switch(instance) {

        case EMI_INSTANCE_0: config_space = (emi_config_space_t*) EC_EMI_0_BASE; break;
        case EMI_INSTANCE_1: config_space = (emi_config_space_t*) EC_EMI_1_BASE; break;
        default:             config_space = NULL;                                break;
    }

    return config_space;
}