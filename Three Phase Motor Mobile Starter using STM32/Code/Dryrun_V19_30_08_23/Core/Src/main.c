/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include<stdio.h>
#include<string.h>
#include "FLASH_SECTOR_F4.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef unsigned char uc;
typedef uint8_t ui8;
typedef uint16_t ui16;
typedef unsigned int ui;
typedef uint32_t ui32;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define SW_DryrunBypass		GPIO_PIN_9	//PE9
#define SW_DryrunSet		GPIO_PIN_10	//PE10
#define SW_AutoManual		GPIO_PIN_8	//PE8

#define MOTOR_LED		GPIO_PIN_14	//PE14
#define SPP_LED			GPIO_PIN_15	//PE15
#define DRN_LED			GPIO_PIN_1	//PB1

#define SPP_Sense		GPIO_PIN_11	//PE11

#define Relay_Start		GPIO_PIN_12	// PE12
#define Relay_Stop		GPIO_PIN_13	// PE13


#define SEG_A			GPIO_PIN_12	// PB12
#define SEG_B			GPIO_PIN_13	// PB13
#define SEG_C			GPIO_PIN_14	// PB14
#define SEG_D			GPIO_PIN_15	// PB15
#define SEG_E			GPIO_PIN_8	// PD8
#define SEG_F			GPIO_PIN_9	// PD9
#define SEG_G			GPIO_PIN_10	// PD10
#define SEG_DP			GPIO_PIN_11	// PD11

#define DISP_1			GPIO_PIN_9	// PC9 = DIGIT-1
#define DISP_2			GPIO_PIN_8	// PC8 = DIGIT-2
#define DISP_3			GPIO_PIN_6	// PC6 = DIGIT-3

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

//Flags

ui8 RYB_Flag = 0;
ui8 PhaseFault_Flag = 0;
ui8 R_Flag = 0;
ui8 Y_Flag = 0;
ui8 B_Flag = 0;
ui8 Motor_Status = 0;
ui8 Dryrun_Flag = 0;

ui16 R_Voltage = 0;
ui16 Y_Voltage = 0;
ui16 B_Voltage = 0;

ui16 count;
volatile ui16 count1;
ui16 adc_val=0;

//EEPROM Memory Addresses

ui32 write_add = 0;
uc write_buffer[2] = {0xFF,0xFF};

ui32 read_add = 0;
uc Read_Buffer[2] = {0};

ui16 E_value = 0;

//UART
ui8 tx_buf1[30] = {0};


//Phase, Current & POT ADC Varaibles

ui chan_value_R = 0;
ui chan_value_Y = 0;
ui chan_value_B = 0;
ui chan_value_I = 0;
ui Start_Delay = 0;

ui16 CT_Val = 0;
ui16 CT_High_Limit=0;
ui16 CT_Low_Limit=0;
ui16 I_Current = 0;

int i, k, m, s=0, t=0;
ui8 Current_Flag = 0, Voltage_Flag = 0;


// GSM
char buf_on[100] = {0};
char buf_off[100] = {0};
char buf_tim[100] = {0};
char cmd[64] = {0};
char msg[128] = {0};
ui16 tim_count = 0;
ui16 tim_sec = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM7_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */

void Syst_Init(void);
void HW_Test();
void init_disp();

void zero_0();
void one_1();
void two_2();
void three_3();
void four_4();
void five_5();
void six_6();
void seven_7();
void eight_8();
void nine_9();

void disp(unsigned int);

void CheckPhaseVoltage();
void CheckDryrun();
void MotorON();
void MotorOFF();
void DryRunSet();

void led_blink(uint16_t pin, uint16_t delay);
void led_off(uint16_t pin);

void ADC_Select_VADC_R(void);
void ADC_Select_VADC_Y(void);
void ADC_Select_VADC_B(void);
void ADC_Select_IADC_R(void);
void ADC_Select_STS(void);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

void msDelay(uint16_t timecount);

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
  MX_ADC1_Init();
  MX_TIM7_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  Syst_Init();
  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CMGDA=\"DEL ALL\"\r\n", 20, 1000);

  write_add = 0x08005000;	//EEPROM Write Address

  read_add = 0x08005000;	//EEPROM Read Address

  __disable_irq();
  Flash_Read_Data(read_add, Read_Buffer, 4);	//Read the value from EEPROM
  __enable_irq();

  E_value = Read_Buffer[1];			//Write MSB of EEPROM value
  E_value = (E_value<<8)| Read_Buffer[0];		//Write LSB of EEPROM value
  HAL_Delay(20);

  CT_High_Limit = E_value + ((E_value/100)*30);	//Set High Limit of Current
  CT_Low_Limit = E_value - ((E_value/100)*30);	//Set Low Limit of Current

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CMGR=1\r\n", 11, 100);
	  HAL_UART_Receive(&huart2, (uint8_t *)buf_on, sizeof(buf_on), 100);
	  if(strstr(buf_on, "*ON#"))
	  {
		  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CMGR=2\r\n", 11, 1000);
		  HAL_UART_Receive(&huart2, (uint8_t *)buf_tim, sizeof(buf_tim), 1000);
		  if(strstr(buf_tim, "*T1#") || strstr(buf_tim, "*T2#") || strstr(buf_tim, "*T3#"))
		  {
			  if(strstr(buf_tim, "*T1#"))
			  {
				  tim_count = 60;
			  }
			  else if(strstr(buf_tim, "*T2#"))
			  {
				  tim_count = 120;
			  }
			  else if(strstr(buf_tim, "*T3#"))
			  {
				  tim_count = 180;
			  }
			  HAL_TIM_Base_Start_IT(&htim6);
			  if(tim_sec <= tim_count)
			  {
				  CheckPhaseVoltage();

				  if(HAL_GPIO_ReadPin(GPIOE, SW_AutoManual) == 1)      // if Manual Mode is ON
				  {
					  printf(tx_buf1,"Auto Mode Manual\r ");
					  HAL_UART_Transmit(&huart1, tx_buf1, 17, 100);

					  if(Motor_Status==1)	// if Motor is ON, turn Motor OFF
					  {
						  MotorOFF();
					  }
				  }
				  if(HAL_GPIO_ReadPin(GPIOE, SPP_Sense) == 0)	// if Phase Fault occurs  (Need to check with hardware)
				  {
					  for(count = 0; count<10; count++)   // Checking 10 times
					  {
						  if(HAL_GPIO_ReadPin(GPIOE, SPP_Sense) == 0)
						  {
							  PhaseFault_Flag = 1;
						  }
						  else
						  {
							  PhaseFault_Flag = 0;
							  break;
						  }
					  }
					  if(PhaseFault_Flag == 1)	//Turn Motor OFF if PhaseFault occurs
					  {
						  if(Motor_Status==1)	// if Motor is ON, turn Motor OFF
						  {
							  MotorOFF();
							  HAL_GPIO_WritePin(GPIOE, SPP_LED, GPIO_PIN_RESET);
							  sprintf(tx_buf1,"Phase Fault\r");
							  HAL_UART_Transmit(&huart1, tx_buf1, 11, 100);
						  }
					  }
					  else
					  {
						  HAL_GPIO_WritePin(GPIOE, SPP_LED, GPIO_PIN_SET);
					  }

				  }

				  if(R_Flag==1 || Y_Flag==1 || B_Flag==1)
				  {
					  for(count = 0; count<10; count++)   // Checking 10 times
					  {
						  if(R_Flag==1 || Y_Flag==1 || B_Flag==1)
						  {
							  RYB_Flag = 1;
						  }

					  }
					  if(RYB_Flag == 1)	//Turn Motor OFF if PhaseFault occurs
					  {
						  if(Motor_Status==1)	// if Motor is ON, turn Motor OFF
						  {
							  MotorOFF();
							  sprintf(tx_buf1,"K Fault\r");
							  HAL_UART_Transmit(&huart1, tx_buf1, 9, 100);
						  }
					  }
				  }
				  else
				  {
					  RYB_Flag = 0;

				  }

				  if(Motor_Status == 0 && HAL_GPIO_ReadPin(GPIOE, SW_AutoManual) == 0 && PhaseFault_Flag == 0 && RYB_Flag == 0 && Dryrun_Flag == 0)	// Checking for Motor ON & all fault status, if Motor OFF & all fault Clear then Motor ON
				  {
					  MotorON();
					  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CMGR=3\r\n", 11, 1000);
					  HAL_UART_Receive(&huart2, (uint8_t *)buf_off, sizeof(buf_off), 1000);
					  if(strstr(buf_off, "*OFF#"))
					  {
						  MotorOFF();
						  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CMGDA=\"DEL ALL\"\r\n", 20, 1000);
					  }
				  }
				  if(Motor_Status == 1 && HAL_GPIO_ReadPin(GPIOE, SW_AutoManual) == 0 && PhaseFault_Flag == 0 && RYB_Flag == 0 )	// Checking for Motor ON & all fault status, if Motor ON & all fault Clear then Check for Bypass
				  {
					  if(HAL_GPIO_ReadPin(GPIOE, SW_DryrunBypass) == 0)   		// If Bypass is OFF, check for Dryrun
					  {
						  if(E_value<0xFFFF)		// Check for EEPROM value, if Current reference value exist then check for Dryrun
						  {
							  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CMGR=3\r\n", 11, 1000);
							  HAL_UART_Receive(&huart2, (uint8_t *)buf_off, sizeof(buf_off), 1000);
							  if(strstr(buf_off, "*OFF#"))
							  {
								  MotorOFF();
								  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CMGDA=\"DEL ALL\"\r\n", 20, 1000);
							  }
							  CheckDryrun();
							  if(Dryrun_Flag == 1)	// If Dryrun occurs
							  {
								  HAL_GPIO_WritePin(GPIOB, DRN_LED, GPIO_PIN_RESET);
								  MotorOFF();
								  sprintf(tx_buf1,"Dryrun Detected\r");
								  HAL_UART_Transmit(&huart1, tx_buf1, 16, 100);
							  }
							  else
							  {
								  HAL_GPIO_WritePin(GPIOB, DRN_LED, GPIO_PIN_SET);
								  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CMGR=3\r\n", 11, 1000);
								  HAL_UART_Receive(&huart2, (uint8_t *)buf_off, sizeof(buf_off), 1000);
								  if(strstr(buf_off, "*OFF#"))
								  {
									  MotorOFF();
									  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CMGDA=\"DEL ALL\"\r\n", 20, 1000);
								  }
							  }
						  }
						  if(HAL_GPIO_ReadPin(GPIOE, SW_DryrunSet) == 0)		// If Dryrun Switch is pressed
						  {
							  sprintf(tx_buf1,"Dryrun Set Pressed\r ");
							  HAL_UART_Transmit(&huart1, tx_buf1, 18, 100);
							  DryRunSet();
							  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CMGR=3\r\n", 11, 1000);
							  HAL_UART_Receive(&huart2, (uint8_t *)buf_off, sizeof(buf_off), 1000);
							  if(strstr(buf_off, "*OFF#"))
							  {
								  MotorOFF();
								  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CMGDA=\"DEL ALL\"\r\n", 20, 1000);
							  }
						  }
					  }
					  else
					  {
						  sprintf(tx_buf1,"Dryrun Bypass\r");
						  HAL_UART_Transmit(&huart1, tx_buf1, 14, 100);
						  if(HAL_GPIO_ReadPin(GPIOE, SW_DryrunSet) == 0)		// If Dryrun Switch is pressed
						  {
							  sprintf(tx_buf1,"Dryrun Set Pressed\r");
							  HAL_UART_Transmit(&huart1, tx_buf1, 18, 100);
							  DryRunSet();
							  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CMGR=3\r\n", 11, 1000);
							  HAL_UART_Receive(&huart2, (uint8_t *)buf_off, sizeof(buf_off), 1000);
							  if(strstr(buf_off, "*OFF#"))
							  {
								  MotorOFF();
								  HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CMGDA=\"DEL ALL\"\r\n", 20, 1000);
							  }
						  }
					  }
				  }
			  }
			  else
			  {
				  MotorOFF();
			  }
		  }
		  else
		  {
			  MotorOFF();
		  }

	  }
	  else
	  {
		  MotorOFF();
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
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
//  sConfig.Channel = ADC_CHANNEL_1;
//  sConfig.Rank = 1;
//  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//  */
//  sConfig.Channel = ADC_CHANNEL_2;
//  sConfig.Rank = 2;
//  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//  */
//  sConfig.Channel = ADC_CHANNEL_3;
//  sConfig.Rank = 3;
//  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//  */
//  sConfig.Channel = ADC_CHANNEL_11;
//  sConfig.Rank = 4;
//  sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
//
//  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
//  */
//  sConfig.Channel = ADC_CHANNEL_12;
//  sConfig.Rank = 5;
//  sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
//  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
//  {
//    Error_Handler();
//  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 12499;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 60000;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 12499;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 10;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

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
  huart1.Init.BaudRate = 9600;
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
  huart2.Init.BaudRate = 9600;
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

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB1 PB10 PB12 PB13
                           PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PE8 PE9 PE10 PE11 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PE12 PE13 PE14 PE15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 PD10 PD11 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PC6 PC8 PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PD1 PD2 PD3 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void Syst_Init(void)
{
	//__enable_irq();

	HAL_GPIO_WritePin(GPIOB, DRN_LED, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, MOTOR_LED, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, SPP_LED, GPIO_PIN_SET);

    HAL_GPIO_WritePin(GPIOC, DISP_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, DISP_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, DISP_3, GPIO_PIN_RESET);

	init_disp();
    HW_Test();

    HAL_GPIO_WritePin(GPIOE, Relay_Start, GPIO_PIN_RESET);
    MotorOFF();


    HAL_TIM_Base_Start_IT(&htim7);
}

void HW_Test()
{
	HAL_GPIO_WritePin(GPIOB, DRN_LED, GPIO_PIN_RESET);
	HAL_Delay(1000);

	HAL_GPIO_WritePin(GPIOE, MOTOR_LED, GPIO_PIN_RESET);
	HAL_Delay(1000);

	HAL_GPIO_WritePin(GPIOE, SPP_LED, GPIO_PIN_RESET);
	HAL_Delay(1000);

	HAL_GPIO_WritePin(GPIOE, SPP_LED, GPIO_PIN_SET);
	HAL_Delay(1000);

	HAL_GPIO_WritePin(GPIOE, MOTOR_LED, GPIO_PIN_SET);
	HAL_Delay(1000);

	HAL_GPIO_WritePin(GPIOB, DRN_LED, GPIO_PIN_SET);
}

void init_disp()
{
	HAL_GPIO_WritePin(GPIOC, DISP_3, GPIO_PIN_SET); // DIG3
    HAL_GPIO_WritePin(GPIOC, DISP_2, GPIO_PIN_SET); // DIG2
    HAL_GPIO_WritePin(GPIOC, DISP_1, GPIO_PIN_SET); // DIG1



    disp(0);
    HAL_Delay(300);
    disp(1);
    HAL_Delay(300);
    disp(2);
    HAL_Delay(300);
    disp(3);
    HAL_Delay(300);
    disp(4);
    HAL_Delay(300);
    disp(5);
    HAL_Delay(300);
    disp(6);
    HAL_Delay(300);
    disp(7);
    HAL_Delay(300);
    disp(8);
    HAL_Delay(300);
    disp(9);
    HAL_Delay(300);

    HAL_GPIO_WritePin(GPIOC, DISP_3, GPIO_PIN_RESET); // DIG3
    HAL_GPIO_WritePin(GPIOC, DISP_2, GPIO_PIN_RESET); // DIG2
    HAL_GPIO_WritePin(GPIOC, DISP_1, GPIO_PIN_RESET); // DIG1

    HAL_Delay(2000);
}

void CheckPhaseVoltage()
{
	ADC_Select_VADC_R();   //select channel 12

	HAL_ADC_Start(&hadc1);	//Start ADC
	HAL_ADC_PollForConversion(&hadc1, 100);
	chan_value_R = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);  	//Stop ADC
	HAL_Delay(50);

	ADC_Select_VADC_Y();   //select channel 11

	HAL_ADC_Start(&hadc1);	//Start ADC
	HAL_ADC_PollForConversion(&hadc1, 100);
	chan_value_Y = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);  	//Stop ADC
	HAL_Delay(50);


	ADC_Select_VADC_B();   //select channel 3

	HAL_ADC_Start(&hadc1);	//Start ADC
	HAL_ADC_PollForConversion(&hadc1, 100);
	chan_value_B = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);  	//Stop ADC
	HAL_Delay(50);

	R_Voltage = (chan_value_R * 40)/100;

	Y_Voltage = (chan_value_Y * 40)/100;

	B_Voltage = (chan_value_B * 40)/100;


	HAL_UART_Transmit(&huart1, chan_value_Y, 2, 100);

	if(R_Voltage < 180)	//if R phase Voltage is less than 180 then Set R_Flag
	{
		R_Flag = 1;
		sprintf(tx_buf1,"R Fault\r ");
		HAL_UART_Transmit(&huart1, tx_buf1, 7, 100);
	}
	else
	{
		R_Flag = 0;
	}
	if(Y_Voltage < 180)	//if Y phase Voltage is less than 180 then Set Y_Flag
	{
		Y_Flag = 1;
		sprintf(tx_buf1,"Y Fault\r ");
		HAL_UART_Transmit(&huart1, tx_buf1, 7, 100);
	}
	else
	{
		Y_Flag = 0;
	}
	if(B_Voltage < 180)	//if B phase Voltage is less than 180 then Set B_Flag
	{
		B_Flag = 1;
		sprintf(tx_buf1,"B Fault\r ");
		HAL_UART_Transmit(&huart1, tx_buf1, 7, 100);
	}
	else
	{
		B_Flag = 0;
	}
}

void CheckDryrun()
{
	for(count = 0; count<10; count++)	// Checking 10 times
	{
		ADC_Select_IADC_R();   //select channel 2

		HAL_ADC_Start(&hadc1);	//Start ADC
		HAL_ADC_PollForConversion(&hadc1, 100);
		chan_value_I = HAL_ADC_GetValue(&hadc1);
		HAL_ADC_Stop(&hadc1);  	//Stop ADC
		HAL_Delay(50);

		CT_Val = chan_value_I;		//Assign Current Value

		I_Current = chan_value_I / 100;


		if(CT_Val < CT_Low_Limit || CT_Val > CT_High_Limit)	//if CT value is less than Low Limit & greater than High Limit then Set Dryrun Flag
		{
			HAL_GPIO_WritePin(GPIOB, DRN_LED, GPIO_PIN_RESET);
			Dryrun_Flag = 1;
			HAL_Delay(100);

		}
		else if(CT_Val > CT_Low_Limit && CT_Val < CT_High_Limit)	//if CT value is in between Low Limit & High Limit then Clear Dryrun Flag
		{
			HAL_GPIO_WritePin(GPIOB, DRN_LED, GPIO_PIN_SET);
			Dryrun_Flag = 0;
		}
	}
}

void MotorON()
{
	ADC_Select_STS();   //select channel 1

	HAL_ADC_Start(&hadc1);	//Start ADC
	HAL_ADC_PollForConversion(&hadc1, 100);
	Start_Delay = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);  	//Stop ADC
	HAL_Delay(50);

	Start_Delay = Start_Delay * 10;

	//msDelay(Start_Delay);   //Need to update
	HAL_Delay(3000);

	HAL_GPIO_WritePin(GPIOE, Relay_Start, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, Relay_Stop, GPIO_PIN_SET);

	sprintf(tx_buf1,"MOTOR ON\r");
	HAL_UART_Transmit(&huart1, tx_buf1, 9, 100);

	HAL_Delay(3000);
	HAL_GPIO_WritePin(GPIOE, Relay_Start, GPIO_PIN_RESET);
	Motor_Status = 1;

	HAL_GPIO_WritePin(GPIOE, MOTOR_LED, GPIO_PIN_RESET);
}

void MotorOFF()
{
	HAL_TIM_Base_Stop_IT(&htim6);
	tim_count = 0;
	tim_sec = 0;

	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOE, Relay_Stop, GPIO_PIN_RESET);
	Motor_Status = 0;

	HAL_GPIO_WritePin(GPIOE, MOTOR_LED, GPIO_PIN_SET);

	sprintf(tx_buf1,"MOTOR OFF\r");
	HAL_UART_Transmit(&huart1, tx_buf1, 10, 100);
}

void DryRunSet()
{
	if(HAL_GPIO_ReadPin(GPIOE, SW_DryrunSet) == 0)		//if Dryrun Set Switch is pressed
	{
		HAL_Delay(3000);
		if(HAL_GPIO_ReadPin(GPIOE, SW_DryrunSet) == 0)
		{

			ADC_Select_IADC_R();   //select channel 2

			HAL_ADC_Start(&hadc1);	//Start ADC
			HAL_ADC_PollForConversion(&hadc1, 100);
			chan_value_I = HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Stop(&hadc1);  	//Stop ADC
			HAL_Delay(50);

			write_buffer[0] = chan_value_I;
			write_buffer[1] = chan_value_I >> 8 & 0x00FF;


			__disable_irq();
			//HAL_Delay(20);
			Flash_Write_Data(write_add, write_buffer, 4); 	// write one value to Data Flash
			//HAL_Delay(20);
			__enable_irq();

			CT_Val = chan_value_I;				//Assign Current Value
			CT_High_Limit = CT_Val + ((CT_Val/100)*30);	//Set Current High Limit
			CT_Low_Limit = CT_Val - ((CT_Val/100)*30);	//Set Current Low Limit





			HAL_Delay(3000);
			HAL_GPIO_WritePin(GPIOB, DRN_LED, GPIO_PIN_RESET);
			HAL_Delay(500);
			HAL_GPIO_WritePin(GPIOB, DRN_LED, GPIO_PIN_SET);
			HAL_Delay(500);
			HAL_GPIO_WritePin(GPIOB, DRN_LED, GPIO_PIN_RESET);
			HAL_Delay(500);
			HAL_GPIO_WritePin(GPIOB, DRN_LED, GPIO_PIN_SET);
			HAL_Delay(500);
			HAL_GPIO_WritePin(GPIOB, DRN_LED, GPIO_PIN_RESET);
			HAL_Delay(500);
			HAL_GPIO_WritePin(GPIOB, DRN_LED, GPIO_PIN_SET);

			HAL_Delay(20);


		}
		else
		{
			HAL_GPIO_WritePin(GPIOB, DRN_LED, GPIO_PIN_SET);
		}
	}
}

void zero_0()
{
	HAL_GPIO_WritePin(GPIOB, SEG_A, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_B, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_C, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_D, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_E, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_F, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_G, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_DP, GPIO_PIN_RESET);
}
void one_1()
{
	HAL_GPIO_WritePin(GPIOB, SEG_A, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, SEG_B, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_C, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_D, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_E, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_F, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_G, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_DP, GPIO_PIN_RESET);
}
void two_2()
{
	HAL_GPIO_WritePin(GPIOB, SEG_A, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_B, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_C, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, SEG_D, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_E, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_F, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_G, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_DP, GPIO_PIN_RESET);
}
void three_3()
{
	HAL_GPIO_WritePin(GPIOB, SEG_A, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_B, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_C, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_D, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_E, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_F, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_G, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_DP, GPIO_PIN_RESET);
}
void four_4()
{
	HAL_GPIO_WritePin(GPIOB, SEG_A, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, SEG_B, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_C, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_D, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_E, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_F, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_G, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_DP, GPIO_PIN_RESET);
}
void five_5()
{
	HAL_GPIO_WritePin(GPIOB, SEG_A, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_B, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, SEG_C, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_D, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_E, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_F, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_G, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_DP, GPIO_PIN_RESET);
}
void six_6()
{
	HAL_GPIO_WritePin(GPIOB, SEG_A, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_B, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, SEG_C, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_D, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_E, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_F, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_G, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_DP, GPIO_PIN_RESET);
}
void seven_7()
{
	HAL_GPIO_WritePin(GPIOB, SEG_A, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_B, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_C, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_D, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_E, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_F, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_G, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_DP, GPIO_PIN_RESET);
}
void eight_8()
{
	HAL_GPIO_WritePin(GPIOB, SEG_A, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_B, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_C, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_D, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_E, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_F, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_G, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_DP, GPIO_PIN_RESET);
}
void nine_9()
{
	HAL_GPIO_WritePin(GPIOB, SEG_A, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_B, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_C, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SEG_D, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_E, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, SEG_F, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_G, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, SEG_DP, GPIO_PIN_RESET);
}

void disp(unsigned int s)
{
	if(s==0)
		zero_0();
	else if(s==1)
		one_1();
	else if(s==2)
		two_2();
	else if(s==3)
		three_3();
	else if(s==4)
		four_4();
	else if(s==5)
		five_5();
	else if(s==6)
		six_6();
	else if(s==7)
		seven_7();
	else if(s==8)
		eight_8();
	else if(s==9)
		nine_9();
	else
		zero_0();
}

//void led_blink(uint16_t pin, uint16_t delay)
//{
//	HAL_GPIO_WritePin(GPIOE, pin, GPIO_PIN_SET);
//	HAL_Delay(1000);
//	HAL_GPIO_WritePin(GPIOE, pin, GPIO_PIN_RESET);
//	HAL_Delay(1000);
//}
//
//void led_off(uint16_t pin)
//{
//	HAL_GPIO_WritePin(GPIOE, pin, GPIO_PIN_RESET);
//}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
		tim_sec++;

	else if(htim->Instance == TIM7)
	{
		count1++;

		if(count1 < 500)
		{
			Voltage_Flag = 1;

			Current_Flag = 0;
		}
		else if (count1 > 499 && count1 < 1000)
		{
			Voltage_Flag = 0;

			Current_Flag = 1;
		}

		else
		{
			count1 = 0;
		}

		if(Voltage_Flag == 1 && Current_Flag == 0)
		{
			adc_val = R_Voltage;
		}

		if(Voltage_Flag == 0 && Current_Flag == 1)
		{
			adc_val = I_Current;
		}

		i = adc_val%10;
		k = (adc_val%100)/10;
		m = adc_val/100;

		HAL_GPIO_WritePin(GPIOC, DISP_3, GPIO_PIN_SET); // DIG3
		HAL_GPIO_WritePin(GPIOC, DISP_2, GPIO_PIN_RESET); // DIG2
		HAL_GPIO_WritePin(GPIOC, DISP_1, GPIO_PIN_RESET); // DIG1

		disp(i);
		msDelay(40);

		HAL_GPIO_WritePin(GPIOC, DISP_3, GPIO_PIN_RESET); // DIG3
		HAL_GPIO_WritePin(GPIOC, DISP_2, GPIO_PIN_SET); // DIG2
		HAL_GPIO_WritePin(GPIOC, DISP_1, GPIO_PIN_RESET); // DIG1

		disp(k);
		msDelay(40);

		HAL_GPIO_WritePin(GPIOC, DISP_3, GPIO_PIN_RESET); // DIG3
		HAL_GPIO_WritePin(GPIOC, DISP_2, GPIO_PIN_RESET); // DIG2
		HAL_GPIO_WritePin(GPIOC, DISP_1, GPIO_PIN_SET); // DIG1

		disp(m);
		msDelay(1);
	}
}

void ADC_Select_STS(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};

	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

void ADC_Select_IADC_R(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

void ADC_Select_VADC_B(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

void ADC_Select_VADC_Y(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

void ADC_Select_VADC_R(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = ADC_CHANNEL_12;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

void msDelay(uint16_t timecount)
{
	int x,y;
	for(x=0;x<=timecount;x++)
		for(y=0;y<=100;y++);
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
