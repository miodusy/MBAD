/*
 * Radio_433.c
 *
 *  Created on: 09.06.2018
 *      Author: miodu_uomeei1
 */

#include "Radio_433.h"
#include "gpio.h"

volatile int flaga_nadawania = 0;
volatile int flaga_odbierania = 1;

volatile int odebrano[2] = { 0, 0 };
volatile int nadaj[2] = { 45, 156 };

volatile int nr_bitu = 0;
volatile int kwant_bitu = 0;
volatile int tlo_nadawania = 0;
volatile int tryb_odbierania = 1;

volatile int czasy_start[4] = { 0, 0, 0, 0 };
volatile int bit[2] = { 0, 0 };
volatile int tik = 0;
volatile int tik50_nadawanie = 0;
volatile int koniec = 0;

volatile int nr_bitu_odbieranie = 0;
volatile int kwant_bitu_odbieranie = 0;

void podstawa_czasowa_kominikacji() {
	tik++;
	tik50_nadawanie++;
	obsluga_odbierania_1();

	//HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
	if (tik50_nadawanie == 50) {
		tik50_nadawanie = 0;
		obsluga_nadawania();
	}
	if (tik >= czasy_start[3] && czasy_start[3] != 0) {
		kwant_bitu_odbieranie++;

		if (kwant_bitu_odbieranie == 10) {
			nr_bitu_odbieranie++;
			kwant_bitu_odbieranie = 0;
		}
		tik = 0;
		obsluga_odbierania_2();

	}
}

void obsluga_nadawania() {
	if (nr_bitu >= 51 && MASTER) {
		nr_bitu = 0;
		//	kwant_bitu = 0;
	}

	if (kwant_bitu == 10) {
		//HAL_GPIO_TogglePin(PORT_Write, PIN_Write);
		nr_bitu++;
		kwant_bitu = 0;
	}

	if (flaga_nadawania == 1 ) {

		if (nr_bitu < 3) {
			if (kwant_bitu == 0) {
				HAL_GPIO_WritePin(PORT_Write, PIN_Write, 1);
			}
			if (kwant_bitu == 5) {
				HAL_GPIO_WritePin(PORT_Write, PIN_Write, 0);
			}
		}
		if (nr_bitu >= 3 && nr_bitu < 11) {
			if (kwant_bitu == 0) {
				if ((nadaj[0] & (1 << (nr_bitu - 3)))) {
					HAL_GPIO_WritePin(PORT_Write, PIN_Write, 1);
				} else {
					HAL_GPIO_WritePin(PORT_Write, PIN_Write, 0);
				}
			}
			if (kwant_bitu == 5) {
				HAL_GPIO_TogglePin(PORT_Write, PIN_Write);
			}
		}

		if (nr_bitu >= 11 && nr_bitu < 19) {
			if (kwant_bitu == 0) {
				if ((nadaj[1] & (1 << (nr_bitu - 11)))) {
					HAL_GPIO_WritePin(PORT_Write, PIN_Write, 1);
				} else {
					HAL_GPIO_WritePin(PORT_Write, PIN_Write, 0);
				}
			}
			if (kwant_bitu == 5) {
				HAL_GPIO_TogglePin(PORT_Write, PIN_Write);
			}
		}

		if (nr_bitu == 19) {
			HAL_GPIO_WritePin(PORT_Write, PIN_Write, 0);
		}
		if (nr_bitu >= 19 && MASTER == 0) {
			flaga_nadawania = 0;
			nr_bitu = 0;
		}
		if (nr_bitu >= 19 && nr_bitu < 50) { //Czekanie na odpowiedz
			HAL_GPIO_WritePin(PORT_Write, PIN_Write, 0);
		}
		if (nr_bitu > 50) {
			flaga_nadawania = 0;
			nr_bitu = 0;
		}
	}

	kwant_bitu++;

	if (flaga_nadawania == 0 && MASTER) { //T£O
		if (kwant_bitu > 9) {
			tlo_nadawania++;
		}
		if (tlo_nadawania == 2) {
			HAL_GPIO_WritePin(PORT_Write, PIN_Write, 0);
		}

		if (tlo_nadawania == 0) {
			HAL_GPIO_WritePin(PORT_Write, PIN_Write, 1);
		}
		if (tlo_nadawania > 3) {
			tlo_nadawania = 0;
		}

	}
	if (flaga_nadawania == 2) { //ODBIERANIE
		HAL_GPIO_WritePin(PORT_Write, PIN_Write, 0);
	}

}

void obsluga_odbierania_1() {

	if (flaga_odbierania == 1 && nr_bitu_odbieranie <= 2) {
		//HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

		if (tryb_odbierania == 0) {
			//printf("d0: %d nr_n:%d  kw_n:%d nr_o: %d kw_o:%d \r\n", tik,nr_bitu,kwant_bitu, nr_bitu_odbieranie,kwant_bitu_odbieranie);

			if (HAL_GPIO_ReadPin(PORT_Read, PIN_Read) == 0) { //stan 0
				tryb_odbierania = 1;
				czasy_start[3] = 0;
			}
		} else if (tryb_odbierania == 1) { //1do 7 bity startu
			tik = 0;
			if (HAL_GPIO_ReadPin(PORT_Read, PIN_Read) == 1) { // ppierwsze zbocze narastaj¹ce
				tryb_odbierania = 2;
			}
		} else if (tryb_odbierania == 2) {
			if ((HAL_GPIO_ReadPin(PORT_Read, PIN_Read) == 0)) { //kontrola dlugosci pierwszego stanu wysokiego

				if (tik > 25 * 11) {
					tryb_odbierania = 0;
					//HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

				} else if (tik < 25 * 9) {
					tryb_odbierania = 0;
					//HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

				} else {
					czasy_start[0] = tik;
					tryb_odbierania = 3;
				}
			}
		} else if (tryb_odbierania == 3) {
			tik = 0;
			if (HAL_GPIO_ReadPin(PORT_Read, PIN_Read) == 1) {
				tryb_odbierania = 4;
			}
		} else if (tryb_odbierania == 4) {

			if ((HAL_GPIO_ReadPin(PORT_Read, PIN_Read) == 0)) {

				if (tik > 25 * 11) {
					tryb_odbierania = 0;
					//				HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

				} else if (tik < 25 * 9) {
					tryb_odbierania = 0;
//					HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

				} else {
					czasy_start[1] = tik;
					tryb_odbierania = 5;
				}
			}
		} else if (tryb_odbierania == 5) {
			tik = 0;
			if (HAL_GPIO_ReadPin(PORT_Read, PIN_Read) == 1) {
				tryb_odbierania = 6;
			}
		} else if (tryb_odbierania == 6) {
			if ((HAL_GPIO_ReadPin(PORT_Read, PIN_Read) == 0)) {
				if (tik > 25 * 11) {
					tryb_odbierania = 0;
					//HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

				} else if (tik < 25 * 9) {
					tryb_odbierania = 0;
					//HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

				} else {

					czasy_start[2] = tik;
					tryb_odbierania = 7;
				}
			}
		} else if (tryb_odbierania == 7) { //kontrola startu
			if ((czasy_start[0] < czasy_start[1] + czasy_start[0] / 10)
					&& (czasy_start[0] > czasy_start[1] - czasy_start[0] / 10)) {
				if ((czasy_start[1] < czasy_start[2] + czasy_start[1] / 10)
						&& (czasy_start[1]
								> czasy_start[2] - czasy_start[1] / 10)) {
					if ((czasy_start[0] < czasy_start[2] + czasy_start[0] / 10)
							&& (czasy_start[0]
									> czasy_start[2] - czasy_start[0] / 10)) {
						tryb_odbierania = 10;
						czasy_start[3] = (czasy_start[2]+czasy_start[1]+czasy_start[0]) / (3*5); //podstawa czasowa odbierania
						nr_bitu_odbieranie = 2;
						kwant_bitu_odbieranie = 5;
						//printf("d0: %d \r\n", czasy_start[0]);
						//						printf("d1: %d \r\n", czasy_start[1]);
						//						printf("d2: %d \r\n", czasy_start[2]);
						//printf("d0: %d nr_n:%d  kw_n:%d nr_o: %d kw_o:%d \r\n", czasy_start[3],nr_bitu,kwant_bitu, nr_bitu_odbieranie,kwant_bitu_odbieranie);

						tik = 0;
						// printf("Podstwa czasowa: %d\r\n", czasy_start[3]);

					} else {
						printf("Blond Startu \r\n");
						printf("d0: %d \r\n", czasy_start[0]);
						printf("d1: %d \r\n", czasy_start[1]);
						printf("d2: %d \r\n", czasy_start[2]);

						tryb_odbierania = 0;
					}
				} else {
					printf("Blond Startu \r\n");
					printf("d0: %d \r\n", czasy_start[0]);
					printf("d1: %d \r\n", czasy_start[1]);
					printf("d2: %d \r\n", czasy_start[2]);
					tryb_odbierania = 0;

				}
			} else {
				printf("Blond Startu \r\n");
				printf("d0: %d \r\n", czasy_start[0]);
				printf("d1: %d \r\n", czasy_start[1]);
				printf("d2: %d \r\n", czasy_start[2]);
				tryb_odbierania = 0;
			}
		} else if (tryb_odbierania == 10) { //dekodowanie  danych

		}

	}
}
void obsluga_odbierania_2() {
	if (nr_bitu_odbieranie >= 3 && nr_bitu_odbieranie < 11) {
		if (kwant_bitu_odbieranie == 2) {
			//printf("k_o: %d, k_w:%d, b_o:%d, b_n:%d\r\n", kwant_bitu_odbieranie,
			//		kwant_bitu, nr_bitu_odbieranie, nr_bitu);

			if (HAL_GPIO_ReadPin(PORT_Read, PIN_Read) == 1) {
				bit[0] = 1;
			} else {
				bit[0] = 0;
			}
		}
		if (kwant_bitu_odbieranie == 7) {
			if (HAL_GPIO_ReadPin(PORT_Read, PIN_Read) == 1) {
				bit[1] = 1;
			} else {
				bit[1] = 0;
			}
		}
		if (bit[0] == 1 && bit[1] == 0 && kwant_bitu_odbieranie == 9) {
			odebrano[0] = odebrano[0] + (1 << (nr_bitu_odbieranie - 3));
		}
		if (kwant_bitu_odbieranie == 9) {

			//printf("1: %d, 2: %d \r\n", bit[0], bit[1]);
			bit[0] = 0;
			bit[1] = 0;
		}
	}

	if (nr_bitu_odbieranie >= 11 && nr_bitu_odbieranie <= 18) {
		if (kwant_bitu_odbieranie == 2) {
			//printf("k_o: %d, k_w:%d, b_o:%d, b_n:%d\r\n", kwant_bitu_odbieranie,
			//kwant_bitu, nr_bitu_odbieranie, nr_bitu);
			if (HAL_GPIO_ReadPin(PORT_Read, PIN_Read) == 1) {
				bit[0] = 1;
			} else {
				bit[0] = 0;
			}
		}
		if (kwant_bitu_odbieranie == 7) {
			if (HAL_GPIO_ReadPin(PORT_Read, PIN_Read) == 1) {
				bit[1] = 1;
			} else {
				bit[1] = 0;
			}
		}
		if (bit[0] == 1 && bit[1] == 0 && kwant_bitu_odbieranie == 9) {
			odebrano[1] = odebrano[1] + (1 << (nr_bitu_odbieranie - 11));
		}
		if (kwant_bitu_odbieranie == 9) {

			//printf("2: %d, 3: %d \r\n", bit[0], bit[1]);
			bit[0] = 0;
			bit[1] = 0;
		}
	}
	if (nr_bitu_odbieranie == 19 && kwant_bitu_odbieranie == 9) {
		//printf("o: %d, %d \r\n", odebrano[0], odebrano[1]);
		//odebrano[0] = 0;
		//odebrano[1] = 0;

		//tryb_odbierania = 0;
		flaga_odbierania = 2;
	}
	if (nr_bitu_odbieranie >= 20) {
		nr_bitu_odbieranie = 0;
		tryb_odbierania = 0;
	}

	if (nr_bitu_odbieranie >= 3 && nr_bitu_odbieranie < 11) {
		if (kwant_bitu_odbieranie == 0) {
			if ((nadaj[0] & (1 << (nr_bitu_odbieranie - 3)))) {
				//HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 1);
			} else {
				//HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, 0);
			}
		}
		if (kwant_bitu_odbieranie == 5) {
			//HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
		}
	}
}

void nadaj_433(int adres, int dana) {
	flaga_nadawania = 1;
	nr_bitu = 0;
	kwant_bitu = 0;
	nadaj[0] = adres;
	nadaj[1] = dana;
}
