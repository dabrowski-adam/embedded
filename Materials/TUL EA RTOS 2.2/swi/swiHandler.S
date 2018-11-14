#
# *** SWI handler ***
#
        .text
        .arm
# ******************************************************************************
#   Declare external function
# ******************************************************************************
        .extern mySwiHandler

        .global _swi_handler
        .func   _swi_handler
_swi_handler:

                STMFD   SP!,{r0-r12,lr}     //store registers
                
                MOV     r1,SP
                MRS     r2,SPSR
                TST     r2,#0x20

                //execute if THUMB mode = 16-bit instructions
                LDRNEH  r0,[lr,#-2]         //Calculate address of SWI instruction and load it into r0
                BICNE   r0,r0,#0xff00     
                
                //execute if ARM mode = 32-bit instructions
                LDREQ   r0,[lr,#-4]         //Calculate address of SWI instruction and load it into r0
                BICEQ   r0,r0,#0xff000000   //Mask off top 8 bits of instruction to give SWI number
                
                
                //Call SWI handler
                //Use value in r0 to determine which SWI routine to execute
                //and r1 (second passed paramter) is pointing to stack top
                BL      mySwiHandler
                
                LDMFD   SP!, {r0-r12,pc}^   //Restore registers and return



        .size   _swi_handler, . - _swi_handler
        .endfunc

        .end

