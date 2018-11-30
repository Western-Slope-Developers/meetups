#include "project.h"

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    for(;;)
    {
        ControlRegister_Write(1);
        CyDelay(1000);
        ControlRegister_Write(0);
        CyDelay(2000);
    }
}

