/**
  ******************************************************************************
  * @file    shell.c
  * @brief   This file provides code for shell interface
  ******************************************************************************
  *  Created on: Nov 7, 2022
  *      Author: nicolas
  *
  ******************************************************************************
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shell.h"
#include "usart.h"
#include "motor.h"


const uint8_t prompt[]="user@Nucleo-STM32G4RB31>>";
const uint8_t started[]=
		"\r\n*-------------------------------*"
		"\r\n| Welcome on Nucleo-STM32G431RB |"
		"\r\n*-------------------------------*"
		"\r\n";
const uint8_t newline[]="\r\n";
const uint8_t help[]=
		"\r\n*-------------------------------*"
		"\r\n| Functions available           |"
		"\r\n*-------------------------------*\r\n"
		"help : print this message\r\n"
		"pinout : print all pins used\r\n"
		"powerOn : switch on the motor control driver\r\n"
		"powerOff : switch on the motor control driver\r\n";

const uint8_t pinout[]=
		"\r\n*-------------------------------*"
		"\r\n| Pinout used                   |"
		"\r\n*-------------------------------*\r\n"
		"PC13 : blue button\r\n"
		"PC3 : ISO_RESET\r\n"
		"PA2 : USART2_TX\r\n"
		"PA3 : USART2_RX\r\n"
		"PA5 : LED\r\n"
		"PA8 : TIM1_CH1\r\n"
		"PA9 : TIM1_CH2\r\n"
		"PA11 : TIM1_CH1N\r\n"
		"PA12 : TIM1_CH2N\r\n";
const uint8_t powerOn[]="Switch on motor control\r\n";
const uint8_t powerOff[]="Switch off motor control\r\n";
const uint8_t motorSpeedInst[]="Enter a motor speed such as \"set speed <int>\"\r\n";
const uint8_t cmdNotFound[]="Command not found\r\n";
const uint8_t alpha[]="rapport cyclique\r\n";
const uint8_t setspeed[]="modification de la vitesse\r\n";
const uint8_t mesure[]="mesure du courant\r\n";
const uint8_t showspeed[]="mesure de vitesse\r\n";
const uint8_t asserv[]="asservissement en vitesse lance\r\n";


char cmdBuffer[CMD_BUFFER_SIZE];
extern uint8_t 	uartRxBuffer[UART_RX_BUFFER_SIZE];
extern uint16_t ADC_Buffer[20];
extern int vitesse;
uint8_t	idxCmd;
char* argv[MAX_ARGS];
uint8_t	argc;
extern uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE];
extern uint8_t stringSize;
char chaine1[30];
char chaine2[30];
int sum=0;
int mesure_mean=0;
double mesure_voltage=0;
double Imoyen=0;
double courant_buffer[2];
extern flag_asserv;


/**
  * @brief  Send a stating message
  * @retval None
  */
void shellInit(void){
	HAL_UART_Transmit(&huart2, started, sizeof(started), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, prompt, sizeof(prompt), HAL_MAX_DELAY);
}

/**
  * @brief  Send the prompt
  * @retval None
  */
void shellPrompt(void){
	HAL_UART_Transmit(&huart2, prompt, sizeof(prompt), HAL_MAX_DELAY);
}

/**
  * @brief  Send the default message if the command is not found
  * @retval None
  */
void shellCmdNotFound(void){
	HAL_UART_Transmit(&huart2, cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
}

/**
  * @brief  Function called for saving the new character and call and setup argc and argv variable if ENTER is pressed
  * @retval 1 if a new command is available, 0 if not.
  */
uint8_t shellGetChar(void){
	uint8_t newCmdReady = 0;
	char* token;

	switch(uartRxBuffer[0]){
		// If Enter, update argc and argv
	case ASCII_CR:
		HAL_UART_Transmit(&huart2, newline, sizeof(newline), HAL_MAX_DELAY);
		cmdBuffer[idxCmd] = '\0';
		argc = 0;
		token = (char*)strtok(cmdBuffer, " ");
		while(token!=NULL){
			argv[argc++] = token;
			token = (char*)strtok(NULL, " ");
		}

		idxCmd = 0;
		newCmdReady = 1;
		break;
		// Delete last character if "return" is pressed
	case ASCII_BS:
		cmdBuffer[idxCmd--] = '\0';
		HAL_UART_Transmit(&huart2, uartRxBuffer, 1, HAL_MAX_DELAY);
		break;
		// Default state : add new character to the command buffer
	default:
		cmdBuffer[idxCmd++] = uartRxBuffer[0];
		HAL_UART_Transmit(&huart2, uartRxBuffer, 1, HAL_MAX_DELAY);
	}

	return newCmdReady;
}

/**
  * @brief  Call function depends of the value of argc and argv
  * @retval None
  */
void shellExec(void){
	int i=0;

	if(strcmp(argv[0],"set")==0){
		if(strcmp(argv[1],"PA5")==0 && ((strcmp(argv[2],"0")==0)||(strcmp(argv[2],"1")==0)) ){
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, atoi(argv[2]));
			stringSize = snprintf((char*)uartTxBuffer,UART_TX_BUFFER_SIZE,"Switch on/off led : %d\r\n",atoi(argv[2]));
			HAL_UART_Transmit(&huart2, uartTxBuffer, stringSize, HAL_MAX_DELAY);
		}
		else if(strcmp(argv[1],"speed")==0){
			if(atoi(argv[2])==0 && strcmp(argv[2],"0")!=0){
				HAL_UART_Transmit(&huart2, motorSpeedInst, sizeof(motorSpeedInst), HAL_MAX_DELAY);
			}
			else{
				motorSetSpeed(atoi(argv[2]));
			}
		}
		else{
			shellCmdNotFound();
		}
	}
	else if(strcmp(argv[0],"help")==0)
	{
		HAL_UART_Transmit(&huart2, help, sizeof(help), HAL_MAX_DELAY);
	}
	else if(strcmp(argv[0],"pinout")==0)
	{
		HAL_UART_Transmit(&huart2, pinout, sizeof(pinout), HAL_MAX_DELAY);
	}
	else if((strcmp(argv[0],"power")==0)&&(strcmp(argv[1],"on")==0))
	{
		HAL_UART_Transmit(&huart2, powerOn, sizeof(powerOn), HAL_MAX_DELAY);
		motorPowerOn();
	}
	else if((strcmp(argv[0],"power")==0)&&(strcmp(argv[1],"off")==0))
	{
		HAL_UART_Transmit(&huart2, powerOff, sizeof(powerOff), HAL_MAX_DELAY);
		motorPowerOff();
	}
	else if((strcmp(argv[0],"alpha")==0))
	{
		HAL_UART_Transmit(&huart2, alpha, sizeof(alpha), HAL_MAX_DELAY);
		set_alpha(atoi(argv[1]));
	}
	else if ((strcmp(argv[0],"set")==0)&&(strcmp(argv[1],"speed")==0))
	{
		HAL_UART_Transmit(&huart2, setspeed, sizeof(setspeed), HAL_MAX_DELAY);
		motorSetSpeed(atoi (argv[2]));
	}
	else if ((strcmp(argv[0],"mesure")==0))
	{
		for(i=0;i<20;i++)
		{
			sum=sum+ADC_Buffer[i];
		}
		mesure_mean=sum/10;
		i=0;
		courant_buffer[0]=courant_buffer[1];
		mesure_voltage=((double)mesure_mean*3.3)/4096.0;
		Imoyen=(mesure_voltage-2.5)*12;
		courant_buffer[1]=Imoyen;
		sprintf(chaine1,"le courant vaut %f A \r\n",Imoyen);
		HAL_UART_Transmit(&huart2, chaine1,strlen(chaine1),HAL_MAX_DELAY);
		sum=0;
	}
	else if (strcmp(argv[0],"showspeed")==0)
		{
			sprintf(chaine2,"la vitesse est de %d tr.min \r\n",vitesse);
			HAL_UART_Transmit(&huart2,chaine2 ,strlen(chaine2), HAL_MAX_DELAY);

		}
	else if (strcmp(argv[0],"asserv")==0)
			{
				sprintf(chaine2,"asservissement lance à la vitesse \r\n");
				HAL_UART_Transmit(&huart2,chaine2 ,strlen(chaine2), HAL_MAX_DELAY);
				if(flag_asserv==0){
					flag_asserv=1;
				}
				else{
					flag_asserv=0;
				}
			}
	else{
		shellCmdNotFound();
	}
}

