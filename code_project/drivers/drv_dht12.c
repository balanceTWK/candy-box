#include "drv_dht12.h"

//#define DRV_DEBUG
#define DBG_ENABLE
#define DBG_SECTION_NAME "drv.gt811"
#ifdef DRV_DEBUG
#define DBG_LEVEL         DBG_LOG
#else
#define DBG_LEVEL         DBG_INFO
#endif /* DRV_DEBUG */
#include <rtdbg.h>

static struct rt_i2c_bus_device *dht12_i2c_device = RT_NULL;

static rt_err_t read_regs(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint8_t *data_buf, uint8_t len)
{
    rt_uint8_t reg_buf[2];
    struct rt_i2c_msg msgs[2];

    msgs[0].addr  = DHT12_ADDR;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = &reg;
    msgs[0].len   = 1;

    msgs[1].addr = DHT12_ADDR;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf = data_buf;
    msgs[1].len = len;

    if (rt_i2c_transfer(bus, msgs, 2) == 2)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

int dht12_sensor_init(void)
{
    dht12_i2c_device = (struct rt_i2c_bus_device *)rt_device_find(DHT12_I2C_BUS_NAME);

    if (dht12_i2c_device == RT_NULL)
    {
        LOG_E("can't find %s device!", DHT12_I2C_BUS_NAME);
        return -RT_ERROR;
    }
    else
    {
        return RT_EOK;
    }
}
INIT_APP_EXPORT(dht12_sensor_init);

rt_err_t dht_read(float* temprature ,float* humi)
{
    rt_uint8_t buf[5],crc;
    if (read_regs(dht12_i2c_device, 0x00, buf, 5) == RT_EOK)
    {
        crc = (rt_uint8_t)(buf[0]+buf[1]+buf[2]+buf[3]);
        if(crc == buf[4])
        {
            *humi=buf[0]*10+buf[1];

            if (buf[3] & 0X80)
            {
                *temprature = 0 - (buf[2] * 10 + ((buf[3] & 0x7F)));
            }
            else
            {
                *temprature = buf[2] * 10 + buf[3];
            }
            LOG_D("temprature: %d humi: %d",(int)(temprature/10),(int)(humi/10));
        }
        else
        {
            LOG_E("DHT12 crc err");
            return -RT_ERROR;
        }
    }
    return RT_EOK;
}

#if defined(DRV_DEBUG)
void dht_test(void)
{
    float temprature, humi;
    if(dht_read(&temprature, &humi) == RT_EOK)
    {
        LOG_I("temprature: %d humi: %d",(int)(temprature/10),(int)(humi/10));
    }
    else
    {
        LOG_E("dht test failed");
    }
}
MSH_CMD_EXPORT(dht_test, dht_test);
#endif