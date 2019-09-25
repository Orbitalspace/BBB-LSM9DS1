/*
 * KubOS HAL
 * Copyright (C) 2016 Kubos Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "i2c.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

KI2CStatus k_i2c_init(char * device, int * fp)
{
    if (device == NULL || fp == NULL)
    {
        return I2C_ERROR;
    }

    char bus[] = "/dev/i2c-n\0";
    // Make sure the device name is null terminated
    snprintf(bus, 11, "%s", device);
    *fp = open(bus, O_RDWR);

    if (*fp <= 0)
    {
        perror("Couldn't open I2C bus");
        *fp = 0;
        return I2C_ERROR_CONFIG;
    }

    return I2C_OK;
}

void k_i2c_terminate(int * fp)
{
    if (fp == NULL || *fp == 0)
    {
        return;
    }

    close(*fp);
    *fp = 0;

    return;
}

KI2CStatus k_i2c_write(int i2c, uint16_t addr, uint8_t* ptr, int len)
{
    if (i2c == 0 || ptr == NULL)
    {
        return I2C_ERROR;
    }

    /* Set the desired slave's address */
    if (ioctl(i2c, I2C_SLAVE, addr) < 0)
    {
        perror("Couldn't reach requested address");
        return I2C_ERROR_ADDR_TIMEOUT;
    }

    /* Transmit buffer */
    if (write(i2c, ptr, len) != len)
    {
        perror("I2C write failed");
        return I2C_ERROR;
    }

    return I2C_OK;
}

KI2CStatus k_i2c_read(int i2c, uint16_t addr, uint8_t* ptr, int len)
{
    if (i2c == 0 || ptr == NULL)
    {
        return I2C_ERROR;
    }

    /* Set the desired slave's address */
    if (ioctl(i2c, I2C_SLAVE, addr) < 0)
    {
        perror("Couldn't reach requested address");
        return I2C_ERROR_ADDR_TIMEOUT;
    }

    /* Read in data */
    if (read(i2c, ptr, len) != len)
    {
        perror("I2C read failed");
        return I2C_ERROR;
    }

    return I2C_OK;
}

// Write to an I2C slave device's register:
KI2CStatus i2c_write(int i2c, uint16_t slave_addr, uint8_t reg, uint8_t data) {
    int retval;
    uint8_t outbuf[2];

    struct i2c_msg msgs[1];
    struct i2c_rdwr_ioctl_data msgset[1];

    outbuf[0] = reg;
    outbuf[1] = data;

    msgs[0].addr = slave_addr;
    msgs[0].flags = 0;
    msgs[0].len = 2;
    msgs[0].buf = outbuf;

    msgset[0].msgs = msgs;
    msgset[0].nmsgs = 1;

    if (ioctl(i2c, I2C_RDWR, &msgset) < 0) {
        perror("ioctl(I2C_RDWR) in i2c_write");
        return I2C_ERROR;
    }

    return 0;
}

// Read the given I2C slave device's register and return the read value in `*result`:
KI2CStatus i2c_read(int i2c, uint16_t slave_addr, uint8_t reg, uint8_t *result, uint8_t count) {
    int retval;
    uint8_t outbuf[1], inbuf[1];
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data msgset[1];

    msgs[0].addr = slave_addr;
    msgs[0].flags = 0;
    msgs[0].len = count;
    msgs[0].buf = outbuf;

    msgs[1].addr = slave_addr;
    msgs[1].flags = I2C_M_RD | I2C_M_NOSTART;
    msgs[1].len = 1;
    msgs[1].buf = inbuf;

    msgset[0].msgs = msgs;
    msgset[0].nmsgs = 2;

    outbuf[0] = reg;

    inbuf[0] = 0;

    *result = 0;
    if (ioctl(i2c, I2C_RDWR, &msgset) < 0) {
        perror("ioctl(I2C_RDWR) in i2c_read");
        return I2C_ERROR;
    }

    *result = inbuf[0];
    return 0;
}