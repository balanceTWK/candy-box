#ifndef __DHT12_H_
#define __DHT12_H_

#include <rtthread.h>
#include <rtdevice.h>
#include "drv_i2c.h"

#define DHT12_I2C_BUS_NAME  "i2c3"
#define DHT12_ADDR          0x5C

rt_err_t dht_read(float* temprature ,float* humi);

#endif /*__DHT12_H_*/
