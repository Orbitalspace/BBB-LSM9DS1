#include <stdio.h>
#include <stdlib.h>
#include "i2c.h"

#include "SparkFunLSM9DS1.h"

#define LSM9DS1_M	0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG	0x6B // Would be 0x6A if SDO_AG is LOW

int main(){ 
	
    LSM9DS1 imu;

    imu.settings.device.commInterface = IMU_MODE_I2C;
    imu.settings.device.mAddress = LSM9DS1_M;
    imu.settings.device.agAddress = LSM9DS1_AG;

    printf("\n\nInitializing I2C interface file descriptor\n\n");

	KI2CStatus status;

    if (!imu.begin())
    {
        fprintf(stderr, "Failed to initialize I2C bus connection: %d\n", status);
        return -1;
    }
    printf("Successfuly Connected to i2c Bus #2\n\n"); 


    // // Initialize our file descriptor storage variable
    // int bus = 0;
    // // Open a connection to I2C bus 1
    // status = k_i2c_init("/dev/i2c-2", &bus);
    // if (status != I2C_OK)
    // {
    //     fprintf(stderr, "Failed to initialize I2C bus connection: %d\n", status);
    //     return -1;
    // }


}
