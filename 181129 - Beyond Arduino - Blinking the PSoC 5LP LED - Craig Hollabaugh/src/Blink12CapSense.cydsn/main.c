#include "project.h"

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    CapSense_Start();
    CapSense_InitializeAllBaselines();
    
    for (int i = 0;i<5;i++){
        LED_Write(0);
        CyDelay(100);
        LED_Write(1);
        CyDelay(100);
    }
    CapSense_ScanEnabledWidgets();
    CapSense_UpdateEnabledBaselines();
    for(;;)
    {
        if(!CapSense_IsBusy())
        {
            LED_Write(CapSense_CheckIsWidgetActive(CapSense_BUTTON0__BTN));
            CapSense_ScanEnabledWidgets();
            CapSense_UpdateEnabledBaselines();
        }
   }
}
