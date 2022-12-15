/**
  ******************************************************************************
  * @file    shell.c
  * @brief   This file provides code for motor control
  ******************************************************************************
  *  Created on: Nov 7, 2022
  *      Author: nicolas
  *
  ******************************************************************************
  */

#include "motor.h"
#include "usart.h"
#include "tim.h"

/**
  * @brief  Switch on the motor driver
  * @retval None
  */
void motorPowerOn(void){
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin); // just for test, you can delete it
	HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin,GPIO_PIN_SET);
	int i;
	for(i=0;i<70;i++)
	{
	}
    HAL_GPIO_WritePin(ISO_RESET_GPIO_Port, ISO_RESET_Pin,GPIO_PIN_RESET);
}

/**
  * @brief  Switch off the motor driver
  * @retval None
  */
void motorPowerOff(void){
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin); // just for test, you can delete it
}

/**
  * @brief  Set the motor speed
  * @param  speed : target speed of the motor
  * @retval None
  */
void motorSetSpeed(int speed){
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin); // just for test, you can delete it
}

void set_alpha(int a)
{
	//HAL_UART_Transmit(&huart2, a, sizeof(a), HAL_MAX_DELAY);
	TIM1->CCR1=a*53;
	TIM1->CCR2=5311-a*53;

}
