#include <stdio.h>
#include "project.h"

#define DACOFFSET 1000
#define DIFFGAIN 5
int main(void)
{
    uint16_t raw, raw0;
    int16_t period;
    CyGlobalIntEnable; /* Enable global interrupts. */

    CapSense_Start();
    CapSense_InitializeAllBaselines();
    CapSense_EnableWidget(CapSense_PROXIMITYSENSOR0__PROX);
    UART_Start();
    UART_PutString("Blink14Theremin\n");
    Counter_Start();

    WaveDAC1_Start();
    WaveDAC2_Start();
    WaveDAC3_Start();
    
    AMux_Start();
    AMux_Connect(0);
    AMux_Connect(1);
    AMux_Connect(2);
    
    for (int i = 0;i<5;i++){
        LED_Write(0);
        CyDelay(100);
        LED_Write(1);
        CyDelay(100);
    }
    CapSense_ScanEnabledWidgets();
    CapSense_UpdateEnabledBaselines();
    raw0 = CapSense_ReadSensorRaw(CapSense_PROXIMITYSENSOR0__PROX);
    for(;;)
    {
        if(!CapSense_IsBusy())
        {
            raw = CapSense_ReadSensorRaw(CapSense_PROXIMITYSENSOR0__PROX);
            period = DACOFFSET - DIFFGAIN*(raw - raw0);
            if (period < 3) {
                period = 3;
            }
            Counter_WritePeriod(period);
            
            LED_Write(CapSense_CheckIsWidgetActive(CapSense_PROXIMITYSENSOR0__PROX));
            CapSense_ScanEnabledWidgets();
            CapSense_UpdateEnabledBaselines();
        }
   }
}

/*
    int16_t min,max;
    min = 10000;
    max = 0;
    char buffer[20];
            sprintf(buffer,"%u\n",raw);
            UART_PutString(buffer);

            if (rawdelta < min) {
                min = rawdelta;
                sprintf(buffer,"min %u\n",min);
                UART_PutString(buffer);
            }
            if (rawdelta > max) {
                max = rawdelta;
                sprintf(buffer,"max %u\n",max);
                UART_PutString(buffer);
            }
*/   
