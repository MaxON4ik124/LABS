#include "uart.h"
#include "protocol.h"
#include "eeprom_store.h"

#include <avr/interrupt.h>
#include <avr/sleep.h>

int main(void)
{
    store_init();
    uart_init();
    protocol_init();

    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();

    for (;;)
    {
        protocol_poll();
        sleep_mode();
    }

    return 0;
}
