/*
 * swiatlo.c
 *
 *  Created on: 03.05.2018
 *      Author: miodu_uomeei1
 */
#include "swiatlo.h"
#include "gpio.h"

volatile float swiatlo = 0;
volatile int flag_clk230V = 0;
volatile int zegar_swiatlo = 0;

volatile int flaga_zmiana_jasnosci = 0;
volatile int wartosc_docelowa_jasnosci = 0;
volatile float szybkosc_zmiany_jasnsci = 10;

void zgas_swiatlo(float szybkosc) {
	flaga_zmiana_jasnosci = 2;
	szybkosc_zmiany_jasnsci = szybkosc;
}
void zapal_swiatlo(float szybkosc) {
	flaga_zmiana_jasnosci = 1;
	szybkosc_zmiany_jasnsci = szybkosc;
}
void zmien_jasnosc(float szybkosc, int doscelowa_jasnosc) {
	flaga_zmiana_jasnosci = 3;
	szybkosc_zmiany_jasnsci = szybkosc;
	wartosc_docelowa_jasnosci = doscelowa_jasnosc;

}

void obsluga_jasnosci() {
	if (flag_clk230V == 1) {
		flag_clk230V = 0;
		//printf("zegar:%d\n\r",zegar_swiatlo);

		zegar_swiatlo = REG_DETEKCJI;

	}
	if (swiatlo == 0) {
		HAL_GPIO_WritePin(Zarowka_GPIO_Port, Zarowka_Pin, 0);
		HAL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin);

	} else if (swiatlo > 90) {
		HAL_GPIO_WritePin(Zarowka_GPIO_Port, Zarowka_Pin, 1);
	} else {
		if (zegar_swiatlo >= swiatlo && zegar_swiatlo <= swiatlo + 2) { //zegar_swiatlo< swiatlo
			HAL_GPIO_WritePin(Zarowka_GPIO_Port, Zarowka_Pin, 1);
		} else {
			HAL_GPIO_WritePin(Zarowka_GPIO_Port, Zarowka_Pin, 0);
		}
	}
	zegar_swiatlo--;

	if (flaga_zmiana_jasnosci == 1) {
		if (swiatlo < 90) {
			swiatlo = swiatlo + szybkosc_zmiany_jasnsci / 10000;

		} else {
			flaga_zmiana_jasnosci = 0;
		}
	} else if (flaga_zmiana_jasnosci == 2) {
		if (swiatlo > 1) {
			swiatlo -= szybkosc_zmiany_jasnsci / 10000;
		} else {
			flaga_zmiana_jasnosci = 0;
		}

	} else if (flaga_zmiana_jasnosci == 3) {
		if (swiatlo < wartosc_docelowa_jasnosci) {
			swiatlo += szybkosc_zmiany_jasnsci / 10000;
		} else if (swiatlo > wartosc_docelowa_jasnosci) {
			swiatlo -= szybkosc_zmiany_jasnsci / 10000;
		}
		if (swiatlo > wartosc_docelowa_jasnosci - 1
				&& swiatlo < wartosc_docelowa_jasnosci + 1) {
			flaga_zmiana_jasnosci = 0;
		}
	}
}

