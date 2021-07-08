#include "paCoreInc/all"
#include "./src/u8g2.h"
static char u8_ssd1306_iic_id = 0;
static uint8_t STM32_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    // case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
    //     __NOP();
    //     break;
    // case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
    //     for (uint16_t n = 0; n < 320; n++)
    //     {
    //         __NOP();
    //     }
    //     break;
    // case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
    //     HAL_Delay(1);
    //     break;
    // case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
    //     u8_delay_us(5);
    //     break;                    // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
    // case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
    //     if (arg_int == 1)
    //     {
    //     } // arg_int=1: Input dir with pullup high for I2C clock pin
    //       // HAL_GPIO_WritePin(GPIOB, SCL_Pin, GPIO_PIN_SET);
    //     else if (arg_int == 0)
    //     {
    //     }
    //     // HAL_GPIO_WritePin(GPIOB, SCL_Pin, GPIO_PIN_RESET);
    //     break;
    // case U8X8_MSG_GPIO_I2C_DATA: // arg_int=0: Output low at I2C data pin
    //     if (arg_int == 1)
    //     {
    //     } // arg_int=1: Input dir with pullup high for I2C data pin
    //       // HAL_GPIO_WritePin(GPIOB, SDA_Pin, GPIO_PIN_SET);
    //     else if (arg_int == 0)
    //     {
    //     }
    //     // HAL_GPIO_WritePin(GPIOB, SDA_Pin, GPIO_PIN_RESET);
    //     break;
    case U8X8_MSG_GPIO_MENU_SELECT:
        u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_NEXT:
        u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_PREV:
        u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_HOME:
        u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
        break;
    default:
        u8x8_SetGPIOResult(u8x8, 1); // default return value
        break;
    }
    return 1;
}
static uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    /* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
    // struct rt_i2c_msg msgs;
    static uint8_t buffer[32];
    static uint8_t buf_idx;
    uint8_t *data;

    uint8_t t = 0;
    switch (msg)
    {
    case U8X8_MSG_BYTE_SEND:
        data = (uint8_t *)arg_ptr;
        while (arg_int > 0)
        {
            buffer[buf_idx++] = *data;
            data++;
            arg_int--;
        }
        break;

    case U8X8_MSG_BYTE_INIT:
        // i2c_bus = rt_i2c_bus_device_find(U8G2_I2C_DEVICE_NAME);
        // if (i2c_bus == RT_NULL)
        // {
        //     rt_kprintf("[u8g2] Failed to find bus %s\n", U8G2_I2C_DEVICE_NAME);
        //     return 0;
        // }
        break;

    case U8X8_MSG_BYTE_SET_DC:
        break;

    case U8X8_MSG_BYTE_START_TRANSFER:
        buf_idx = 0;
        break;

    case U8X8_MSG_BYTE_END_TRANSFER:
    {
        auto byteArr = ByteArr(buf_idx, buffer);
        _G_paIIC.writeLen(u8_ssd1306_iic_id, 0x3c, 1, &byteArr);
    }
    // if (i2c_bus == RT_NULL)
    // {
    //     rt_kprintf("[u8g2] Failed to find bus %s\n", U8G2_I2C_DEVICE_NAME);
    //     return 0;
    // }
    // // I2C Data Transfer
    // msgs.addr  = u8x8_GetI2CAddress(u8x8)>>1;
    // msgs.flags = RT_I2C_WR;
    // msgs.buf   = buffer;
    // msgs.len   = buf_idx;
    // while(rt_i2c_transfer(i2c_bus, &msgs, 1) != 1 && t < MAX_RETRY)
    // {
    //     t++;
    // };
    // if(t >= MAX_RETRY)
    // {
    //     return 0;
    // }
    break;

    default:
        return 0;
    }
    return 1;
}
/****
 * 
 *   make sure you have init i2c and
 * 
 * ******/
static void u8g2_i2c_ssd1306_init(u8g2_t *u8g2, char iicId)
{
    u8_ssd1306_iic_id = iicId;
    _G_paBase.delayMs(1);
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(u8g2, U8G2_R0, u8x8_byte_hw_i2c, STM32_gpio_and_delay);
}