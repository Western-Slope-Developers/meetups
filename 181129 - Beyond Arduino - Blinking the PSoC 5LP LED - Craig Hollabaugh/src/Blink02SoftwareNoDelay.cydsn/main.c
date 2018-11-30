#include "project.h"

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    uint32_t timeout, counter;
    timeout = 1000000;
    for(;;)
    {
        if (counter++ > timeout) {
            LED_Write(!LED_Read());
            counter = 0;
        }
    }
}

