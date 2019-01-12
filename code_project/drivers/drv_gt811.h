#ifndef __GT811_H_
#define __GT811_H_

#include <rtthread.h>
#include <rtdevice.h>
#include "drv_i2c.h"
#include "drv_pin.h"
#include "littlevgl2rtt.h" 

#define GT811_I2C_BUS_NAME  "i2c1"
#define GT811_ADDR          0x5D
#define GT811_RESET_PIN     54
#define GT811_INT_PIN       45
#define GT811_EN_PIN        76

#define GT811_MAX_WIDTH     ((rt_uint16_t)1024)  /* Touchscreen pad max width */
#define GT811_MAX_HEIGHT    ((rt_uint16_t)600)   /* Touchscreen pad max height */

#define GT811_EXCHG_XY      1                    /* Transform XY coordinates */

#define GT811_MAX_TOUCH     1                    /* Maximum number of points supported by capacitive touch screen */
#define GT811_READ_XY_REG   0x721                /* Read the coordinate start register */
#define GT811_CONFIG_REG    0x6A2                /* Configure the parameter start register */

void GT811_Scan(void);                           /* scanning gt811 */

#endif /*__GT811_H_*/
