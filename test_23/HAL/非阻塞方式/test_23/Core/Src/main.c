/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "dma.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADDR_24LCxx_Write 0xA0		//24C02д��ַ
#define ADDR_24LCxx_Read 0xA1		//24C02����ַ
#define BufferSize 0x100			//�ܴ洢�ռ� 256�ֽ�
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t WriteBuffer[BufferSize], ReadBuffer[BufferSize];
uint16_t i,j;
uint16_t recv;		//����IIC��д�����ķ���ֵ������Debug

//�жϷ�ʽʵ��I2C
uint16_t check_TX;
uint16_t check_RX;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
//�жϷ�ʽʵ��I2C
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)  
//���HAL��ʹ��ʱ,ĳЩ������ܱ����bug
int _ttywrch(int ch)    
{
    ch=ch;
	return ch;
}
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->ISR&0X40)==0);//ѭ������,ֱ���������   
	USART1->TDR = (uint8_t) ch;      
	return ch;
}
#endif
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t key;		//����ֵ
	uint16_t i=0;		//LEDѭ��
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_LTDC_Init();
  MX_DMA_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	MX_FMC_Init();
	SDRAM_Initialization_Sequence(&hsdram1);//����SDRAM��ʼ������
	MX_LTDC_Init();
	LCD_Init();
	printf("��ʼ�����");
	
	POINT_COLOR=RED;
	LCD_ShowString(30,50,200,16,16,(uint8_t *)"Apollo STM32F4/F7"); 
	LCD_ShowString(30,70,200,16,16,(uint8_t *)"IIC TEST");	
	LCD_ShowString(30,90,200,16,16,(uint8_t *)"ATOM@ALIENTEK");
	LCD_ShowString(30,110,200,16,16,(uint8_t *)"2022/8/11");	 
	LCD_ShowString(30,130,200,16,16,(uint8_t *)"KEY1:Write  KEY0:Read");	//��ʾ��ʾ��Ϣ
	
	POINT_COLOR=BLUE;//��������Ϊ��ɫ

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	key=KEY_Scan(0);
	if(key==KEY1_PRES)	//KEY1���£�д��24C02
	{
		//д
		printf("Write data in EEPROM\r\n");
		LCD_Fill(0,170,239,319,WHITE);//�������
		LCD_ShowString(30,170,200,16,16,(uint8_t *)"Start Write 24C02....");
		for(i=0;i<256;i++)
		{
			WriteBuffer[i]=i;
		}
		for(j=0;j<32;j++)
		{
			//������ʽ
			//if((recv = HAL_I2C_Mem_Write(&hi2c2, ADDR_24LCxx_Write, 8 * j, I2C_MEMADD_SIZE_8BIT, WriteBuffer + 8 * j, 8,1000)) == HAL_OK)
			//DMA��ʽ��CubeMX���DMA I2C2_RX/TX��һ��Ҫ��ѡI2C2 event interrupt
			if((recv = HAL_I2C_Mem_Write_DMA(&hi2c2, ADDR_24LCxx_Write, 8 * j, I2C_MEMADD_SIZE_8BIT, WriteBuffer + 8 * j, 8)) == HAL_OK)
			{
				printf("\r\n EEPROM 24C02 Write Test OK! \r\n");
				LCD_ShowString(30,170,200,16,16,(uint8_t *)"24C02 Write Finished!");//��ʾ�������
			}
			else
			{
				HAL_Delay(5);
				printf("\r\n EEPROM 24C02 Write Test False! \r\n");
				printf("\r\n recv = %d \r\n", recv);
				LCD_ShowString(30,170,200,16,16,(uint8_t *)"24C02 Write False!   ");//��ʾ����ʧ��
			}
		}
	}
	if(key==KEY0_PRES)//KEY0����,��ȡ�ַ�������ʾ
	{
		//��
		LCD_ShowString(30,170,200,16,16,(uint8_t *)"Start Read 24C02.... ");
		//������ʽ
		//if((recv = HAL_I2C_Mem_Read(&hi2c2, ADDR_24LCxx_Read, 0, I2C_MEMADD_SIZE_8BIT, ReadBuffer, BufferSize, 1000))==HAL_OK)
		//DMA��ʽ
		if((recv = HAL_I2C_Mem_Read_DMA(&hi2c2, ADDR_24LCxx_Read, 0, I2C_MEMADD_SIZE_8BIT, ReadBuffer, BufferSize))==HAL_OK)
		{
		  printf("This is data in EEPROM!\r\n");
		  for(i=0;i<256;i++)
		  {
			printf("%d",ReadBuffer[i]);
		  }
		  printf("\r\n GOT Finished!! \r\n");
		  //�ж�ʵ��I2C
		  printf("\r\n check_TX = %d \r\n",check_TX);
		  printf("\r\n check_RX = %d \r\n",check_RX);
		  LCD_ShowString(30,170,200,16,16,(uint8_t *)"The Data Readed Is:  ");//��ʾ�������
		  printf("%d",ReadBuffer[BufferSize-1]);
		  LCD_ShowxNum(30,190,ReadBuffer[BufferSize-1],3,16,0x80);//��ʾ���������һ�����֣���Ϊ255���ȡ�ɹ�
		}
		else
		{
		  printf("Failed to get data!\r\n");
		  printf("recv = %d \r\n",recv);
		}
	}

	i++;
	HAL_Delay(10);
	if(i==20)
	{
		HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);  //��ʾϵͳ��������
		i=0;
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//�жϷ�ʽʵ��IIC
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	check_TX++;
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	check_RX++;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
