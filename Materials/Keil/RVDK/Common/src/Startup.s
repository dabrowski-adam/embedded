;/*****************************************************************************
;*   startup.s:  Startup file for Philips LPC214x Family Microprocessors
;*
;*   Copyright(C) 2006, Philips Semiconductor
;*   All rights reserved.
;*
;*   History
;*   2005.10.01  ver 1.00    Prelimnary version, first Release
;*
;*****************************************************************************/

	PRESERVE8
	
;define the stack size
SVC_STACK_LEGTH		EQU	0
FIQ_STACK_LEGTH		EQU	0
IRQ_STACK_LEGTH		EQU	256
ABT_STACK_LEGTH		EQU	0
UND_STACK_LEGTH		EQU	0

I_Bit			EQU	0x80
F_Bit			EQU	0x40

USR32Mode		EQU	0x10
SVC32Mode		EQU	0x13
SYS32Mode		EQU	0x1F
IRQ32Mode		EQU	0x12
FIQ32Mode		EQU	0x11
ABT32Mode		EQU	0x17
UDF32Mode		EQU	0x1B

;The imported labels
    IMPORT  FIQ_Exception		;Fast interrupt exceptions handler 
    IMPORT  __main				;The entry point to the main function 
    IMPORT  TargetResetInit		;initialize the target board
    IMPORT  SoftwareInterrupt	;SWI handler from swi_handler.s
       
;The exported labels
    EXPORT  bottom_of_heap
    EXPORT  StackUsr
    EXPORT  Reset
    EXPORT __user_initial_stackheap
    
    CODE32

    AREA    vectors,CODE,READONLY
    ENTRY

;interrupt vectors
Reset
        LDR     PC, ResetAddr
        LDR     PC, UndefinedAddr
        LDR     PC, SWI_Addr
        LDR     PC, PrefetchAddr
        LDR     PC, DataAbortAddr
        DCD     0xb9205f80
        LDR     PC, [PC, #-0x0ff0]
        LDR     PC, FIQ_Addr

ResetAddr	DCD     ResetInit
UndefinedAddr	DCD     Undefined
SWI_Addr	DCD     SoftwareInterrupt
PrefetchAddr	DCD     PrefetchAbort
DataAbortAddr	DCD     DataAbort
Nouse		DCD     0
IRQ_Addr	DCD     0
FIQ_Addr	DCD     FIQ_Handler

Undefined
	B	Undefined

PrefetchAbort
	B       PrefetchAbort

DataAbort
	B       DataAbort

FIQ_Handler
	STMFD   SP!, {R0-R3, LR}
	BL      FIQ_Exception
	LDMFD   SP!, {R0-R3, LR}
	SUBS    PC,  LR,  #4

;*****************************************************************************
;*  Initialize the stacks                                                    *
;*  Function : void InitStack(void)                                          *
;*      Parameters                                                           *
;*          input  : None                                                    * 
;*          output : None                                                    *
;*****************************************************************************
InitStack    
	MOV     R0, LR
;Build the SVC stack
	MSR     CPSR_c, #SVC32Mode		
	LDR     SP, StackSvc	
;Build the IRQ stack
	MSR     CPSR_c, #IRQ32Mode
	LDR     SP, StackIrq
;Build the FIQ stack
	MSR     CPSR_c, #FIQ32Mode
	LDR     SP, StackFiq
;Build the DATAABORT stack
	MSR     CPSR_c, #ABT32Mode
	LDR     SP, StackAbt
;Build the UDF stack
	MSR     CPSR_c, #UDF32Mode
	LDR     SP, StackUnd
;Build the SYS stack
	MSR     CPSR_c, #SYS32Mode
	LDR     SP, =StackUsr

	MOV     PC, R0

;******************************************************************************
;*  Reset Entry                                                               *
;*  Function : void ResetInit(void)                                           *
;*      Parameters                                                            *
;*          input  : None                                                     * 
;*          output : None                                                     *
;******************************************************************************
ResetInit
        
	BL      InitStack               ;Initialize the stack
	BL      TargetResetInit         ;Initialize the target board
                                        ;Jump to the entry point of C program
	B       __main

;******************************************************************************
;*  Initialize the function library stacks and heaps, can't be                * 
;*  deleted                                                                   *
;*  Function : void __user_initial_stackheap(void)                            *
;*      Parameters                                                            *
;*          input  : None                                                     * 
;*          output : None                                                     *
;******************************************************************************
__user_initial_stackheap    
	LDR	r0,=bottom_of_heap
;    LDR   r1,=StackUsr
	MOV	pc,lr
            
StackSvc	DCD     SvcStackSpace + (SVC_STACK_LEGTH - 1)* 4
StackIrq	DCD     IrqStackSpace + (IRQ_STACK_LEGTH - 1)* 4
StackFiq	DCD     FiqStackSpace + (FIQ_STACK_LEGTH - 1)* 4
StackAbt	DCD     AbtStackSpace + (ABT_STACK_LEGTH - 1)* 4
StackUnd	DCD     UndStackSpace + (UND_STACK_LEGTH - 1)* 4

;******************************************************************************
;*  Set security to the chip                                                  *
;*  Function : void CrpData(void)                                             *
;*      Parameters                                                            *
;*          input  : None                                                     * 
;*          output : None                                                     *
;******************************************************************************
    IF :DEF: EN_CRP
        IF  . >= 0x1fc
        INFO    1,"\nThe data at 0x000001fc must be 0x87654321.\nPlease delete some source before this line."
        ENDIF
CrpData
	WHILE . < 0x1fc
	NOP
	WEND
CrpData1
	DCD     0x87654321          ;When the Data is 0x87654321,user code be protected.
    ENDIF
    
        AREA    MyStacks, DATA, NOINIT, ALIGN=2
SvcStackSpace	SPACE   SVC_STACK_LEGTH * 4  ;Stack spaces for ADMIN Mode
IrqStackSpace	SPACE   IRQ_STACK_LEGTH * 4  ;Stack spaces for IRQ Mode
FiqStackSpace	SPACE   FIQ_STACK_LEGTH * 4  ;Stack spaces for FIQ Mode
AbtStackSpace	SPACE   ABT_STACK_LEGTH * 4  ;Stack spaces for ABORT Mode
UndStackSpace	SPACE   UND_STACK_LEGTH * 4  ;Stack spaces for UNDEF Mode

        AREA    Heap, DATA, NOINIT
bottom_of_heap	SPACE   1

	AREA    Stacks, DATA, NOINIT
StackUsr    	SPACE   1

           END

;/*****************************************************************************
;**                            End Of File
;*****************************************************************************/
