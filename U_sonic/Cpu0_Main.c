/**********************************************************************************************************************
 * \file Cpu0_Main.c
 * \copyright Copyright (C) Infineon Technologies AG 2019
 * 
 * Use of this file is subject to the terms of use agreed between (i) you or the company in which ordinary course of 
 * business you are acting and (ii) Infineon Technologies AG or its licensees. If and as long as no such terms of use
 * are agreed, use of this file is subject to following:
 * 
 * Boost Software License - Version 1.0 - August 17th, 2003
 * 
 * Permission is hereby granted, free of charge, to any person or organization obtaining a copy of the software and 
 * accompanying documentation covered by this license (the "Software") to use, reproduce, display, distribute, execute,
 * and transmit the Software, and to prepare derivative works of the Software, and to permit third-parties to whom the
 * Software is furnished to do so, all subject to the following:
 * 
 * The copyright notices in the Software and this entire statement, including the above license grant, this restriction
 * and the following disclaimer, must be included in all copies of the Software, in whole or in part, and all 
 * derivative works of the Software, unless such copies or derivative works are solely in the form of 
 * machine-executable object code generated by a source language processor.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE 
 * COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN 
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *********************************************************************************************************************/
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include <stdio.h>
#define PORT00_BASE     (0xF003A000)
#define PORT00_OMR      (*(volatile unsigned int*)(PORT00_BASE + 0x04))
#define PORT00_IOCR4    (*(volatile unsigned int*)(PORT00_BASE + 0x14))
#define PORT00_IN       (*(volatile unsigned int*)(PORT00_BASE + 0x24))

#define PORT02_BASE     (0xF003A200)
#define PORT02_OMR      (*(volatile unsigned int*)(PORT02_BASE + 0x04))
#define PORT02_IOCR0    (*(volatile unsigned int*)(PORT02_BASE + 0X10))
#define PORT02_IOCR4    (*(volatile unsigned int*)(PORT02_BASE + 0x14))
#define PORT02_IN       (*(volatile unsigned int*)(PORT02_BASE + 0x24))

#define PORT10_BASE     (0xF003B000)
#define PORT10_OMR      (*(volatile unsigned int*)(PORT10_BASE + 0x04))
#define PORT10_IOCR0    (*(volatile unsigned int*)(PORT10_BASE + 0x10))
#define PORT10_IOCR4    (*(volatile unsigned int*)(PORT10_BASE + 0x14))
#define PORT10_IN       (*(volatile unsigned int*)(PORT10_BASE + 0x24))

#define P3              3
#define P4              4
#define P6              6
#define P7              7
#define PC0             3
#define PC1             11
#define PC2             19
#define PC3             27
#define PC4             3
#define PC5             11
#define PC6             19
#define PC7             27
#define PS1             1
#define PS3             3
#define PS4             4
#define PS5             5
#define PS6             6
#define PS7             7
#define PCL1             17
#define PCL3            19
#define PCL4            20
#define PCL5            21
#define PCL6            22
#define PCL7            23


void init_LED(void)
{
    // reset Port IOCR register
    PORT02_IOCR4 &= ~((0x1F) << PC7);
    PORT10_IOCR4 &= ~((0x1F) << PC5);
    PORT10_IOCR0 &= ~((0x1F) << PC3);
    PORT10_IOCR0 &= ~((0X1F) << PC1);

    // set Port as general purpose output (push-pull)
    PORT02_IOCR4 |= 0x11 << PC7;
    PORT10_IOCR4 |= 0x10 << PC5;
    PORT10_IOCR0 |= 0x10 << PC3;
    PORT10_IOCR0 |= 0X10 << PC1;
}

void init_Switch(void)
{
    PORT02_IOCR0 &= ~((0X1F)<<PC0);
    PORT02_IOCR0 |= ((0X2)<<PC0);

}

// SCU Registers
#define SCU_BASE            (0xF0036000)
#define SCU_WDT_CPU0CON0    (*(volatile unsigned int*)(SCU_BASE + 0x100))
#define SCU_EICR0           (*(volatile unsigned int*)(SCU_BASE + 0x210))
#define SCU_EICR1           (*(volatile unsigned int*)(SCU_BASE + 0x214))
#define SCU_IGCR0           (*(volatile unsigned int*)(SCU_BASE + 0x22C))

#define EXIS0               4
#define FEN0                8
#define REN0                9
#define EIEN0               11
#define INP0                12
#define IGP0                14

#define EXIS1               20
#define FEN1                24
#define REN1                25
#define EIEN1               27
#define INP1                28
#define IGP1                30

#define LCK                 1
#define ENDINIT             0

// SRC Registers
#define SRC_BASE            (0xF0038000)
#define SRC_CCU60_SR0       (*(volatile unsigned int*)(SRC_BASE + 0x420))
#define SRC_SCUERU0         (*(volatile unsigned int*)(SRC_BASE + 0xCD4))
#define SRC_SCUERU1         (*(volatile unsigned int*)(SRC_BASE + 0xCD8))

#define TOS                 11
#define SRE                 10
#define SRPN                0

void init_ERU(void)
{
    // ERU setting
    SCU_EICR1 &= ~((0x07) << EXIS0);
    SCU_EICR1 |=  ((0x02) << EXIS0);     // ERS2 - Input 2
    SCU_EICR1 &= ~((0x07) << EXIS1);
    SCU_EICR1 |=  ((0x02) << EXIS1);

    SCU_EICR1 |=  ((0x01) << FEN0);      // Enable falling edge
    SCU_EICR1 |=  ((0x01) << REN0);      // Enable rising edge
    SCU_EICR1 |=  (1 << FEN1);
    //SCU_EICR1 |=  (1 << REN1);

    SCU_EICR1 |=  ((0x01) << EIEN0);     // External Interrupt Enable
    SCU_EICR1 |=  ((0x01) << EIEN1);

    SCU_EICR1 &= ~((0x07) << INP0);      // Input Node Pointer from OGU0
    SCU_EICR1 &= ~((0x07) << INP1);
    SCU_EICR1 |=  ((0x01) << INP1);


    SCU_IGCR0 &= ~((0x03) << IGP0);
    SCU_IGCR0 |=  ((0x01) << IGP0);      // Interrupt Gatting Pattern 0

    SCU_IGCR0 &= ~((0x3) << IGP1);
    SCU_IGCR0 |=  ((0x1) << IGP1);


    // SRC Interrupt setting
    SRC_SCUERU0 &= ~((0xFF) << SRPN);
    SRC_SCUERU0 |=  ((0x0A) << SRPN);

    SRC_SCUERU0 &= ~((0x03) << TOS);
    SRC_SCUERU0 |=  ((0x01) << SRE);

    SRC_SCUERU1 &= ~((0xFF) << SRPN);
    SRC_SCUERU1 |=  ((0x0C) << SRPN);

    SRC_SCUERU1 &= ~((0x3) << TOS);
    SRC_SCUERU1 |=  (1 << SRE);
}

// CCU6 Registers
#define CCU60_BASE          (0xF0002A00)
#define CCU60_CLC           (*(volatile unsigned int*)(CCU60_BASE + 0x00))
#define CCU60_T12           (*(volatile unsigned int*)(CCU60_BASE + 0x20))
#define CCU60_T12PR         (*(volatile unsigned int*)(CCU60_BASE + 0x24))
#define CCU60_TCTR0         (*(volatile unsigned int*)(CCU60_BASE + 0x70))
#define CCU60_TCTR2         (*(volatile unsigned int*)(CCU60_BASE + 0x74))
#define CCU60_TCTR4         (*(volatile unsigned int*)(CCU60_BASE + 0x78))
#define CCU60_INP           (*(volatile unsigned int*)(CCU60_BASE + 0xAC))
#define CCU60_IEN           (*(volatile unsigned int*)(CCU60_BASE + 0xB0))

#define CCU61_BASE          (0xF0002B00)
#define CCU61_CLC           (*(volatile unsigned int*)(CCU61_BASE + 0x00))
#define CCU61_T12           (*(volatile unsigned int*)(CCU61_BASE + 0x20))
#define CCU61_T12PR         (*(volatile unsigned int*)(CCU61_BASE + 0x24))
#define CCU61_TCTR0         (*(volatile unsigned int*)(CCU61_BASE + 0x70))
#define CCU61_TCTR2         (*(volatile unsigned int*)(CCU61_BASE + 0x74))
#define CCU61_TCTR4         (*(volatile unsigned int*)(CCU61_BASE + 0x78))
#define CCU61_INP           (*(volatile unsigned int*)(CCU61_BASE + 0xAC))
#define CCU61_IEN           (*(volatile unsigned int*)(CCU61_BASE + 0xB0))


#define DISS                1
#define DISR                0
#define CTM                 7
#define T12PRE              3
#define T12CLK              0
#define T12STR              6
#define INPT12              10
#define ENT12PM             7
#define T12SSC              0
#define T12RR               0
#define T12RS               1
#define T12RES              2

volatile uint32 range;
volatile uint8 range_valid_flag = 0;

void init_USonic(void)
{
    PORT02_IOCR4 &= ~(0x1F << PC6);     // reset PORT02_IOCR4 PC6
    PORT00_IOCR4 &= ~(0x1F << PC4);     // reset PORT00_IOCR4 PC4

    PORT00_IOCR4 |= 0x01 << PC4;        // set P00.4 general input (pull-down connected) [Echo]
    PORT02_IOCR4 |= 0x10 << PC6;        // set P02.6 push-pull general output            [Trig]

    PORT02_OMR |= ((0x01) << PCL6);
}


void init_CCU60(void)
{
    // Password Access to unlock SCU_WDTSCON0
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0);    // wait until unlocked

    // Modify Access to clear ENDINIT
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) & ~(1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0);    // wait until locked

    CCU60_CLC &= ~(1 << DISR);                      // enable CCU

    // Password Access to unlock SCU_WDTSCON0
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0);    // wait until unlocked

    // Modify Access to set ENDINIT
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0);    // wait until locked

    // CCU60 T12 configurations
    while((CCU60_CLC & (1 << DISS)) != 0);          // wait until CCU60 module enabled


    CCU60_TCTR0 &= ~((0x07) << T12CLK);     // f_T12 = f_CCU6 / prescaler
    CCU60_TCTR0 |=  ((0x02) << T12CLK);     // f_CCU6 = 100 MHz, prescaler = 4

    CCU60_TCTR0 &= ~((0x01) << CTM);        // T12 auto reset when period match (PM) occur

    CCU60_T12PR = 250 - 1;                  // PM interrupt freq. = f_T12 / (T12PR + 1)
    CCU60_TCTR4 |= ((0x01) << T12STR);      // load T12PR from shadow register

    CCU60_TCTR2 =  ((0x01) << T12SSC);      // Single Shot Control

    CCU60_T12 = 0;                          // clear T12 counter register


    // CCU60 T12 PM interrupt setting
    CCU60_INP &= ~((0x03) << INPT12);       // service request output SR0 selected
    CCU60_IEN |=  ((0x01) << ENT12PM);      // enable T12 PM interrupt


    // SRC setting for CCU60
    SRC_CCU60_SR0 &= ~((0xFF) << SRPN);
    SRC_CCU60_SR0 |=  ((0x0B) << SRPN);     // set priority 0x0B

    SRC_CCU60_SR0 &= ~((0x03) << TOS);      // CPU0 service T12 PM interrupt
    SRC_CCU60_SR0 |=  ((0x01) << SRE);      // SR0 enabled
}

void usonicTrigger(void)
{
    // start of 10us Trigger Pulse
    // GPIO P02.6 --> HIGH
    PORT02_OMR |= ((0x01) << PS6);
    range_valid_flag = 0;
    CCU60_TCTR4 = 0x1 << T12RS;         // T12 start counting
}


__interrupt(0x0B) __vector_table(0)
void CCU60_T12_ISR(void)
{
    // end of 10us Trig
    // GPIO P02.6 --> LOW
    PORT02_OMR |= ((0x01) << PCL6);
}



void init_CCU61(void)
{

    // Password Access to unlock SCU_WDTSCON0
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0);    // wait until unlocked

    // Modify Access to clear ENDINIT
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) & ~(1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0);    // wait until locked

    CCU61_CLC &= ~(1 << DISR);                      // enable CCU

    // Password Access to unlock SCU_WDTSCON0
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) & ~(1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) != 0);    // wait until unlocked

    // Modify Access to set ENDINIT
    SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) | (1 << ENDINIT);
    while((SCU_WDT_CPU0CON0 & (1 << LCK)) == 0);    // wait until locked

    // CCU60 T12 configurations
    while((CCU61_CLC & (1 << DISS)) != 0);          // wait until CCU60 module enabled



    CCU61_TCTR0 &= ~(0x7 << T12CLK);        // f_T12 = f_CCU6 / prescaler = 25 MHz
    CCU61_TCTR0 |= 0x2 << T12CLK;           // f_CCU6 = 100 MHz, prescaler = 4

    CCU61_TCTR0 |= 0x1 << T12PRE;           // f_T12 = f_CCU6 / 256 = 97,656 Hz


    CCU61_T12PR = 100000 -1;                // PM interrupt freq. = f_T12 / (T12PR + 1)
    CCU61_TCTR4 |= 0x1 << T12STR;           // load T12PR from shadow register

    CCU61_T12 = 0;                          // clear T12 counter register
}

__interrupt(0x0A) __vector_table(0)
void ERU0_ISR(void)
{
    if( (PORT00_IN & (0x1 << P4)) != 0 )    // rising edge of echo
    {
        //                    _________
        //        echo ______|
        //                   ^
        CCU61_TCTR4 = 0x01 << T12RS;        // start CCU61 T12 counter
    }
    else                                    // falling edge of echo
    {
        //             ______
        //        echo       |______
        //                   ^
        CCU61_TCTR4 = ((0x01) << T12RR);    // stop CCU61 T12 counter

        // (1 / t_freq) * counter * 1000000 / 58  =  centimeter
        //range = ((CCU61_T12 * 1000000) / 48828) / 58;
        range = ((CCU61_T12 * 1000000) / 97656) / 58;
        if (range < 500)
            range_valid_flag = 1;

        CCU61_TCTR4 |=  ((0x01) << T12RES);  // reset CCU61 T12 counter
    }
}

//GTM Registers
//GTM CMU
#define GTM_BASE            (0xF0100000)
#define GTM_CMU_CLK_EN      (*(volatile unsigned int*)(GTM_BASE + 0x00300))
#define GTM_CMU_FXCLK_CTRL  (*(volatile unsigned int*)(GTM_BASE + 0x00344))

#define EN_FXCLK            22
#define FXCLK_SEL           0

//GTM TOM0

#define GTM_TOM0_TGC1_GLB_CTRL     (*(volatile unsigned int*)(GTM_BASE + 0x08230))
#define GTM_TOM0_TGC1_ENDIS_CTRL   (*(volatile unsigned int*)(GTM_BASE + 0x08270))
#define GTM_TOM0_TGC1_OUTEN_CTRL   (*(volatile unsigned int*)(GTM_BASE + 0x08278))
#define GTM_TOM0_TGC1_FUPD_CTRL    (*(volatile unsigned int*)(GTM_BASE + 0x08238))

#define GTM_TOM0_CH15_CTRL          (*(volatile unsigned int*)(GTM_BASE + 0x083C0))
#define GTM_TOM0_CH15_SR0           (*(volatile unsigned int*)(GTM_BASE + 0x083C4))
#define GTM_TOM0_CH15_SR1           (*(volatile unsigned int*)(GTM_BASE + 0x083C8))

#define CLK_SRC_SR         12
#define SL                 11
#define HOST_TRIG           0
#define UPEN_CTRL15         30
#define ENDIS_CTRL15        14
#define OUTEN_CTRL15        14
#define RSTCN0_CH15         30
#define FUPD_CTRL15         14

//GTM
#define GTM_CLC            (*(volatile unsigned int*)(GTM_BASE + 0x9FD00))
#define GTM_TOUTSEL6       (*(volatile unsigned int*)(GTM_BASE + 0x9FD48))
#define GTM_TOUTSEL1       (*(volatile unsigned int*)(GTM_BASE + 0x9FD34))

#define DISS               1
#define DISR               0
#define SEL7               14
#define SEL8               16
#define SEL11              22

void init_GTM_TOM0_PWM(void)
{
    /* Passwaord Access to unlock WDTSCON0 */
       SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) &~ (1 << LCK)) | (1 << ENDINIT);
       while ((SCU_WDT_CPU0CON0 & (1 << LCK))!=0);

       // Modify Access to clear ENDINIT bit
       SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) & ~ (1 << ENDINIT);
       while ((SCU_WDT_CPU0CON0 & (1 << LCK))==0);

       GTM_CLC &= ~(1 << DISR);

    /* Passwaord Access to unlock WDTSCON0 */
       SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) &~ (1 << LCK)) | (1 << ENDINIT);
       while ((SCU_WDT_CPU0CON0 & (1 << LCK))!=0);

    // Modify Access to clear ENDINIT bit
       SCU_WDT_CPU0CON0 = ((SCU_WDT_CPU0CON0 ^ 0xFC) | (1 << LCK)) | (1 << ENDINIT);
       while ((SCU_WDT_CPU0CON0 & (1 << LCK))==0);

       while ((GTM_CLC & (1 <<DISS)) != 0);

      //GTM Clock Setting
      GTM_CMU_FXCLK_CTRL &= ~((0xF) << FXCLK_SEL);

      GTM_CMU_CLK_EN &= ~((0x3) << EN_FXCLK);
      GTM_CMU_CLK_EN |=   ((0x2) << EN_FXCLK);

      GTM_TOM0_TGC1_GLB_CTRL &= ~((0x3) << UPEN_CTRL15);
      GTM_TOM0_TGC1_GLB_CTRL |=   ((0x2) << UPEN_CTRL15);

      GTM_TOM0_TGC1_FUPD_CTRL &= ~((0x3) << FUPD_CTRL15);
      GTM_TOM0_TGC1_FUPD_CTRL |=   ((0x2) << FUPD_CTRL15);
      GTM_TOM0_TGC1_FUPD_CTRL &= ~((0x3) << RSTCN0_CH15);
      GTM_TOM0_TGC1_FUPD_CTRL |=   ((0x2) << RSTCN0_CH15);

      GTM_TOM0_TGC1_ENDIS_CTRL &= ~((0x3) << ENDIS_CTRL15);
      GTM_TOM0_TGC1_ENDIS_CTRL |=   ((0x2) << ENDIS_CTRL15);
      GTM_TOM0_TGC1_OUTEN_CTRL &= ~((0x3) << OUTEN_CTRL15);
      GTM_TOM0_TGC1_OUTEN_CTRL |=   ((0x2) << OUTEN_CTRL15);

      GTM_TOM0_CH15_CTRL |=  (1 << SL);
      GTM_TOM0_CH15_CTRL &= ~((0x7) << CLK_SRC_SR);
      GTM_TOM0_CH15_CTRL |=  (1     << CLK_SRC_SR);
      GTM_TOM0_CH15_SR0 = 12500 - 1;
      GTM_TOM0_CH15_SR1 = 0;

      GTM_TOUTSEL1 &= ~((0x3) << SEL7);
      GTM_TOM0_TGC1_GLB_CTRL |= (1 << HOST_TRIG);
}


IfxCpu_syncEvent g_cpuSyncEvent = 0;


int core0_main(void)
{
    IfxCpu_enableInterrupts();

    /* !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdogs and service them periodically if it is required
     */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    init_ERU();
    init_CCU60();
    init_CCU61();
    init_LED();
    init_USonic();
    init_GTM_TOM0_PWM();

    while(1)
    {
        usonicTrigger();

        for (uint8 i = 0; i < 4; i++)
            for(uint32 j = 0; j < 10000000; j++);

        if (range_valid_flag > 0) {

            if( range >= 30 ) // scenario_1
            {
                GTM_TOM0_CH15_SR1 = 0;
                PORT10_OMR |= ((0x01) << PCL3);
                PORT10_OMR |= ((0x01) << PCL5);
            }
            else if( range >= 20 ) // scenario_2 /
            {
                GTM_TOM0_CH15_SR1 = 1250-1;
                PORT10_OMR |= ((0x01) << PCL3);
                PORT10_OMR |= ((0x01) << PCL5);
            }
            else if( range >= 10) // scenario_3
            {
                GTM_TOM0_CH15_SR1 = 5250-1;;
                PORT10_OMR |= ((0x01) << PCL3);
                PORT10_OMR |= ((0x01) << PCL5);
            }
            else // white / scenario_4
            {
                GTM_TOM0_CH15_SR1 = 12500-1;
                PORT10_OMR |= ((0x01) << PCL3);
                PORT10_OMR |= ((0x01) << PCL5);
            }

        }

    }
    return (1);
}

__interrupt(0x0C) __vector_table(0)
void ERU1_ISR(void)
{
    GTM_TOM0_CH15_SR1 = 0;

}



