#ifndef __GT811_H_
#define __GT811_H_

#include <rtthread.h>
#include <rtdevice.h>

#define GT811_I2C_BUS_NAME          "i2c1"
#define GT811_ADDR                  0x5D
#define GT811_RESET_PIN             54
#define GT811_INT_PIN               45
#define GT811_EN_PIN                76

#define GT811_MAX_WIDTH        ((rt_uint16_t)1024)     //Touchscreen pad max width
#define GT811_MAX_HEIGHT       ((rt_uint16_t)600)      //Touchscreen pad max height

#define GT811_EXCHG_XY          1                                           //��תXY����

#define GT811_MAX_TOUCH     5                                           //���ݴ��������֧�ֵĵ���
#define GT811_READ_XY_REG   0x721                               //��ȡ����Ĵ��� 
#define GT811_CONFIG_REG    0x6A2                                   //���ò�����ʼ�Ĵ���

#define GT811_DEBUG                 1

typedef struct
{
    volatile rt_uint8_t Flag;
    volatile rt_uint8_t          Count;
    volatile rt_uint8_t          Static;
    volatile rt_uint8_t          TouchPointFlag;
    volatile rt_uint16_t         x[GT811_MAX_TOUCH];                                     //����X����
    volatile rt_uint16_t         y[GT811_MAX_TOUCH];                                     //����Y����
    volatile rt_uint8_t          ppr[GT811_MAX_TOUCH];                                   //�������ѹ��
} GT811_Device;

extern GT811_Device     GT811_Dev;

rt_uint8_t GT811_Init(void);                                                //��ʼ��GT811
void GT811_Scan(void);                                                      //ɨ��GT811

#endif /*__GT811_H_*/
