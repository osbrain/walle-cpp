#ifndef _BSP_PS2_H
#define _BSP_PS2_H

#include "gd32f4xx.h"
#include "systick.h"

//¶Ë¿ÚÒÆÖ²
#define RCU_CLK  RCU_GPIOG
#define PORT_CLK  GPIOG
#define GPIO_CLK  GPIO_PIN_11

#define RCU_DAT  RCU_GPIOG
#define PORT_DAT  GPIOG
#define GPIO_DAT  GPIO_PIN_14


#define RCU_CMD   RCU_GPIOG
#define PORT_CMD  GPIOG
#define GPIO_CMD  GPIO_PIN_13

#define RCU_CS  RCU_GPIOG
#define PORT_CS  GPIOG
#define GPIO_CS  GPIO_PIN_10

#define CMD_OUT(x) 	gpio_bit_write(PORT_CMD,GPIO_CMD, (x?SET:RESET))
#define CLK_OUT(x) 	gpio_bit_write(PORT_CLK,GPIO_CLK, (x?SET:RESET))
#define DAT_GET()  	gpio_input_bit_get(PORT_DAT,GPIO_DAT)
#define CS_OUT(x) 	gpio_bit_write(PORT_CS,GPIO_CS, (x?SET:RESET))
/***********************************************************************************/

#define PSB_SELECT      1
#define PSB_L3          2
#define PSB_R3          3
#define PSB_START       4
#define PSB_PAD_UP      5
#define PSB_PAD_RIGHT   6
#define PSB_PAD_DOWN    7
#define PSB_PAD_LEFT    8
#define PSB_L2          9
#define PSB_R2          10
#define PSB_L1          11
#define PSB_R1          12
#define PSB_GREEN       13  //Èý½ÇÐÍ¼ü
#define PSB_RED         14	//Ô²È¦¼ü
#define PSB_BLUE        15	//X¼ü
#define PSB_PINK        16  //¾ØÐÎ¼ü

#define PSB_TRIANGLE    13
#define PSB_CIRCLE      14
#define PSB_CROSS       15
#define PSB_SQUARE      26

#define PSS_RX 5                //ÓÒÒ¡¸ËXÖáÊý¾Ý
#define PSS_RY 6
#define PSS_LX 7
#define PSS_LY 8

void PS2_GPIO_Init(void);
uint8_t PS2_RedLight(void);
uint8_t PS2_DataKey(void);
uint8_t PS2_AnologData(uint8_t button);
void PS2_SetInit(void);


#endif

