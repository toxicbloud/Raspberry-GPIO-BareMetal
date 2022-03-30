/**
 * @file demo.c
 * @author Antonin Rousseau (antoninrousseau55000@gmail.com)
 * @brief pour le cours de nano pc controle du GPIO
 * @version 0.1
 * @date 2022-03-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "gpio.c"
#include <time.h>
#define PIN_BOUTON (23)
#define PIN_LED (24)
#define periode 0.5

int main(int argc, char *argv[])
{
    PI_GPIO_config(PIN_BOUTON, BCM_GPIO_IN);
    PI_GPIO_config(PIN_LED, BCM_GPIO_OUT);
    gpio_set_n(PIN_LED);

    int swap = 0;
    time_t lastime;
    time(&lastime);

    for (;;)
    {
        time_t now;
        time(&now);
        if (now - lastime > periode)
        {
            lastime = now;
            if (swap)
                gpio_set_n(PIN_LED);
            else
                gpio_clr_n(PIN_LED);
            swap = !swap;
        }
        if(gpio_lvl_n(PIN_BOUTON))
        {
            gpio_clr_n(PIN_LED);
            printf("Bouton appuyé on quitte :-)\n");
            exit(0);
        }
    }
}
