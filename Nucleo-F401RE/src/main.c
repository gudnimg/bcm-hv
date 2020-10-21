#include <string.h>
#include <stdbool.h> 
#include "main.h"

UART_HandleTypeDef huart1;
I2C_HandleTypeDef hi2c1;

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MX_I2C1_Init();
  MX_USART2_UART_Init();

  while (1)
  {
    char msg[53];

    read_status_specific(&hi2c1);
    read_status_cml(&hi2c1);
    read_status_iout(&hi2c1);
    read_status_input(&hi2c1);

    clear_faults(&hi2c1);

    read_temperature(&hi2c1);
    read_iout(&hi2c1);
    read_iin(&hi2c1);
    read_pout(&hi2c1);

    HAL_Delay(1000);

    sprintf(msg, "%s      0x%x\n\r", "Status CML:", bcm.status_cml.all);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 0xFFFF);

    sprintf(msg, "%s 0x%x\n\r", "Status Specific:", bcm.status_specific.all);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 0xFFFF);

    sprintf(msg, "%s     0x%x\n\r", "Temperature:", bcm.measurements.temperature);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 0xFFFF);

    sprintf(msg, "%s     0x%x\n\r", "Status Iout:", bcm.status_iout.all);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 0xFFFF);

    sprintf(msg, "%s    0x%x\n\r", "Status Input:", bcm.status_input.all);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 0xFFFF);

    sprintf(msg, "%s %d\n\r", "IOUT(A): ", bcm.measurements.iout);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 0xFFFF);

    sprintf(msg, "%s  %d\n\r", "IIN(A): ", bcm.measurements.iin);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 0xFFFF);

    sprintf(msg, "%s%d\n\r", "Power(W): ", bcm.measurements.pout);
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 0xFFFF);

    if (bcm.status_cml.cmd_stat_rx) {
      sprintf(msg, "CML: Invalid Or Unsupported Command Received\r\n");
      HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 0xFFFF);
    }

    if (bcm.status_cml.data_stat_rx) {
      sprintf(msg, "CML: Invalid Or Unsupported Data Received\r\n");
      HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 0xFFFF);
    }

    if (bcm.status_cml.other) {
      sprintf(msg, "CML: Other Communication Faults\r\n");
      HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 0xFFFF);
    }

    sprintf(msg, "\r\n");
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), 0xFFFF);
  }
}

// I2C and UART setup

void MX_I2C1_Init(void)
{
  
  hi2c1.Instance = I2C1;

  // Step 3. Configure the Communication Speed, Duty cycle, Addressing mode, Own Address1, Dual Addressing mode,
  // Own Address2, General call and Nostretch mode in the hi2c Init structure.
  hi2c1.Init.ClockSpeed      = 400000;
  hi2c1.Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
  hi2c1.Init.OwnAddress1     = 0;
  hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
  hi2c1.Init.OwnAddress2     = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
  hi2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLED;

  if (HAL_I2C_Init(&hi2c1) != HAL_OK) asm("bkpt 255");
  
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c1) {
  GPIO_InitTypeDef GPIO_InitStruct;

  if (hi2c1->Instance == I2C1)
  {
    // I2C1 GPIO Configuration
    // PB6     ------> I2C1_SCL
    // PB7     ------> I2C1_SDA
    __GPIOB_CLK_ENABLE();
    __I2C1_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    //GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    //Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    //Error_Handler();
  }
}

void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0);
}

void MX_USART2_UART_Init(void)
{
	huart1.Instance = USART2;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  //huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart1);
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance==USART2)
  {
    __USART2_CLK_ENABLE();
    __GPIOA_CLK_ENABLE();
 
    /**USART1 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

void SysTick_Handler(void) {
  HAL_IncTick();
}