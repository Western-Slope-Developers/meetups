#include <stdio.h>
#include "project.h"

int main(void)
{
    uint16_t raw;
    char buffer[20];
    CyGlobalIntEnable; /* Enable global interrupts. */

    CapSense_Start();
    CapSense_InitializeAllBaselines();
    CapSense_EnableWidget(CapSense_PROXIMITYSENSOR0__PROX);
    UART_Start();
    UART_PutString("Blink13CapSenseProximity\n");
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
            raw = CapSense_ReadSensorRaw(CapSense_PROXIMITYSENSOR0__PROX);
            sprintf(buffer,"%u\n",raw);
            UART_PutString(buffer);
            
            LED_Write(CapSense_CheckIsWidgetActive(CapSense_PROXIMITYSENSOR0__PROX));
            CapSense_ScanEnabledWidgets();
            CapSense_UpdateEnabledBaselines();
        }
   }
}
