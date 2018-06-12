/*
 * swiatlo.h
 *
 *  Created on: 03.05.2018
 *      Author: miodu_uomeei1
 */

#ifndef SWIATLO_H_
#define SWIATLO_H_

#define REG_DETEKCJI 90

volatile int flaga_zmiana_jasnosci;
volatile int wartosc_docelowa_jasnosci;
volatile float szybkosc_zmiany_jasnsci;

volatile float swiatlo;
volatile int flag_clk230V;
volatile int zegar_swiatlo;

void obsluga_jasnosci();

void zapal_swiatlo(float szybkosc);
void zgas_swiatlo(float szybkosc);
void zmien_jasnosc(float szybkosc, int doscelowa_jasnosc);

#endif /* SWIATLO_H_ */
