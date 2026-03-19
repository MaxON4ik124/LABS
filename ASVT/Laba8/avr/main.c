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
        /*
         * The parser works in the foreground, UART moves bytes in the background.
         * Idle sleep reduces pointless busy-waiting and is a nice optimization for the lab.
         */
        protocol_poll();
        sleep_mode();
    }

    return 0;
}
