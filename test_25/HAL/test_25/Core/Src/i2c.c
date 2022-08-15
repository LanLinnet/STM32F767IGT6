/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c2;

/* I2C2 init function */
void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x60201E2B;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
    PeriphClkInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_RCC_GPIOH_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PH4     ------> I2C2_SCL
    PH5     ------> I2C2_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    /* I2C2 clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();
  /* USER CODE BEGIN I2C2_MspInit 1 */

  /* USER CODE END I2C2_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspDeInit 0 */

  /* USER CODE END I2C2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C2_CLK_DISABLE();

    /**I2C2 GPIO Configuration
    PH4     ------> I2C2_SCL
    PH5     ------> I2C2_SDA
    */
    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_4);

    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_5);

  /* USER CODE BEGIN I2C2_MspDeInit 1 */

  /* USER CODE END I2C2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//��ʼ��AP3216C
uint8_t AP3216C_Init(void)
{
	uint8_t temp=0;
    AP3216C_WriteOneByte(0x00,0X04);	//��λAP3216C
    HAL_Delay(50);						//AP33216C��λ����10ms
    AP3216C_WriteOneByte(0x00,0X03);	//����ALS��PS+IR   
	temp=AP3216C_ReadOneByte(0X00);		//��ȡ�ո�д��ȥ��0X03
	if(temp==0X03)return 0;				//AP3216C����
	else return 1;						//AP3216Cʧ��
} 

//��ȡAP3216C������
//��ȡԭʼ���ݣ�����ALS,PS��IR
//ע�⣡���ͬʱ��ALS,IR+PS�Ļ��������ݶ�ȡ��ʱ����Ҫ����112.5ms
void AP3216C_ReadData(uint16_t* ir,uint16_t* ps,uint16_t* als)
{
    uint8_t buf[6];
    uint8_t i;
    for(i=0;i<6;i++)	
    {
        buf[i]=AP3216C_ReadOneByte(0X0A+i);		//ѭ����ȡ���д���������
    }
    if(buf[0]&0X80)*ir=0;						//IR_OFλΪ1,��������Ч
	else *ir=((uint16_t)buf[1]<<2)|(buf[0]&0X03); 	//��ȡIR������������  
	*als=((uint16_t)buf[3]<<8)|buf[2];				//��ȡALS������������   
    if(buf[4]&0x40)*ps=0;    					//IR_OFλΪ1,��������Ч
	else *ps=((uint16_t)(buf[5]&0X3F)<<4)|(buf[4]&0X0F); //��ȡPS������������   
}

//IICдһ���ֽ�  
//reg:�Ĵ�����ַ
//data:Ҫд�������
//����ֵ:0,����
//    ����,�������
uint8_t AP3216C_WriteOneByte(uint8_t reg,uint8_t data)
{
    return HAL_I2C_Mem_Write(&hi2c2, AP3216C_ADDR|0x00, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xff);
}

//IIC��һ���ֽ� 
//reg:�Ĵ�����ַ 
//����ֵ:����������
uint8_t AP3216C_ReadOneByte(uint8_t reg)
{
    uint8_t res;
	HAL_I2C_Mem_Read(&hi2c2, AP3216C_ADDR|0x01, reg, I2C_MEMADD_SIZE_8BIT, &res, 1, 0xff);
    return res;  
}

/* USER CODE END 1 */
