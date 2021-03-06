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
 * COPYRIGHT(c) 2018 STMicroelectronics
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
//#include "swiatlo.h"
#include "Radio_433.h"

#define ADRESS 0x40
#define IODIR 0x00
#define GPIO 0x09
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
volatile int Duty = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	//if (htim == &htim7) {
	//obsluga_jasnosci();

	//}
	if (htim == &htim17) {
		podstawa_czasowa_kominikacji();
		//obsluga_nadawania();
	}
	//if (htim == &htim17) {
	//}
}
void HAL_SYSTICK_Callback(void) {

}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 *
 * @retval None
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

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
	MX_I2C1_Init();
	MX_USART2_UART_Init();
	MX_TIM17_Init();
	/* USER CODE BEGIN 2 */
	//HAL_TIM_Base_Start_IT(&htim7);
	HAL_TIM_Base_Start_IT(&htim17);
	//HAL_TIM_Base_Start_IT(&htim17);

	//HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // Uruchamia generowanie PWM przez timer 4 na kanale 3
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	//ekspander
	uint8_t wyjscia = 0xFC; //1111 1100 - GP0 jako wyjscie, GP1-GP6 jako wejscie
	uint8_t wartosc = 0x01;  //0000 0001 GP0 stan HIGH reszta LOW

	HAL_I2C_Mem_Write(&hi2c1, ADRESS, IODIR, 1, &wyjscia, 1, 10);


	//swiatlo = 0;
	int tmp = 1;
	int dana = 0;
	int numer = 0;
	while (1) {
		if (flaga_odbierania == 2) {
			if (odebrano[0] == 48 && odebrano[1] == 100) {
				HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
				HAL_GPIO_WritePin(Gniazdo1_GPIO_Port, Gniazdo1_Pin, 0);
				for (dana = 0; dana < 100000; dana++) {
					HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
				}
				nadaj_433(200, 220);
			}
			if (odebrano[0] == 48 && odebrano[1] == 101) {
				HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, 1);
				HAL_GPIO_WritePin(Gniazdo1_GPIO_Port, Gniazdo1_Pin, 1);
				for (dana = 0; dana < 100000; dana++) {
					HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
				}
				nadaj_433(200, 221);
			}
			if (odebrano[0] == 48 && odebrano[1] == 102) {
				HAL_GPIO_WritePin(Gniazdo2_GPIO_Port, Gniazdo2_Pin, 0);
				for (dana = 0; dana < 100000; dana++) {
					HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
				}
				nadaj_433(200, 222);
			}
			if (odebrano[0] == 48 && odebrano[1] == 103) {
				HAL_GPIO_WritePin(Gniazdo2_GPIO_Port, Gniazdo2_Pin, 1);
				for (dana = 0; dana < 100000; dana++) {
					HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
				}
				nadaj_433(200, 223);
			}
			if (odebrano[0] == 60) {
				HAL_I2C_Mem_Write(&hi2c1, ADRESS, GPIO, 1, &odebrano[1], 1, 10);
				for (dana = 0; dana < 100000; dana++) {
					HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
				}
				nadaj_433(160, odebrano[1]);
			}
			odebrano[0] = 0;
			odebrano[1] = 0;
			flaga_odbierania = 1;
			tryb_odbierania = 0;
		}

		tmp++;
		if (tmp == 400000) {
			tmp = 0;
			numer++;
			//nadaj_433(122,212);
			//HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);

		}
		//flaga_nadawania = 2;
		/*
		 if (numer >= 1000 && tlo_nadawania == 3 && flaga_nadawania == 0) {
		 flaga_nadawania = 1;
		 numer = 0;
		 nr_bitu = 0;
		 kwant_bitu = 0;
		 nadaj[0] = nadaj[0] + 1;
		 nadaj[1] = nadaj[1] - 1;

		 //nr_bitu_odbieranie=0;
		 //HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

		 }*/

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

	}
	/* USER CODE END 3 */

}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  file: The file name as string.
 * @param  line: The line in file as a number.
 * @retval None
 */
void _Error_Handler(char *file, int line) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while (1) {
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
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
