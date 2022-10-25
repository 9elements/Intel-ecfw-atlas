#
# Copyright (c) 2022 Microchip Technology Inc.
#
# SPDX-License-Identifier: Apache-2.0
#

zephyr_library()
zephyr_library_sources(pinmux.c)

if(CMAKE_HOST_UNIX)
    set(EVERGLADES_SPI_GEN ${CMAKE_SOURCE_DIR}/tools/everglades_spi_gen/everglades_spi_gen_RomE)
elseif(CMAKE_HOST_WIN32)
    set(EVERGLADES_SPI_GEN ${CMAKE_SOURCE_DIR}/tools/everglades_spi_gen/everglades_spi_gen_RomE.exe)
else()
    message(FATAL_ERROR "There is no SPI image generation tool for MacOS.")
endif()

set_property(GLOBAL APPEND PROPERTY extra_post_build_commands
  COMMAND ${EVERGLADES_SPI_GEN}
  -i ${BOARD_DIR}/support/spi_cfg.txt
  -o ${PROJECT_BINARY_DIR}/ec.bin)
