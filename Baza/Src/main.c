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
#include "stm32l4xx_hal.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "Radio_433.h"

#define ADRES_GNIAZDKO 48
#define Adres_GNIAZDKO_OBIERANIE 49
#define ADRES_ROLETA 68
#define ADRES_ROLETA_ODBIERANIE 69
#define ADRES_ZAROWKA 58
#define ADRES_ZAROWKA_ODBIERANIE 59

#define ADRES_EKSPANDER_GNIAZDO 60
#define ADRES_EKSPANDER_GNIAZDO_ODBIERANIE 160

#define ADRES_EKSPANDER_ROLETA 70

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

// BAZA USTAWIEN
volatile int zarowka = 100; //100 wylacz, 101 wlacz, x>102 --> ster jasnoscia
volatile int gniazdo = 100; //100 wylacz, 101 wlacz
volatile int roleta = 100;  //100 zamknij, 101 otwoz, x>102 --> ster wysokoscia

volatile int ekspander_gniazdko = 2;
volatile int ekspander_roleta = 0;

volatile int Adres_do_wyslania = 0;
volatile int Dana_do_wyslania = 0;
int tmp = 1;
int numer = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

int _write(int file, char*ptr, int len) {
	HAL_UART_Transmit(&huart2, ptr, len, 50);
	return len;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	if (GPIO_Pin == 1) {
		if (ekspander_gniazdko == 1) {
			ekspander_gniazdko = 2;
		} else if (ekspander_gniazdko == 2) {
			ekspander_gniazdko = 1;
		}
	}
	printf("Dioda ekspander gniazdko %d\r\n", ekspander_gniazdko);

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim7) {

	}
	if (htim == &htim6) {
		podstawa_czasowa_kominikacji();
		//obsluga_nadawania();
	}
	//if (htim == &htim17) {
	//}
}
void HAL_SYSTICK_Callback(void) {
	if (HAL_GPIO_ReadPin(Guzik1_GPIO_Port, Guzik1_Pin) == GPIO_PIN_SET) {
		if (zarowka != 0) {
			zarowka = 0;
			printf("Moc zarowki:%d\r\n", zarowka);
		}
	}
	if (HAL_GPIO_ReadPin(Guzik2_GPIO_Port, Guzik2_Pin) == GPIO_PIN_SET) {
		if (zarowka != 200) {
			zarowka = 200;
			printf("Moc zarowki:%d\r\n", zarowka);
		}
	}
	if (HAL_GPIO_ReadPin(Guzik3_GPIO_Port, Guzik3_Pin) == GPIO_PIN_SET) {
		if (gniazdo != 100) {
			gniazdo = 100;
			printf("Gniazdo wylaczone\r\n");
		}
	}
	if (HAL_GPIO_ReadPin(Guzik4_GPIO_Port, Guzik4_Pin) == GPIO_PIN_SET) {
		if (gniazdo != 101) {
			gniazdo = 101;
			printf("Gniazdo wlaczone\r\n");
		}
	}
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
	MX_DMA_Init();
	MX_USART2_UART_Init();
	MX_TIM7_Init();
	MX_TIM6_Init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim7);
	HAL_TIM_Base_Start_IT(&htim6);
	//HAL_TIM_Base_Start_IT(&htim17);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	int dana = 0;
	while (1) {
		tmp++;
		if (tmp == 4000) {
			tmp = 0;
			numer++;
		}
		if (numer >= 1800 && flaga_nadawania == 0 && tlo_nadawania == 3) {// tlo_nadawania==3
			numer = 0;
			if (dana == 5) {
				nadaj_433(ADRES_ROLETA, roleta);
				dana = 0;
			} else if (dana == 4) {
				nadaj_433(ADRES_GNIAZDKO, gniazdo);
			} else if (dana == 3) {
				nadaj_433(ADRES_ZAROWKA, zarowka);
			} else if (dana == 2) {
				nadaj_433(ADRES_EKSPANDER_ROLETA, ekspander_roleta);
			} else if (dana == 1) {
				nadaj_433(ADRES_EKSPANDER_GNIAZDO, ekspander_gniazdko);
				zarowka = zarowka + 10;

			}
			dana++;
		}
		if (flaga_odbierania == 2) {
			printf("odebrano: %d, %d \r\n", odebrano[0], odebrano[1]);
			if (odebrano[0] == ADRES_ZAROWKA_ODBIERANIE) {
				if (odebrano[1] != zarowka) {
					zarowka = odebrano[1];
				}
			}
			if (odebrano[0] == ADRES_ROLETA_ODBIERANIE) {
				if (odebrano[1] != roleta) {
					roleta = odebrano[1];
				}
			}
			if (odebrano[0] == Adres_GNIAZDKO_OBIERANIE) {
				if (odebrano[1] != gniazdo) {
					gniazdo = odebrano[1];
				}
			}
			if (odebrano[0] == ADRES_EKSPANDER_GNIAZDO_ODBIERANIE) {
				if (odebrano[1] != ekspander_gniazdko) {
					ekspander_gniazdko = odebrano[1];
				}
			}

			if (zarowka == 250) {
				zarowka = 100;
			}
			if (zarowka < 100) {
				zarowka = 100;
			}

			odebrano[0] = 0;
			odebrano[1] = 0;
			flaga_odbierania = 1;
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
void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 40;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
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
