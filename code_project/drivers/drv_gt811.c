#include "drv_i2c.h"
#include "drv_gt811.h"
#include "drv_pin.h"
#include "littlevgl2rtt.h" 
//#define DRV_DEBUG
#define DBG_ENABLE
#define DBG_SECTION_NAME    "drv.rtc"
#ifdef DRV_DEBUG
#define DBG_LEVEL                      DBG_LOG
#else
#define DBG_LEVEL                      DBG_INFO
#endif /* DRV_DEBUG */
#include <rtdbg.h>

static struct rt_i2c_bus_device *gt811_i2c_device = RT_NULL;
static volatile rt_uint8_t GTP_CFG_DATA[] =
{
    0x12,0x10,0x0E,0x0C,0x0A,0x08,0x06,0x04,0x02,0x00,0x05,0x55,0x15,0x55,0x25,0x55,
    0x35,0x55,0x45,0x55,0x55,0x55,0x65,0x55,0x75,0x55,0x85,0x55,0x95,0x55,0xA5,0x55,
    0xB5,0x55,0xC5,0x55,0xD5,0x55,0xE5,0x55,0xF5,0x55,0x1B,0x03,0x00,0x00,0x00,0x13,
    0x13,0x13,0x0F,0x0F,0x0A,0x50,0x30,0x05,0x03,0x64,0x05,0xe0,0x01,0x20,0x03,0x00,
    0x00,0x32,0x2C,0x34,0x2E,0x00,0x00,0x04,0x14,0x22,0x04,0x00,0x00,0x00,0x00,0x00,
    0x20,0x14,0xEC,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x30,
    0x25,0x28,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x01
};

GT811_Device GT811_Dev;

static void GT811_Delay(uint32_t value)
{
    rt_thread_delay(1);
}

static rt_err_t write_regs(struct rt_i2c_bus_device *bus, rt_uint16_t reg, rt_uint8_t *data_buf, uint8_t len)
{
    rt_uint8_t buf[len + 2];
    struct rt_i2c_msg msgs;

    buf[0] = (rt_uint8_t)(reg >> 8);
    buf[1] = (rt_uint8_t)(reg & 0XFF);

    for (rt_uint32_t i = 0; i < len; i++)
    {
        buf[i+2] = data_buf[i];
    }

    msgs.addr = GT811_ADDR;
    msgs.flags = RT_I2C_WR;
    msgs.buf = buf;
    msgs.len = len + 2;

    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

static rt_err_t read_regs(struct rt_i2c_bus_device *bus, rt_uint16_t reg, rt_uint8_t *data_buf, uint8_t len)
{
    rt_uint8_t reg_buf[2];
    struct rt_i2c_msg msgs[2];

    reg_buf[0] = (rt_uint8_t)(reg >> 8); //cmd
    reg_buf[1] = (rt_uint8_t)(reg & 0XFF); //cmd

    msgs[0].addr  = GT811_ADDR;       /* Slave address */
    msgs[0].flags = RT_I2C_WR;        /* Write flag */
    msgs[0].buf   = reg_buf;          /* Slave register address */
    msgs[0].len   = 2;                /* Number of bytes sent */

    msgs[1].addr = GT811_ADDR;
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

static uint8_t GT811_Send_Cfg(uint8_t *buf, uint16_t cfg_len)
{
    rt_err_t ret = 0;
    uint8_t retry = 0;
    for (retry = 0; retry < 5; retry++)
    {
        ret = write_regs(gt811_i2c_device, GT811_CONFIG_REG, buf, cfg_len);
        if (ret == RT_EOK)break;
        rt_thread_mdelay(100);
    }
    return ret;
}
#ifdef DRV_DEBUG
static void GT811_Read_Cfg()
{

    uint8_t CFG_DATA[sizeof(GTP_CFG_DATA)];
    uint16_t tmp;
    uint8_t tmp2;
    const uint16_t CFG_SIZE = sizeof(GTP_CFG_DATA);

    read_regs(gt811_i2c_device, GT811_CONFIG_REG, CFG_DATA, CFG_SIZE);

    for (tmp = 0; tmp < CFG_SIZE - 1; tmp++)
    {
        read_regs(gt811_i2c_device, GT811_CONFIG_REG + tmp, &tmp2, 1);          //Read version of eight
        LOG_I("REG:0x%x:0x%02x -- 0x%02x", GT811_CONFIG_REG + tmp, tmp2, GTP_CFG_DATA[tmp]);
        if (tmp2 !=  GTP_CFG_DATA[tmp])
        {
            LOG_E("***error***");
        }
    }
}
#endif
static void GT811_Reset()
{
    rt_pin_mode(GT811_RESET_PIN, PIN_MODE_OUTPUT);
//    rt_pin_mode(GT811_EN_PIN, PIN_MODE_OUTPUT);
//    rt_pin_write(GT811_EN_PIN, PIN_LOW);
    rt_pin_write(GT811_RESET_PIN, PIN_HIGH);
    rt_thread_mdelay(200);
    rt_pin_write(GT811_RESET_PIN, PIN_LOW);
    rt_thread_mdelay(200);
    rt_pin_write(GT811_RESET_PIN, PIN_HIGH);
    rt_thread_mdelay(200);
}

struct rt_semaphore touch_sem;
void touch_thread_entry(void* parameter)
{
    rt_uint8_t buf[34];
    while(1)
    {
        rt_sem_take(&touch_sem, RT_WAITING_FOREVER);
        read_regs(gt811_i2c_device, GT811_READ_XY_REG, buf, 34);
        __DSB();
        LOG_D("rt_sem_take touch_sem");
#if (GT811_EXCHG_XY == 1)
        GT811_Dev.y[0] = GT811_MAX_HEIGHT - (((uint16_t)buf[2] << 8) + buf[3]);
        GT811_Dev.x[0] = ((uint16_t)buf[4] << 8) + buf[5];
        GT811_Dev.ppr[0] = buf[6];
        GT811_Dev.y[1] = GT811_MAX_HEIGHT - (((uint16_t)buf[7] << 8) + buf[8]);
        GT811_Dev.x[1] = ((uint16_t)buf[9] << 8) + buf[10];
        GT811_Dev.ppr[1] = buf[11];
        GT811_Dev.y[2] = GT811_MAX_HEIGHT - (((uint16_t)buf[12] << 8) + buf[13]);
        GT811_Dev.x[2] = ((uint16_t)buf[14] << 8) + buf[15];
        GT811_Dev.ppr[2] = buf[16];
        GT811_Dev.y[3] = GT811_MAX_HEIGHT - (((uint16_t)buf[17] << 8) + buf[24]);
        GT811_Dev.x[3] = ((uint16_t)buf[25] << 8) + buf[26];
        GT811_Dev.ppr[3] = buf[27];
        GT811_Dev.y[4] = GT811_MAX_HEIGHT - (((uint16_t)buf[28] << 8) + buf[29]);
        GT811_Dev.x[4] = ((uint16_t)buf[30] << 8) + buf[31];
        GT811_Dev.ppr[4] = buf[32];
#else
        GT811_Dev.y[0] = ((uint16_t)buf[2] << 8) + buf[3];
        GT811_Dev.x[0] = GT811_MAX_WIDTH - (((uint16_t)buf[4] << 8) + buf[5]);
        GT811_Dev.ppr[0] = buf[6];
        GT811_Dev.y[1] = ((uint16_t)buf[7] << 8) + buf[8];
        GT811_Dev.x[1] = GT811_MAX_WIDTH - (((uint16_t)buf[9] << 8) + buf[10]);
        GT811_Dev.ppr[1] = buf[11];
        GT811_Dev.y[2] = ((uint16_t)buf[12] << 8) + buf[13];
        GT811_Dev.x[2] = GT811_MAX_WIDTH - (((uint16_t)buf[14] << 8) + buf[15]);
        GT811_Dev.ppr[2] = buf[16];
        GT811_Dev.y[3] = ((uint16_t)buf[17] << 8) + buf[24];
        GT811_Dev.x[3] = GT811_MAX_WIDTH - (((uint16_t)buf[25] << 8) + buf[26]);
        GT811_Dev.ppr[3] = buf[27];
        GT811_Dev.y[4] = ((uint16_t)buf[28] << 8) + buf[29];
        GT811_Dev.x[4] = GT811_MAX_WIDTH - (((uint16_t)buf[30] << 8) + buf[31]);
        GT811_Dev.ppr[4] = buf[32];
#endif
//rt_kprintf("GT811_Dev.y[0]:%d GT811_Dev.x[0]:%d GT811_Dev.ppr[0]:%d\n",GT811_Dev.y[0],GT811_Dev.x[0],GT811_Dev.ppr[0]);
        if((GT811_Dev.ppr[0])==0)
        {
            littlevgl2rtt_send_input_event(GT811_Dev.x[0], GT811_Dev.y[0], LITTLEVGL2RTT_INPUT_UP);
        }
        else if((GT811_Dev.ppr[0])>=3)
        {
            littlevgl2rtt_send_input_event(GT811_Dev.x[0], GT811_Dev.y[0], LITTLEVGL2RTT_INPUT_DOWN);
        }
        else if((GT811_Dev.ppr[0])>0)
        {
            littlevgl2rtt_send_input_event(GT811_Dev.x[0], GT811_Dev.y[0], LITTLEVGL2RTT_INPUT_MOVE);
        }
        }
}
void touch_irq(void *args)
{
    LOG_D("touch_irq!");
    rt_sem_release(&touch_sem);
}
uint8_t GT811_Init(void)
{
    uint16_t version = 0;
    uint8_t temp;

    gt811_i2c_device = (struct rt_i2c_bus_device *)rt_device_find(GT811_I2C_BUS_NAME);

    if (gt811_i2c_device == RT_NULL)
    {
        LOG_E("can't find %s device!", GT811_I2C_BUS_NAME);
    }
    else
    {
        rt_pin_mode(GT811_INT_PIN, PIN_MODE_INPUT_PULLUP);
        rt_pin_attach_irq(GT811_INT_PIN,PIN_IRQ_MODE_FALLING,touch_irq,RT_NULL);
        GT811_Reset();
        rt_sem_init(&touch_sem , "lcd_touch",   0,    RT_IPC_FLAG_FIFO);
        rt_pin_irq_enable(GT811_INT_PIN, PIN_IRQ_ENABLE);
        GTP_CFG_DATA[62] = GT811_MAX_WIDTH >> 8;
        GTP_CFG_DATA[61] = GT811_MAX_WIDTH & 0xff;
        GTP_CFG_DATA[60] = GT811_MAX_HEIGHT >> 8;
        GTP_CFG_DATA[59] = GT811_MAX_HEIGHT & 0xff;

        read_regs(gt811_i2c_device, 0X717, &temp, 1);
        version = (uint16_t)temp << 8;
        read_regs(gt811_i2c_device, 0X718, &temp, 1);
        version |= temp;
        LOG_I("version:%x", version);

        if (version == 0X2010)
        {
            temp = GT811_Send_Cfg((uint8_t *)GTP_CFG_DATA, sizeof(GTP_CFG_DATA));
        }
        else temp = 2;
#ifdef DRV_DEBUG
        GT811_Read_Cfg();
#endif
        rt_thread_t thread = RT_NULL; 
        thread = rt_thread_create("lcd_touch", touch_thread_entry, RT_NULL, 512, 7, 10); 
        rt_thread_startup(thread);
    }

    return temp;
}
INIT_APP_EXPORT(GT811_Init);

void GT811_Scan(void)
{
    uint8_t buf[34];
    uint8_t tmp, tmp2;
    uint16_t sum = 0;
    uint8_t count = 0;
    if (GT811_Dev.Flag == 0)return;
    GT811_Dev.Flag = 0;

    while (1)
    {
        count++;
        if (count == 5)
        {
            return;
        }
        read_regs(gt811_i2c_device, GT811_READ_XY_REG, buf, 34);
        __DSB();

        GT811_Dev.TouchPointFlag = tmp2 = buf[0] & 0X1F;
        sum = 0;
        for (tmp = 0; tmp < 5; tmp++)
        {
            if (tmp2 & 0x01)
            {
                sum++;
            }
            tmp2 = tmp2 >> 1;
        }
        GT811_Dev.Count = sum;
        sum = 0;

        for (tmp = 0; tmp < 33; tmp++)
        {
            sum += buf[tmp];
        }

        if (sum == 0)
        {
            GT811_Delay(5);
            continue;
        }
        __DSB();
        if (((sum & 0xff) == buf[33]) || (GT811_Dev.Count == 1) || (GT811_Dev.Count == 2))
        {
            GT811_Dev.Static = 1;
            break;
        }
        else
        {
            GT811_Dev.Static = 0;
            GT811_Delay(5);
            continue;
        }
    }

#if (GT811_EXCHG_XY == 1)
    GT811_Dev.y[0] = GT811_MAX_HEIGHT - (((uint16_t)buf[2] << 8) + buf[3]);
    GT811_Dev.x[0] = ((uint16_t)buf[4] << 8) + buf[5];
    GT811_Dev.ppr[0] = buf[6];
    GT811_Dev.y[1] = GT811_MAX_HEIGHT - (((uint16_t)buf[7] << 8) + buf[8]);
    GT811_Dev.x[1] = ((uint16_t)buf[9] << 8) + buf[10];
    GT811_Dev.ppr[1] = buf[11];
    GT811_Dev.y[2] = GT811_MAX_HEIGHT - (((uint16_t)buf[12] << 8) + buf[13]);
    GT811_Dev.x[2] = ((uint16_t)buf[14] << 8) + buf[15];
    GT811_Dev.ppr[2] = buf[16];
    GT811_Dev.y[3] = GT811_MAX_HEIGHT - (((uint16_t)buf[17] << 8) + buf[24]);
    GT811_Dev.x[3] = ((uint16_t)buf[25] << 8) + buf[26];
    GT811_Dev.ppr[3] = buf[27];
    GT811_Dev.y[4] = GT811_MAX_HEIGHT - (((uint16_t)buf[28] << 8) + buf[29]);
    GT811_Dev.x[4] = ((uint16_t)buf[30] << 8) + buf[31];
    GT811_Dev.ppr[4] = buf[32];
#else
    GT811_Dev.y[0] = ((uint16_t)buf[2] << 8) + buf[3];
    GT811_Dev.x[0] = GT811_MAX_WIDTH - (((uint16_t)buf[4] << 8) + buf[5]);
    GT811_Dev.ppr[0] = buf[6];
    GT811_Dev.y[1] = ((uint16_t)buf[7] << 8) + buf[8];
    GT811_Dev.x[1] = GT811_MAX_WIDTH - (((uint16_t)buf[9] << 8) + buf[10]);
    GT811_Dev.ppr[1] = buf[11];
    GT811_Dev.y[2] = ((uint16_t)buf[12] << 8) + buf[13];
    GT811_Dev.x[2] = GT811_MAX_WIDTH - (((uint16_t)buf[14] << 8) + buf[15]);
    GT811_Dev.ppr[2] = buf[16];
    GT811_Dev.y[3] = ((uint16_t)buf[17] << 8) + buf[24];
    GT811_Dev.x[3] = GT811_MAX_WIDTH - (((uint16_t)buf[25] << 8) + buf[26]);
    GT811_Dev.ppr[3] = buf[27];
    GT811_Dev.y[4] = ((uint16_t)buf[28] << 8) + buf[29];
    GT811_Dev.x[4] = GT811_MAX_WIDTH - (((uint16_t)buf[30] << 8) + buf[31]);
    GT811_Dev.ppr[4] = buf[32];
#endif
}
