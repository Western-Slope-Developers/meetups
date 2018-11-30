#include "project.h"

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    Comparator_Start();
    uint8_t status;
    for(;;)
    {
        status = Status_Read();
    }
}

