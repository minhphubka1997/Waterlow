/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2020 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "STM_MY_LCD16X2.h"
#include "app.h"
#include "lora.h"
#include "sim.h"
#include "dwt_stm32_delay.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define delay_us	DWT_Delay_us
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc2;

IWDG_HandleTypeDef hiwdg;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_IWDG_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC2_Init(void);
/* USER CODE BEGIN PFP */

void ph_measure_process(void);
void distance_measure_process(void);
void LoraReceiveDMA(void);
void SIMReceiveDMA(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t LcdState = 0;
uint8_t LcdbtnOldState = 0;
void LcdHandler(void)
{
	char CrLcdBtnState = Lcd_button();
	
	if ((LcdbtnOldState == 0) &&( CrLcdBtnState == 1))	// Press event
	{
		if (LcdState == 0) 
		{
			Lcd1602_ON();
			LcdState = 1;
		}
		else
		{
			Lcd1602_OFF();
			LcdState = 0;
		}
	}
	
	LcdbtnOldState = CrLcdBtnState;
}
//

uint16_t Distance = 0;
float PHValue = 0;
uint16_t ID = 2;
uint32_t LoraSendDataTime = 0, LCDTimeUpdate = 0;
uint32_t TimeNotifyLoraLoss1 = 0;
uint32_t TimeNotifyLoraLoss2 = 0;
bool isNotifyLoraNoConnect = false;

bool SensorValueChanged = true;

bool FlagN1PumpOnViaSMS = false;	// Flag require Node1 On PUMP via SMS
bool FlagN1PumpOffViaSMS = false;	// Flag require Node1 Off PUMP via SMS

bool FlagUpdateDataViaSMS	= false;

bool GWPumpNeedOn = false;				// Flag require Gateway On/Off pump via Lora signal
bool Node1PumpNeedOn = false;			// Flag require Node1 On/Off pump via Lora signal

bool FlagWarningPHValueToUser = false;

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
  MX_DMA_Init();
  MX_IWDG_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
	OFF_4V();
	if (DWT_Delay_Init() != 0) Error_Handler();
	
	Lcd1602_ON();
	LCD1602_Begin4BIT(RS_GPORT, RS_GPIN, EN_GPIN, D4_GPORT, D4_GPIN, D5_GPIN, D6_GPIN, D7_GPIN);
	LCD1602_noBlink();
	LCD1602_noCursor();
	LCD1602_1stLine();
	LCD1602_print("Node 2");
	
	HAL_ADC_Start(&hadc2);
	Lora_Mode0();
	
	#if USING_LORA_RECEIVE_DMA > 0
	LoraReceiveDMA();
	#endif
	
	#if USING_SIM_RECEIVE_DMA > 0
	SIMReceiveDMA();
	#endif
	
	
	HAL_Delay(200);
	sim_async_init();
	TimeNotifyLoraLoss1 = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
		/*----------- Measurement distance-------------*/
		distance_measure_process();
		//
		
		/*-------------- Measure PH value --------------*/
		ph_measure_process();
		
		
		/*---------------- Lora receive message -----------*/
		lora_receive_process();
		
		/*-------------------  SIM process --------------*/
		sim_process();
		
		
		
		/*-------------------  Gửi tin nhắn báo mất kết nối lora --------------*/
		if (((HAL_GetTick() - TimeNotifyLoraLoss1 >= 30000)&&(TimeNotifyLoraLoss1 != 0))
			||((HAL_GetTick()  - TimeNotifyLoraLoss2 >= 5000)&&(TimeNotifyLoraLoss1 == 0)))
		{
			if (lora_is_connected() == false)
			{
				if (isNotifyLoraNoConnect == false)	// Nếu chưa thông báo cho User biết đã mất kết nối Lora
				{
					if (sim_is_init_ok() && (sim_is_sendingMsg() == false))
					{
						char abc[100];
						sprintf(abc, "ID: %03d canh bao: mat ket noi lora", ID);
						sim_async_sendmsg(USER_PNB, abc);	// Thông báo
						isNotifyLoraNoConnect = true;
					}
				}
			}
			TimeNotifyLoraLoss1 = 0;
			TimeNotifyLoraLoss2 = HAL_GetTick();
		}
		
		
		
		/*---------print sensor value to LCD1602----------*/
		if (sim_is_initializing() == false)
		{
			if (HAL_GetTick() - LCDTimeUpdate >= 500)
			{
				if (SensorValueChanged)
				{
					LCD1602_2ndLine();
					char as[17];
					if (Distance == 9999)
					sprintf(as, "D: ?     PH:%3f",  PHValue);
					else
					if (Distance > 200)
					sprintf(as, "D:>200   PH:%3f",  PHValue);	
					else
					sprintf(as, "D:%4d   PH:%3f", Distance, PHValue);
					
					LCD1602_print(as);
					
					sprintf(as, "ID:%03d  S:%d  L:%d", ID,sim_is_init_ok(), lora_is_connected());
					LCD1602_1stLine();
					LCD1602_print(as);
				}
				LCDTimeUpdate = HAL_GetTick();
			}
		} else LCDTimeUpdate = HAL_GetTick();
		
		
				/*---------print sensor value to Server----------*/
		if (HAL_GetTick()   - LoraSendDataTime >= 3550)
		{
			char as[50];
			
			sprintf(as,"#ID%03dDPD%04dP%03dPP%d%d!",ID, Distance, (int)(PHValue *10), GWPumpNeedOn, Node1PumpNeedOn);
			lora_send_data((uint8_t*)as, strlen(as));
		
			if (FlagWarningPHValueToUser)
			{
				if (sim_is_init_ok() &&(sim_is_sendingMsg() == false))
				{
					char ac[120];
					sprintf(ac,"ID%03d canh bao: do PH < 6.5", ID);	// Gửi tin nhắn cảnh báo độ PH thấp cho User
					sim_async_sendmsg(USER_PNB, ac);
					FlagWarningPHValueToUser = false;
				}
			}
			
			if (lora_is_connected() == false)
			{
				if (sim_is_init_ok() &&(sim_is_sendingMsg() == false))
				{
					if (FlagN1PumpOffViaSMS )
					{
						sim_async_sendmsg(N1_PHONENUMBER, "PUMP_OFF");	// Gửi tin nhắn Tắt bơm cho Node1
						FlagN1PumpOffViaSMS = false;
					}
					if (FlagN1PumpOnViaSMS )
					{
						sim_async_sendmsg(N1_PHONENUMBER, "PUMP_ON");		// Gửi tin nhắn bật bơm cho Node1
						FlagN1PumpOnViaSMS = false;
					}
					
					if (FlagUpdateDataViaSMS)
					{
						sim_async_sendmsg(GW_PHONENUMBER,as);						// Gửi tin nhắn cho Gateway trạng thái hiện tại, để Gateway biết bật hay tắt bơm
						FlagUpdateDataViaSMS = false;
					}
				}
			}
			
			LoraSendDataTime = HAL_GetTick();
			
			SensorValueChanged = false;
		}
		
		LcdHandler();
		HAL_IWDG_Refresh(&hiwdg);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */
  /**Common config 
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = ENABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure Regular Channel 
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
static void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
  hiwdg.Init.Reload = 3999;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 19200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
	#if USING_LORA_RECEIVE_IT > 0
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	#endif
  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 19200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
	#if USING_SIM_RECEIVE_IT > 0
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	#endif
  /* USER CODE END USART2_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_4 
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8 
                          |GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB11 PB13 PB4 
                           PB5 PB6 PB7 PB8 
                           PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_4 
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8 
                          |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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

	while(1);
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
