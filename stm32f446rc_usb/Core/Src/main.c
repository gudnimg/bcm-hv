/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include "vicor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
//BCM4414 bcm; // Global BCM variable.
char msg[53];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
void print_data(char *msg, char *data);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_USB_DEVICE_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //uint8_t capability = read_capability(&hi2c1);
	  //read_status_specific(&hi2c1);
	  read_status_cml(&hi2c1);
	  /*read_status_iout(&hi2c1);
	  read_status_input(&hi2c1);*/

	  sprintf(msg, "%s      0x%x\n\r", "Status CML:", bcm.status_cml.all);
	  CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));

	  clear_faults(&hi2c1);

	  /*get_mfr_partnr(&hi2c1);
	  get_mfr_serialnr(&hi2c1);
	  get_mfr_date(&hi2c1);
	  get_mfr_location(&hi2c1);
	  get_mfr_revision(&hi2c1);
	  read_temperature(&hi2c1);
	  read_iout(&hi2c1);
	  read_iin(&hi2c1);
	  read_pout(&hi2c1)*/;

	  HAL_StatusTypeDef myop = read_operation(&hi2c1);

	  switch (myop)
	      {
	      case HAL_ERROR:
	          sprintf(msg, "HAL Error.\r\n");
	          CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));
	          break;
	      case HAL_BUSY:
	          sprintf(msg, "HAL Busy.\r\n");
	          CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));
	          break;
	      case HAL_TIMEOUT:
	          sprintf(msg, "HAL Timeout.\r\n");
	          CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));
	          break;

	      default:
	          // Keep default to ignore warning about HAL_OK case.
	          break;
	      }
	   write_page(&hi2c1, 0x01);
	   read_page(&hi2c1);


	  sprintf(msg, "%s      0x%x\n\r", "Operation:", bcm.operation);
	  CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));

	  write_operation(&hi2c1, 0x80);
	  //myop = read_operation(&hi2c1);

	  /*switch (myop)
	  	      {
	  	      case HAL_ERROR:
	  	          sprintf(msg, "HAL Error.\r\n");
	  	          CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));
	  	          break;
	  	      case HAL_BUSY:
	  	          sprintf(msg, "HAL Busy.\r\n");
	  	          CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));
	  	          break;
	  	      case HAL_TIMEOUT:
	  	          sprintf(msg, "HAL Timeout.\r\n");
	  	          CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));
	  	          break;

	  	      default:
	  	          // Keep default to ignore warning about HAL_OK case.
	  	          break;
	  	      }*/


	  sprintf(msg, "%s      0x%x\n\r", "Operation:", bcm.operation);
	  CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));

	  write_page(&hi2c1, 0x00);

	  sprintf(msg, "%s      0x%x\n\r", "Operation:", bcm.operation);
	  CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));

	  HAL_Delay(1000);

	  /*if (bcm.status_specific.bcm_uart_cml) {
		  sprintf(msg, "CML: BCM UART CML\r\n");
		  CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));
	  }*/

	  /*sprintf(msg, "%s      0x%x\n\r", "Capability:", capability);
	  result = CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));

	  if (result != USBD_OK) {
	  	Error_Handler();
	  }*/



	  /*if (result != USBD_OK) {
		Error_Handler();
	  }*/

	  /*sprintf(msg, "%s 0x%x\n\r", "Status Specific:", bcm.status_specific.all);
	  result = CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));

	  if (result != USBD_OK) {
		Error_Handler();
	  }

	  sprintf(msg, "%s     0x%x\n\r", "Temperature:", bcm.measurements.temperature);
	  result = CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));

	  if (result != USBD_OK) {
		Error_Handler();
	  }

	  sprintf(msg, "%s     0x%x\n\r", "Status Iout:", bcm.status_iout.all);
	  result = CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));

	  if (result != USBD_OK) {
		Error_Handler();
	  }

	  sprintf(msg, "%s    0x%x\n\r", "Status Input:", bcm.status_input.all);
	  result = CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));

	  if (result != USBD_OK) {
		Error_Handler();
	  }

	  sprintf(msg, "%s %d\n\r", "IOUT(A): ", bcm.measurements.iout);
	  result = CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));

	  if (result != USBD_OK) {
		Error_Handler();
	  }

	  sprintf(msg, "%s  %d\n\r", "IIN(A): ", bcm.measurements.iin);
	  result = CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));

	  if (result != USBD_OK) {
		Error_Handler();
	  }

	  sprintf(msg, "%s%d\n\r", "Power(W): ", bcm.measurements.pout);
	  result = CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));

	  if (result != USBD_OK) {
		Error_Handler();
	  }*/

	  /*print_data("Part Number: ", bcm.module_specific.partno);
	  print_data("Serial Number: ", bcm.module_specific.serialno);
	  print_data("Manufacture Date: ", bcm.module_specific.date);
	  print_data("Manufacture Revision: ", bcm.module_specific.revision);
	  print_data("Manufacture Location: ", bcm.module_specific.location);*/


	  if (bcm.status_cml.cmd_stat_rx) {
		sprintf(msg, "CML: Invalid Or Unsupported Command Received\r\n");
		CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));
	  }

	  if (bcm.status_cml.data_stat_rx) {
		sprintf(msg, "CML: Invalid Or Unsupported Data Received\r\n");
		CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));
	  }

	  if (bcm.status_cml.other) {
		sprintf(msg, "CML: Other Communication Faults\r\n");
		CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));
	  }

	  sprintf(msg, "\r\n");
	  CDC_Transmit_FS((uint8_t*)msg, (uint16_t)strlen(msg));
  }
  /* USER CODE END 3 */
}

void print_data(char *msg, char *data) {
	size_t len = strlen(msg)  // Description for data e.g. "Voltage Input: "
			   + strlen(data) // Actual data
			   + 2
			   + 1;           // '\0' null delimiter

	char buffer[len]; // Allocate space for message.
	snprintf(buffer, len, "%s%s\n\r", msg, data);
	uint8_t result = CDC_Transmit_FS((uint8_t*)buffer, (uint16_t)len);

    if (result != USBD_OK) {
		Error_Handler();
	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLQ;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 88000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_ENABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
