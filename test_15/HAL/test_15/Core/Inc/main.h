/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
//LCD LTDC��Ҫ������
typedef struct  
{							 
	uint32_t pwidth;			//LCD���Ŀ��,�̶�����,������ʾ����ı�,���Ϊ0,˵��û���κ�RGB������
	uint32_t pheight;		//LCD���ĸ߶�,�̶�����,������ʾ����ı�
	uint16_t hsw;			//ˮƽͬ�����
	uint16_t vsw;			//��ֱͬ�����
	uint16_t hbp;			//ˮƽ����
	uint16_t vbp;			//��ֱ����
	uint16_t hfp;			//ˮƽǰ��
	uint16_t vfp;			//��ֱǰ�� 
	uint8_t activelayer;		//��ǰ����:0/1	
	uint8_t dir;				//0,����;1,����;
	uint16_t width;			//LCD���
	uint16_t height;			//LCD�߶�
	uint32_t pixsize;		//ÿ��������ռ�ֽ���
}_ltdc_dev; 

typedef __IO uint32_t  vuint32_t;
typedef __IO uint16_t vuint16_t;
typedef __IO uint8_t  vuint8_t;

typedef __I uint32_t vuc32;  
typedef __I uint16_t vuc16; 
typedef __I uint8_t vuc8; 
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern uint32_t *ltdc_framebuf[2];					//LTDC LCD֡��������ָ��,����ָ���Ӧ��С���ڴ�����
extern _ltdc_dev lcdltdc;						//����LCD LTDC����Ҫ����
extern LTDC_HandleTypeDef  LTDC_Handler;	    //LTDC���
extern SDRAM_HandleTypeDef hsdram1;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
#define KEY2_Pin GPIO_PIN_13
#define KEY2_GPIO_Port GPIOC
#define KEY_WkUP_Pin GPIO_PIN_0
#define KEY_WkUP_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOH
#define KEY0_Pin GPIO_PIN_3
#define KEY0_GPIO_Port GPIOH
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOB
#define LED0_Pin GPIO_PIN_1
#define LED0_GPIO_Port GPIOB

//������ɫ���ظ�ʽ,һ����RGB565
#define LCD_PIXFORMAT				0X02	
//����Ĭ�ϱ�������ɫ
#define LTDC_BACKLAYERCOLOR			0X00000000	
//LCD֡�������׵�ַ,���ﶨ����SDRAM����.
#define LCD_FRAME_BUF_ADDR			0XC0000000  

#define Bank5_SDRAM_ADDR    ((uint32_t)(0XC0000000)) //SDRAM��ʼ��ַ

//SDRAM���ò���
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
