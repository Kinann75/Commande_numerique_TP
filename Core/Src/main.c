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
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "motor.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_BUF_SIZE 8
#define PERIODE_VITESSE 0.1
#define V_CONSIGNE 200
#define I_CONSIGNE 3
#define P 0.5
#define I 0.01
#define D 0
#define Te 0.00000625
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

extern uint8_t uartRxReceived;
extern uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE];
extern uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE];
uint8_t flag=0;
uint8_t flagADC=0;
uint32_t ADC_Buffer[20];
char chaine[30];
int vitesse=0;
int vitesse_buffer[2];
extern double courant_buffer[2];
double erreur[2];

double alpha1;

double alpha2[2];
double alpha_corrige[2];
int32_t valeur_timer;
double Current_loop_buffer[3];
extern sum;
extern Imoyen;
extern mesure_mean;
extern mesure_voltage;

int i=0;
int j=0;
alpha2[0]=0;
alpha2[1]=0;
erreur[0]=0;
erreur[1]=0;
alpha_corrige[0]=0;
alpha_corrige[1]=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
	HAL_Delay(1);
	shellInit();

	HAL_TIM_Base_Start(&htim2);

	if(HAL_OK!=HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED))
	{
		Error_Handler();
	}
	if(HAL_OK!=HAL_ADC_Start_DMA(&hadc1, ADC_Buffer, 10)){
		Error_Handler();
	}


	TIM3->CNT=32000;
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
	HAL_TIM_Base_Start_IT(&htim5);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);


	/*if(HAL_OK!=HAL_TIM_Base_Start(&htim1)){
		Error_Handler();
	}*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		if (flag==1)
		{
			HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin,GPIO_PIN_SET);
			for(i=0;i<70;i++)
			{
			}
			HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin,GPIO_PIN_RESET);
			flag=0;
		}
		// SuperLoop inside the while(1), only flag changed from interrupt could launch functions
		if(uartRxReceived){
			if(shellGetChar()){
				shellExec();
				shellPrompt();
			}
			uartRxReceived = 0;
		}

		if (flagADC==1)
		{
			//sprintf(chaine,"resultat de conversion : %li\r\n", ADC_Buffer[0]);
			//HAL_UART_Transmit(&huart2, (uint8_t *)chaine, strlen(chaine), HAL_MAX_DELAY);
			flagADC=0;
		}
		for(j=0;j<20;j++)
		{
			sum=sum+ADC_Buffer[j];
		}
		mesure_mean=sum/10;
		j=0;
		courant_buffer[0]=courant_buffer[1];
		mesure_voltage=((double)mesure_mean*3.3)/4096.0;
		Imoyen=(mesure_voltage-2.5)*12;
		courant_buffer[1]=Imoyen;
		sum=0;
		erreur[0]=erreur[1];
		alpha2[0]=alpha2[1];
		erreur[1]=I_CONSIGNE-courant_buffer[1];
		alpha_corrige[0]=alpha_corrige[1];
		alpha1=erreur[1]*P;
		alpha2[1]=alpha2[0]+((I*Te)/2)*(erreur[1]-erreur[0]);
		alpha_corrige[1]=alpha_corrige[0]+alpha1+alpha2[1];
		if (alpha_corrige[1]>50)
		{
			alpha_corrige[1]=50;
		}
		else if(alpha_corrige[1]<0)
		{
			alpha_corrige[1]=0;
		}
		set_alpha((50+(int)alpha_corrige[1]));

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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV6;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	flag=1;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{

	flagADC=1;
}
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if(htim->Instance==TIM5){
	  valeur_timer=TIM3->CNT;
	  vitesse_buffer[0]=vitesse;
	  vitesse=((((valeur_timer-32000)*60)/4096)/PERIODE_VITESSE);
	  vitesse_buffer[1]=vitesse;
	  TIM3->CNT=32000;

  }
  /* USER CODE END Callback 1 */
}

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
