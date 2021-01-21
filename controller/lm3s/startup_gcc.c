//*****************************************************************************
//
// startup_gcc.c - Startup code for use with GNU tools.
//
// Copyright (c) 2009 Luminary Micro, Inc.  All rights reserved.
// Software License Agreement
// 
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's microcontroller products.
// 
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  You may not combine
// this software with "viral" open-source software in order to form a larger
// program.  Any use in violation of the foregoing restrictions may subject
// the user to criminal sanctions under applicable laws, as well as to civil
// liability for the breach of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 32 of the Stellaris CMSIS Package.
//
//*****************************************************************************

#define WEAK __attribute__ ((weak))

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
void WEAK Reset_Handler(void);
static void Default_Handler(void);
void WEAK NMI_Handler(void);
void WEAK HardFault_Handler(void);
void WEAK MemManage_Handler(void);
void WEAK BusFault_Handler(void);
void WEAK UsageFault_Handler(void);
void WEAK MemManage_Handler(void);
void WEAK SVC_Handler(void);
void WEAK DebugMon_Handler(void);
void WEAK PendSV_Handler(void);
void WEAK SysTick_Handler(void);
void WEAK GPIOPortA_IRQHandler(void);
void WEAK GPIOPortB_IRQHandler(void);
void WEAK GPIOPortC_IRQHandler(void);
void WEAK GPIOPortD_IRQHandler(void);
void WEAK GPIOPortE_IRQHandler(void);
void WEAK UART0_IRQHandler(void);
void WEAK UART1_IRQHandler(void);
void WEAK SSI0_IRQHandler(void);
void WEAK I2C0_IRQHandler(void);
void WEAK PWMFault_IRQHandler(void);
void WEAK PWMGen0_IRQHandler(void);
void WEAK PWMGen1_IRQHandler(void);
void WEAK PWMGen2_IRQHandler(void);
void WEAK QEI0_IRQHandler(void);
void WEAK ADCSeq0_IRQHandler(void);
void WEAK ADCSeq1_IRQHandler(void);
void WEAK ADCSeq2_IRQHandler(void);
void WEAK ADCSeq3_IRQHandler(void);
void WEAK Watchdog_IRQHandler(void);
void WEAK Timer0A_IRQHandler(void);
void WEAK Timer0B_IRQHandler(void);
void WEAK Timer1A_IRQHandler(void);
void WEAK Timer1B_IRQHandler(void);
void WEAK Timer2A_IRQHandler(void);
void WEAK Timer2B_IRQHandler(void);
void WEAK Comp0_IRQHandler(void);
void WEAK Comp1_IRQHandler(void);
void WEAK Comp2_IRQHandler(void);
void WEAK SysCtrl_IRQHandler(void);
void WEAK FlashCtrl_IRQHandler(void);
void WEAK GPIOPortF_IRQHandler(void);
void WEAK GPIOPortG_IRQHandler(void);
void WEAK GPIOPortH_IRQHandler(void);
void WEAK UART2_IRQHandler(void);
void WEAK SSI1_IRQHandler(void);
void WEAK Timer3A_IRQHandler(void);
void WEAK Timer3B_IRQHandler(void);
void WEAK I2C1_IRQHandler(void);
void WEAK QEI1_IRQHandler(void);
void WEAK CAN0_IRQHandler(void);
void WEAK CAN1_IRQHandler(void);
void WEAK CAN2_IRQHandler(void);
void WEAK Ethernet_IRQHandler(void);
void WEAK Hibernate_IRQHandler(void);

//*****************************************************************************
//
// The entry point for the application.
//
//*****************************************************************************
extern int main(void);

//*****************************************************************************
//
// Reserve space for the system stack.
//
//*****************************************************************************
static unsigned long pulStack[64];

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{
    (void (*)(void))((unsigned long)pulStack + sizeof(pulStack)),
                                            // The initial stack pointer
    Reset_Handler,                          // The reset handler
    NMI_Handler,                            // The NMI handler
    HardFault_Handler,                      // The hard fault handler
    MemManage_Handler,                      // The MPU fault handler
    BusFault_Handler,                       // The bus fault handler
    UsageFault_Handler,                     // The usage fault handler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    SVC_Handler,                            // SVCall handler
    DebugMon_Handler,                       // Debug monitor handler
    0,                                      // Reserved
    PendSV_Handler,                         // The PendSV handler
    SysTick_Handler,                        // The SysTick handler

    //
    // External Interrupts
    //
    GPIOPortA_IRQHandler,                   // GPIO Port A
    GPIOPortB_IRQHandler,                   // GPIO Port B
    GPIOPortC_IRQHandler,                   // GPIO Port C
    GPIOPortD_IRQHandler,                   // GPIO Port D
    GPIOPortE_IRQHandler,                   // GPIO Port E
    UART0_IRQHandler,                       // UART0 Rx and Tx
    UART1_IRQHandler,                       // UART1 Rx and Tx
    SSI0_IRQHandler,                        // SSI0 Rx and Tx
    I2C0_IRQHandler,                        // I2C0 Master and Slave
    PWMFault_IRQHandler,                    // PWM Fault
    PWMGen0_IRQHandler,                     // PWM Generator 0
    PWMGen1_IRQHandler,                     // PWM Generator 1
    PWMGen2_IRQHandler,                     // PWM Generator 2
    QEI0_IRQHandler,                        // Quadrature Encoder 0
    ADCSeq0_IRQHandler,                     // ADC Sequence 0
    ADCSeq1_IRQHandler,                     // ADC Sequence 1
    ADCSeq2_IRQHandler,                     // ADC Sequence 2
    ADCSeq3_IRQHandler,                     // ADC Sequence 3
    Watchdog_IRQHandler,                    // Watchdog timer
    Timer0A_IRQHandler,                     // Timer 0 subtimer A
    Timer0B_IRQHandler,                     // Timer 0 subtimer B
    Timer1A_IRQHandler,                     // Timer 1 subtimer A
    Timer1B_IRQHandler,                     // Timer 1 subtimer B
    Timer2A_IRQHandler,                     // Timer 2 subtimer A
    Timer2B_IRQHandler,                     // Timer 2 subtimer B
    Comp0_IRQHandler,                       // Analog Comparator 0
    Comp1_IRQHandler,                       // Analog Comparator 1
    Comp2_IRQHandler,                       // Analog Comparator 2
    SysCtrl_IRQHandler,                     // System Control (PLL, OSC, BO)
    FlashCtrl_IRQHandler,                   // FLASH Control
    GPIOPortF_IRQHandler,                   // GPIO Port F
    GPIOPortG_IRQHandler,                   // GPIO Port G
    GPIOPortH_IRQHandler,                   // GPIO Port H
    UART2_IRQHandler,                       // UART2 Rx and Tx
    SSI1_IRQHandler,                        // SSI1 Rx and Tx
    Timer3A_IRQHandler,                     // Timer 3 subtimer A
    Timer3B_IRQHandler,                     // Timer 3 subtimer B
    I2C1_IRQHandler,                        // I2C1 Master and Slave
    QEI1_IRQHandler,                        // Quadrature Encoder 1
    CAN0_IRQHandler,                        // CAN0
    CAN1_IRQHandler,                        // CAN1
    CAN2_IRQHandler,                        // CAN2
    Ethernet_IRQHandler,                    // Ethernet
    Hibernate_IRQHandler                    // Hibernate
};

//*****************************************************************************
//
// The following are constructs created by the linker, indicating where the
// the "data" and "bss" segments reside in memory.  The initializers for the
// for the "data" segment resides immediately following the "text" segment.
//
//*****************************************************************************
extern unsigned long _etext;
extern unsigned long _data;
extern unsigned long _edata;
extern unsigned long _bss;
extern unsigned long _ebss;

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
void
Reset_Handler(void)
{
    unsigned long *pulSrc, *pulDest;

    //
    // Copy the data segment initializers from flash to SRAM.
    //
    pulSrc = &_etext;
    for(pulDest = &_data; pulDest < &_edata; )
    {
        *pulDest++ = *pulSrc++;
    }

    //
    // Zero fill the bss segment.  This is done with inline assembly since this
    // will clear the value of pulDest if it is not kept in a register.
    //
    __asm("    ldr     r0, =_bss\n"
          "    ldr     r1, =_ebss\n"
          "    mov     r2, #0\n"
          "    ldr     sp, =_stack_top\n"
          "    .thumb_func\n"
          "zero_loop:\n"
          "        cmp     r0, r1\n"
          "        it      lt\n"
          "        strlt   r2, [r0], #4\n"
          "        blt     zero_loop");

    //
    // Call the application's entry point.
    //
    main();

    // QEMU doesn't provide a good way to gracefully exit for baremetal apps
    // this is to intentionally crash QEMU with an error like
    // "qemu-system-arm: Trying to execute code outside RAM or ROM at 0x77777776"
    void (*die)(void) = (void (*)(void))0x77777777;
    die();
}

//*****************************************************************************
//
// Provide weak aliases for each Exception handler to the Default_Handler.
// As they are weak aliases, any function with the same name will override
// this definition.
//
//*****************************************************************************
#pragma weak NMI_Handler = Default_Handler
#pragma weak HardFault_Handler = Default_Handler
#pragma weak MemManage_Handler = Default_Handler
#pragma weak BusFault_Handler = Default_Handler
#pragma weak UsageFault_Handler = Default_Handler
#pragma weak SVC_Handler = Default_Handler
#pragma weak DebugMon_Handler = Default_Handler
#pragma weak PendSV_Handler = Default_Handler
#pragma weak SysTick_Handler = Default_Handler
#pragma weak GPIOPortA_IRQHandler = Default_Handler
#pragma weak GPIOPortB_IRQHandler = Default_Handler
#pragma weak GPIOPortC_IRQHandler = Default_Handler
#pragma weak GPIOPortD_IRQHandler = Default_Handler
#pragma weak GPIOPortE_IRQHandler = Default_Handler
#pragma weak UART0_IRQHandler = Default_Handler
#pragma weak UART1_IRQHandler = Default_Handler
#pragma weak SSI0_IRQHandler = Default_Handler
#pragma weak I2C0_IRQHandler = Default_Handler
#pragma weak PWMFault_IRQHandler = Default_Handler
#pragma weak PWMGen0_IRQHandler = Default_Handler
#pragma weak PWMGen1_IRQHandler = Default_Handler
#pragma weak PWMGen2_IRQHandler = Default_Handler
#pragma weak QEI0_IRQHandler = Default_Handler
#pragma weak ADCSeq0_IRQHandler = Default_Handler
#pragma weak ADCSeq1_IRQHandler = Default_Handler
#pragma weak ADCSeq2_IRQHandler = Default_Handler
#pragma weak ADCSeq3_IRQHandler = Default_Handler
#pragma weak Watchdog_IRQHandler = Default_Handler
#pragma weak Timer0A_IRQHandler = Default_Handler
#pragma weak Timer0B_IRQHandler = Default_Handler
#pragma weak Timer1A_IRQHandler = Default_Handler
#pragma weak Timer1B_IRQHandler = Default_Handler
#pragma weak Timer2A_IRQHandler = Default_Handler
#pragma weak Timer2B_IRQHandler = Default_Handler
#pragma weak Comp0_IRQHandler = Default_Handler
#pragma weak Comp1_IRQHandler = Default_Handler
#pragma weak Comp2_IRQHandler = Default_Handler
#pragma weak SysCtrl_IRQHandler = Default_Handler
#pragma weak FlashCtrl_IRQHandler = Default_Handler
#pragma weak GPIOPortF_IRQHandler = Default_Handler
#pragma weak GPIOPortG_IRQHandler = Default_Handler
#pragma weak GPIOPortH_IRQHandler = Default_Handler
#pragma weak UART2_IRQHandler = Default_Handler
#pragma weak SSI1_IRQHandler = Default_Handler
#pragma weak Timer3A_IRQHandler = Default_Handler
#pragma weak Timer3B_IRQHandler = Default_Handler
#pragma weak I2C1_IRQHandler = Default_Handler
#pragma weak QEI1_IRQHandler = Default_Handler
#pragma weak CAN0_IRQHandler = Default_Handler
#pragma weak CAN1_IRQHandler = Default_Handler
#pragma weak CAN2_IRQHandler = Default_Handler
#pragma weak Ethernet_IRQHandler = Default_Handler
#pragma weak Hibernate_IRQHandler = Default_Handler

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void
Default_Handler(void)
{
    //
    // Go into an infinite loop.
    //
    while(1)
    {
    }
}
