/*
 * Radio_433.h
 *
 *  Created on: 09.06.2018
 *      Author: miodu_uomeei1
 */

#ifndef RADIO_433_H_
#define RADIO_433_H_

#define MASTER 0 //1 gdy to baza

#define PIN_Write LD_RR_Pin
#define PORT_Write LD_RR_GPIO_Port

#define PIN_Read ODBIORNIK_RF_Pin
#define PORT_Read ODBIORNIK_RF_GPIO_Port


void obsluga_nadawania();
void obsluga_odbierania_1();
void obsluga_odbierania_2();
void podstawa_czasowa_kominikacji();

void nadaj_433(int adres, int dna);


volatile int flaga_nadawania;
volatile int flaga_odbierania;

volatile int odebrano[2];
volatile int nadaj[2];

volatile int nr_bitu;
volatile int kwant_bitu;
volatile int tlo_nadawania;
volatile int tryb_odbierania;

volatile int czasy_start[4];
volatile int bit[2];
volatile int tik;
volatile int tik500_nadawanie;
volatile int koniec;

volatile int nr_bitu_odbieranie;
volatile int kwant_bitu_odbieranie;



#endif /* RADIO_433_H_ */
