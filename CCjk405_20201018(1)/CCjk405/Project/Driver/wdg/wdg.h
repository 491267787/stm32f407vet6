#ifndef _WDG_H
#define _WDG_H

#ifdef  __cplusplus
extern  "C" {
#endif

#include "config.h"
#if defined (STM32F10X_CL)
#include "stm32f10x_wwdg.h"
#include "stm32f10x_iwdg.h"
#endif

#if defined (STM32F4XX)
#include "stm32f4xx_wwdg.h"
#include "stm32f4xx_iwdg.h"
#endif

#define CFG_IWDG_EN		              1          // ���ö������Ź�ʹ�� 
#define CFG_WWDG_EN		              0          // ���ô��ڿ��Ź�ʹ�� 

#if CFG_IWDG_EN
#define WATCH_DOG_ENABLE			  1
#endif

#define OUTWATCHDOG_GPIO_PORT              GPIOG 
#define OUTWATCHDOG_GPIO_CLK               RCC_AHB1Periph_GPIOG  
#define OUTWATCHDOG_GPIO_PIN               GPIO_Pin_0


#define IWDG_TIMEOUT                3000       // ms ��Χ1000-3276

extern void sysIWdgInit(void);
extern void sysWWdgInit(void);
extern void FeedWWdog(void);
extern void Wdg_feeddog(void);

#ifdef  __cplusplus
}
#endif

#endif

