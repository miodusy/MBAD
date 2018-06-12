
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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
int steps_count=0; //licznik krokow
int counter = 0;
int angle=0;
int aState;
int aLastState;
int flag_kontaktron=0, flag_0=0, flag_1=0, flag_2=0, flag_3=0; //do obslugi przerwan
float degree=0.0;
const float motor_angle=1.8;
const float step_size=1.0; //{1.0, 0.5, 0.25, 0.125, 0.0625}
/*step_size - zadawany krok
 * pe³ny krok, czyli 1 -> 200 kroków na obrót
 * 1/2 kroku, czyli 0.5 -> 400
 * 1/4 kroku, czyli 0.25 -> 800
 * 1/8 kroku, czyli 0.125 -> 1600
 * 1/16 kroku, czyli 0.0625 -> 3200
 * PRZY ZMIANIE KROKU PODLACZYC NA PLYTCE ODPOWIEDNIE PINY DO +5V
 */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
//UART wyswietlanie wiadomosci
int _write(int file, uint8_t *ptr, int len){
	HAL_UART_Transmit(&huart2, ptr, len, 100);
	return len;
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){ //przerwania, aby pobieralo dane o kontaktronie
										//tylko gdy jest odlaczany/podlaczany
	if(GPIO_Pin==KONTAKTRON_Pin){
		flag_kontaktron=1;
		if(HAL_GPIO_ReadPin(KONTAKTRON_GPIO_Port, KONTAKTRON_Pin)==GPIO_PIN_RESET)//zamkniete
		  printf("Okno zamkniete\r\n");
		else
		  printf("Okno otwarte\r\n");
	}
	/*if(GPIO_Pin==JOY_UP_Pin){
		flag_0=1;
		printf("JOY_UP wcisniety\r\n");
		//if(HAL_GPIO_ReadPin(JOY_UP_GPIO_Port, JOY_UP_Pin)==GPIO_PIN_SET)
		//	pozycja0();
	}
	if(GPIO_Pin==JOY_LEFT_Pin)
		flag_1=1;
	if(GPIO_Pin==JOY_RIGHT_Pin)
		flag_2=1;
	if(GPIO_Pin==JOY_DOWN_Pin)
		flag_3=1;*/
}
//funkcja realizujaca zadana ilosc obrotow z zadana predkoscia
void stepper_rotate(int steps_amount, float rpm){
	//ilosc krokow na obrot
	float steps_per_rotation;//=((360.00/motor_angle)/step_size);
	//calkowita ilsoc krokow w zadanej ilosci rotacji
	float total_steps;//=rotation*steps_per_rotation;
	//czas trwania jednego obrotu [w us]
	unsigned long step_period_micro_sec;//=((60.0000/(rpm*steps_per_rotation))*(1E6/2.0000));

	if(steps_amount>0){ //krec w prawo
	  HAL_GPIO_WritePin(DIR_PIN_GPIO_Port, DIR_PIN_Pin, GPIO_PIN_SET);

	  steps_per_rotation=((360.00/motor_angle)/step_size);
	  total_steps=steps_amount;//rotation*steps_per_rotation;
	  step_period_micro_sec=((60.0000/(rpm*steps_per_rotation))*(1E6/2.0000));

	  for(unsigned long i=0; i<total_steps; i++){
		HAL_GPIO_WritePin(STEP_PIN_GPIO_Port, STEP_PIN_Pin, GPIO_PIN_SET);
		HAL_Delay(step_period_micro_sec/1000);
		HAL_GPIO_WritePin(STEP_PIN_GPIO_Port, STEP_PIN_Pin, GPIO_PIN_RESET);
		HAL_Delay(step_period_micro_sec/1000);

		steps_count++;
	  }
	  printf("Liczba krokow: %d\r\n", steps_count);
	}
	if(steps_amount<0){ //krec w lewo
	  HAL_GPIO_WritePin(DIR_PIN_GPIO_Port, DIR_PIN_Pin, GPIO_PIN_RESET);
	  steps_amount=steps_amount*(-1); //zamien z powrotem na dodatnia wartosc

	  steps_per_rotation=((360.00/motor_angle)/step_size);
	  total_steps=steps_amount;//rotation*steps_per_rotation;
	  step_period_micro_sec=((60.0000/(rpm*steps_per_rotation))*(1E6/2.0000));

	  for(unsigned long i=0; i<total_steps; i++){
		HAL_GPIO_WritePin(STEP_PIN_GPIO_Port, STEP_PIN_Pin, GPIO_PIN_SET);
		HAL_Delay(step_period_micro_sec/1000);
		HAL_GPIO_WritePin(STEP_PIN_GPIO_Port, STEP_PIN_Pin, GPIO_PIN_RESET);
		HAL_Delay(step_period_micro_sec/1000);

		steps_count--;
	  }
	  printf("Liczba krokow: %d\r\n", steps_count);
	}
}
void pozycja0(){
	if(HAL_GPIO_ReadPin(KONTAKTRON_GPIO_Port, KONTAKTRON_Pin)==GPIO_PIN_RESET){//okno zamkniete
		if((steps_count>=0)&&(steps_count<=1200)) //miedzy poz 0-3
			stepper_rotate(-steps_count, 200);
		if((steps_count<0) || (steps_count>1200))
			printf("Kroki poza dopuszczalnym zakresem 0-1200\r\n");
	}
	else{}//printf("Okno otwarte - brak mozliwosci ruchu roleta\r\n");//caly czas wyswietla

}
void pozycja1(){
	if(HAL_GPIO_ReadPin(KONTAKTRON_GPIO_Port, KONTAKTRON_Pin)==GPIO_PIN_RESET){//okno zamkniete
		if((steps_count>=0)&&(steps_count<400)) //miedzy poz 0-1
			stepper_rotate(400-steps_count, 200);
		if((steps_count>=400)&&(steps_count<=1200)) //miedzy poz 1-3
			stepper_rotate(-(steps_count-400), 200);
		if((steps_count<0) || (steps_count>1200))
			printf("Kroki poza dopuszczalnym zakresem 0-1200\r\n");
	}
	else{}//printf("Okno otwarte - brak mozliwosci ruchu roleta\r\n");//caly czas wyswietla
}
void pozycja2(){
	if(HAL_GPIO_ReadPin(KONTAKTRON_GPIO_Port, KONTAKTRON_Pin)==GPIO_PIN_RESET){//okno zamkniete
		if((steps_count>=0)&&(steps_count<800)) //miedzy poz 0-2
			stepper_rotate(800-steps_count, 200);
		if((steps_count>=800)&&(steps_count<=1200)) //miedzy poz 2-3
			stepper_rotate(-(steps_count-800), 200);
		if((steps_count<0) || (steps_count>1200))
			printf("Kroki poza dopuszczalnym zakresem 0-1200\r\n");
	}
	else{}//printf("Okno otwarte - brak mozliwosci ruchu roleta\r\n");//caly czas wyswietla
}
void pozycja3(){
	if(HAL_GPIO_ReadPin(KONTAKTRON_GPIO_Port, KONTAKTRON_Pin)==GPIO_PIN_RESET){//okno zamkniete
		if((steps_count>=0)&&(steps_count<=1200)) //miedzy poz 0-3
			stepper_rotate(1200-steps_count, 200);
		if((steps_count<0) || (steps_count>1200))
			printf("Kroki poza dopuszczalnym zakresem 0-1200\r\n");
	}
	else{}//printf("Okno otwarte - brak mozliwosci ruchu roleta\r\n");//caly czas wyswietla
}
void rotateCW() {
	if((steps_count<0)||(steps_count>1200))
		printf("Kroki poza dopuszczalnym zakresem 0-1200\r\n");
	else{
		HAL_GPIO_WritePin(DIR_PIN_GPIO_Port, DIR_PIN_Pin, GPIO_PIN_RESET);
	    HAL_GPIO_WritePin(STEP_PIN_GPIO_Port, STEP_PIN_Pin, GPIO_PIN_SET);
	    HAL_Delay(2);
	    HAL_GPIO_WritePin(STEP_PIN_GPIO_Port, STEP_PIN_Pin, GPIO_PIN_RESET);
	    HAL_Delay(2);
	}
}
void rotateCCW() {
	if((steps_count<0)||(steps_count>1200))
		printf("Kroki poza dopuszczalnym zakresem 0-1200\r\n");
	else{
		HAL_GPIO_WritePin(DIR_PIN_GPIO_Port, DIR_PIN_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(STEP_PIN_GPIO_Port, STEP_PIN_Pin, GPIO_PIN_SET);
		HAL_Delay(2);
		HAL_GPIO_WritePin(STEP_PIN_GPIO_Port, STEP_PIN_Pin, GPIO_PIN_RESET);
		HAL_Delay(2);
	}
}
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	/*opcja 2*/
	//rozwin_rolete();
	/*koniec opcji 2*/
	//odczytanie inicjalizujacej wartosci pinu CLK
	aLastState=HAL_GPIO_ReadPin(CLK_PIN_GPIO_Port, CLK_PIN_Pin);
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  printf("START\r\n");
  	  //pierwsze sprawdzenie, jednorazowe przed wejsciem do while, jaki jest stan okna
	  //napisanie tego w innych komentarzach nie dziala
  	  if(HAL_GPIO_ReadPin(KONTAKTRON_GPIO_Port, KONTAKTRON_Pin)==GPIO_PIN_RESET){ //zamkniete
		  HAL_GPIO_WritePin(LD_G_GPIO_Port, LD_G_Pin, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(LD_R_GPIO_Port, LD_R_Pin, GPIO_PIN_RESET);
		  printf("Status okna po wlaczeniu zasilania - zamkniete\r\n");
	  }
	  else{
		  HAL_GPIO_WritePin(LD_G_GPIO_Port, LD_G_Pin, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(LD_R_GPIO_Port, LD_R_Pin, GPIO_PIN_SET);
		  printf("Status okna po wlaczeniu zasilania - otwarte\r\n");
	  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //sprawdzanie kontaktronu - czy okno zamkniete
	  if(flag_kontaktron==1){
		  if(HAL_GPIO_ReadPin(KONTAKTRON_GPIO_Port, KONTAKTRON_Pin)==GPIO_PIN_RESET){ //zamkniete
			  HAL_GPIO_WritePin(LD_G_GPIO_Port, LD_G_Pin, GPIO_PIN_SET);
			  HAL_GPIO_WritePin(LD_R_GPIO_Port, LD_R_Pin, GPIO_PIN_RESET);
			 //printf("Okno zamkniete\r\n"); //caly czas wypisywalo
		  }
		  else{
			  HAL_GPIO_WritePin(LD_G_GPIO_Port, LD_G_Pin, GPIO_PIN_RESET);
			  HAL_GPIO_WritePin(LD_R_GPIO_Port, LD_R_Pin, GPIO_PIN_SET);
			 //printf("Okno otwarte\r\n"); //caly czas wypisywalo
		  }
	  }
	/*opcja 1*/
	//stepper_rotate(1, 100);
	//HAL_Delay(500);
	//stepper_rotate(-1200, 100);
	//HAL_Delay(1000);
	/*koniec opcji 1*/

	/*opcja 2*/
	//rozwin_rolete();
	/*koniec opcji 2*/

	/*opcja 3*/
	if(HAL_GPIO_ReadPin(JOY_DOWN_GPIO_Port, JOY_DOWN_Pin)==GPIO_PIN_SET)
		pozycja3();
	if(HAL_GPIO_ReadPin(JOY_UP_GPIO_Port, JOY_UP_Pin)==GPIO_PIN_SET)
		pozycja0();
	if(HAL_GPIO_ReadPin(JOY_LEFT_GPIO_Port, JOY_LEFT_Pin)==GPIO_PIN_SET)
		pozycja1();
	if(HAL_GPIO_ReadPin(JOY_RIGHT_GPIO_Port, JOY_RIGHT_Pin)==GPIO_PIN_SET)
		pozycja2();

	if(HAL_GPIO_ReadPin(KONTAKTRON_GPIO_Port, KONTAKTRON_Pin)==GPIO_PIN_RESET){//okno zamkniete
		//PORUSZANIE ENKODEREM I SILNIKIEM
		//odczytanie biezacego stanu pinu CLK
		aState=HAL_GPIO_ReadPin(CLK_PIN_GPIO_Port, CLK_PIN_Pin);
		// If the previous and the current state of the outputCLK are different,
		//that means a Pulse has occured
		if(aState != aLastState){
		// If the outputDT state is different to the outputCLK state,
		//that means the encoder is rotating clockwise
			if(HAL_GPIO_ReadPin(DT_PIN_GPIO_Port, DT_PIN_Pin) != aState){
		 //za jednym pokreceniem wykonuje sie 5razy wiecej krokow
				for(int i=0; i<5; i++){
					steps_count--;
					rotateCW();
				}
			}
			else{ //that means the encoder is rotating counterclockwise
				for(int i=0; i<5; i++){
					steps_count++;
					rotateCCW();
				}
			}
		printf("Liczba krokow: %d\r\n", steps_count);
		}
		aLastState = aState; // Updates the previous state of the outputCLK with the current state
	}
	else{}//printf("Okno otwarte - brak mozliwosci ruchu roleta\r\n");//caly czas wyswietla
	/*koniec opcji 3*/
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
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the main internal regulator output voltage 
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

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
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
