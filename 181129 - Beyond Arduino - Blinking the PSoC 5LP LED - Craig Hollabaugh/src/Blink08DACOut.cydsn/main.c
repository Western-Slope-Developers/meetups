#include "project.h"

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    Follower_Start();
    DAC_Start();
    for(;;)
    {
    }
}

