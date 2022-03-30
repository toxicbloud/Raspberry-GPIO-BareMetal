/**
 * @file gpio.c
 * @author Antonin rousseau (antoninrousseau55000@gmail.com)
 * @brief pour le cours de raspberry pi
 * @version 0.1
 * @date 2022-03-30
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef GPIO
#define GPIO

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>

int mmap_fd = 0;

#define GPIO_BASE (0x3F200000) // adresse physique pour un PI 3
#define BLOCK_SIZE (4096)

unsigned *PI_IOmmap(off_t where)
{
	void *map;
	if (mmap_fd <= 0)
	{
		if ((mmap_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
			printf("Echec à l'ouverture de /dev/mem");
	}

	// copié du poly
	// projette les registres GPIO dans la mémoire de notre programme
	map = mmap(
		NULL,					// projette où ça arrange le kernel
		BLOCK_SIZE,				// les registres de contrôle tiennent dans une seule page
		PROT_READ | PROT_WRITE, // on veut lire et écrire
		MAP_SHARED,				// partagé avec d'autres processus
		mmap_fd,				// la mémoire
		where					// adresse de la zone à accéder
	);

	if (map == MAP_FAILED)
		printf("Echec de mmap()");

	return (unsigned *)map;
}

volatile unsigned *PI_gpio = NULL;
// Pour mettre des broches à 1 ou à 0 :
#define RPI_GPSET0 (7)
#define RPI_GPCLR0 (10)
#define RPI_GPLEV0 (13)

#define BCM_GPIO_IN (0)
#define BCM_GPIO_OUT (1)

/**
 * @brief initialise le pin GPIO d'indice port en mode mode
 * @param port le numéro du GPIO
 * @param mode le mode du GPIO
 */
void PI_GPIO_config(int port, int mode)
{
	int registre, offset, temp;

	//  singleton dédicace PAG
	if (PI_gpio == NULL)
		PI_gpio = PI_IOmmap(GPIO_BASE);

	if ((port >= 0) && (port < 32)
		&& (mode >= 0) && (mode < 8))
	{
		// Calcul du numéro de registre :
		registre = port / 10;

		// lecture dudit registre :
		temp = *(PI_gpio + registre);

		// calcul de l'offset :
		offset = port - (registre * 10); // offset = port % 10
		offset *= 3;					 // 3 bits par port

		// Effacer les bits précédents :
		temp &= ~(7 << offset);
		// ajouter le mode désiré :
		temp |= mode << offset; // temp = temp | (mode << offset);

		// réécriture du résultat :
		*(PI_gpio + registre) = temp;
	}
	else
	{
		fprintf(stderr, "Mauvais  port (%d) ou mode (%d)\n", port, mode);
		exit(EXIT_FAILURE);
	}
}
/**
 * @brief met le pin à 0
 * @param port le numéro du GPIO
 */
void gpio_clr_n(int n){
	*(PI_gpio + RPI_GPCLR0) = (1 << n);
	// ou PI_gpio[RPI_GPCLR0] = (1 << n);
}
/**
 * @brief met le pin à 1
 * @param n le numéro du pin GPIO
 */
void gpio_set_n(int n){
	*(PI_gpio + RPI_GPSET0) = (1 << n);
	// ou PI_gpio[RPI_GPSET0] = (1 << n);
}
/**
 * @brief donne la valeur du pin
 * @param n le numéro du pin GPIO
 * @return 1 si le pin est à 1, 0 sinon
 */
int gpio_lvl_n(int n){
	return ((*(PI_gpio + RPI_GPLEV0) >> n) & 1);
}

#endif
