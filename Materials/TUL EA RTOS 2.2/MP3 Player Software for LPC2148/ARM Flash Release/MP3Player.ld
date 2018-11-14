MEMORY
{
  UNPLACED_SECTIONS (wx) : ORIGIN = 0x100000000, LENGTH = 0
  AHB_Peripherals (wx) : ORIGIN = 0xffe00000, LENGTH = 0x00200000
  APB_Peripherals (wx) : ORIGIN = 0xe0000000, LENGTH = 0x00200000
  USB_DMA_RAM (wx) : ORIGIN = 0x7fd00000, LENGTH = 0x00002000
  SRAM (wx) : ORIGIN = 0x40000000, LENGTH = 0x00008000
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00080000
}


SECTIONS
{
  __AHB_Peripherals_segment_start__ = 0xffe00000;
  __AHB_Peripherals_segment_end__ = 0x00000000;
  __APB_Peripherals_segment_start__ = 0xe0000000;
  __APB_Peripherals_segment_end__ = 0xe0200000;
  __USB_DMA_RAM_segment_start__ = 0x7fd00000;
  __USB_DMA_RAM_segment_end__ = 0x7fd02000;
  __SRAM_segment_start__ = 0x40000000;
  __SRAM_segment_end__ = 0x40008000;
  __FLASH_segment_start__ = 0x00000000;
  __FLASH_segment_end__ = 0x00080000;

  __STACKSIZE__ = 8192;
  __STACKSIZE_IRQ__ = 256;
  __STACKSIZE_FIQ__ = 4;
  __STACKSIZE_SVC__ = 4;
  __STACKSIZE_ABT__ = 4;
  __STACKSIZE_UND__ = 4;
  __HEAPSIZE__ = 0;

  __vectors_ram_load_start__ = __SRAM_segment_start__;
  .vectors_ram __SRAM_segment_start__ (NOLOAD) :
  {
    __vectors_ram_start__ = .;
    *(.vectors_ram)
    . = MAX(__vectors_ram_start__ + 0x3C , .);
  }
  __vectors_ram_end__ = __vectors_ram_start__ + SIZEOF(.vectors_ram);

  . = ASSERT(__vectors_ram_end__ >= __SRAM_segment_start__ && __vectors_ram_end__ <= (__SRAM_segment_start__ + 0x00008000) , "error: .vectors_ram is too large to fit in SRAM memory segment");

  __vectors_load_start__ = __FLASH_segment_start__;
  .vectors __FLASH_segment_start__ :
  {
    __vectors_start__ = .;
    *(.vectors .vectors.*)
  }
  __vectors_end__ = __vectors_start__ + SIZEOF(.vectors);

  . = ASSERT(__vectors_end__ >= __FLASH_segment_start__ && __vectors_end__ <= (__FLASH_segment_start__ + 0x00080000) , "error: .vectors is too large to fit in FLASH memory segment");

  __init_load_start__ = ALIGN(__vectors_end__ , 4);
  .init ALIGN(__vectors_end__ , 4) :
  {
    __init_start__ = .;
    *(.init .init.*)
  }
  __init_end__ = __init_start__ + SIZEOF(.init);

  . = ASSERT(__init_end__ >= __FLASH_segment_start__ && __init_end__ <= (__FLASH_segment_start__ + 0x00080000) , "error: .init is too large to fit in FLASH memory segment");

  __text_load_start__ = ALIGN(__init_end__ , 4);
  .text ALIGN(__init_end__ , 4) :
  {
    __text_start__ = .;
    *(.text .text.* .glue_7t .glue_7 .gnu.linkonce.t.*)
  }
  __text_end__ = __text_start__ + SIZEOF(.text);

  . = ASSERT(__text_end__ >= __FLASH_segment_start__ && __text_end__ <= (__FLASH_segment_start__ + 0x00080000) , "error: .text is too large to fit in FLASH memory segment");

  __dtors_load_start__ = ALIGN(__text_end__ , 4);
  .dtors ALIGN(__text_end__ , 4) :
  {
    __dtors_start__ = .;
    KEEP (*(SORT(.dtors.*))) KEEP (*(.dtors))
  }
  __dtors_end__ = __dtors_start__ + SIZEOF(.dtors);

  . = ASSERT(__dtors_end__ >= __FLASH_segment_start__ && __dtors_end__ <= (__FLASH_segment_start__ + 0x00080000) , "error: .dtors is too large to fit in FLASH memory segment");

  __ctors_load_start__ = ALIGN(__dtors_end__ , 4);
  .ctors ALIGN(__dtors_end__ , 4) :
  {
    __ctors_start__ = .;
    KEEP (*(SORT(.ctors.*))) KEEP (*(.ctors))
  }
  __ctors_end__ = __ctors_start__ + SIZEOF(.ctors);

  . = ASSERT(__ctors_end__ >= __FLASH_segment_start__ && __ctors_end__ <= (__FLASH_segment_start__ + 0x00080000) , "error: .ctors is too large to fit in FLASH memory segment");

  __rodata_load_start__ = ALIGN(__ctors_end__ , 4);
  .rodata ALIGN(__ctors_end__ , 4) :
  {
    __rodata_start__ = .;
    *(.rodata .rodata.* .gnu.linkonce.r.*)
  }
  __rodata_end__ = __rodata_start__ + SIZEOF(.rodata);

  . = ASSERT(__rodata_end__ >= __FLASH_segment_start__ && __rodata_end__ <= (__FLASH_segment_start__ + 0x00080000) , "error: .rodata is too large to fit in FLASH memory segment");

  __fast_load_start__ = ALIGN(__rodata_end__ , 4);
  .fast __vectors_ram_end__ : AT(ALIGN(__rodata_end__ , 4))
  {
    __fast_start__ = .;
    *(.fast .fast.*)
  }
  __fast_end__ = __fast_start__ + SIZEOF(.fast);

  __fast_load_end__ = __fast_load_start__ + SIZEOF(.fast);

  . = ASSERT((__fast_load_start__ + SIZEOF(.fast)) >= __FLASH_segment_start__ && (__fast_load_start__ + SIZEOF(.fast)) <= (__FLASH_segment_start__ + 0x00080000) , "error: .fast is too large to fit in FLASH memory segment");

  .fast_run ALIGN(__vectors_ram_end__ , 4) (NOLOAD) :
  {
    __fast_run_start__ = .;
    . = MAX(__fast_run_start__ + SIZEOF(.fast), .);
  }
  __fast_run_end__ = __fast_run_start__ + SIZEOF(.fast_run);

  . = ASSERT(__fast_run_end__ >= __SRAM_segment_start__ && __fast_run_end__ <= (__SRAM_segment_start__ + 0x00008000) , "error: .fast_run is too large to fit in SRAM memory segment");

  __data_load_start__ = ALIGN(__fast_load_start__ + SIZEOF(.fast) , 4);
  .data __fast_run_end__ : AT(ALIGN(__fast_load_start__ + SIZEOF(.fast) , 4))
  {
    __data_start__ = .;
    *(.data .data.* .gnu.linkonce.d.*)
  }
  __data_end__ = __data_start__ + SIZEOF(.data);

  __data_load_end__ = __data_load_start__ + SIZEOF(.data);

  __FLASH_segment_used_end__ = ALIGN(__fast_load_start__ + SIZEOF(.fast) , 4) + SIZEOF(.data);

  . = ASSERT((__data_load_start__ + SIZEOF(.data)) >= __FLASH_segment_start__ && (__data_load_start__ + SIZEOF(.data)) <= (__FLASH_segment_start__ + 0x00080000) , "error: .data is too large to fit in FLASH memory segment");

  .data_run ALIGN(__fast_run_end__ , 4) (NOLOAD) :
  {
    __data_run_start__ = .;
    . = MAX(__data_run_start__ + SIZEOF(.data), .);
  }
  __data_run_end__ = __data_run_start__ + SIZEOF(.data_run);

  . = ASSERT(__data_run_end__ >= __SRAM_segment_start__ && __data_run_end__ <= (__SRAM_segment_start__ + 0x00008000) , "error: .data_run is too large to fit in SRAM memory segment");

  __bss_load_start__ = ALIGN(__data_run_end__ , 4);
  .bss ALIGN(__data_run_end__ , 4) (NOLOAD) :
  {
    __bss_start__ = .;
    *(.bss .bss.* .gnu.linkonce.b.*) *(COMMON)
  }
  __bss_end__ = __bss_start__ + SIZEOF(.bss);

  . = ASSERT(__bss_end__ >= __SRAM_segment_start__ && __bss_end__ <= (__SRAM_segment_start__ + 0x00008000) , "error: .bss is too large to fit in SRAM memory segment");

  __non_init_load_start__ = ALIGN(__bss_end__ , 4);
  .non_init ALIGN(__bss_end__ , 4) (NOLOAD) :
  {
    __non_init_start__ = .;
    *(.non_init .non_init.*)
  }
  __non_init_end__ = __non_init_start__ + SIZEOF(.non_init);

  . = ASSERT(__non_init_end__ >= __SRAM_segment_start__ && __non_init_end__ <= (__SRAM_segment_start__ + 0x00008000) , "error: .non_init is too large to fit in SRAM memory segment");

  __heap_load_start__ = ALIGN(__non_init_end__ , 4);
  .heap ALIGN(__non_init_end__ , 4) (NOLOAD) :
  {
    __heap_start__ = .;
    *(.heap)
    . = MAX(__heap_start__ + __HEAPSIZE__ , .);
  }
  __heap_end__ = __heap_start__ + SIZEOF(.heap);

  . = ASSERT(__heap_end__ >= __SRAM_segment_start__ && __heap_end__ <= (__SRAM_segment_start__ + 0x00008000) , "error: .heap is too large to fit in SRAM memory segment");

  __stack_irq_load_start__ = ALIGN(__heap_end__ , 4);
  .stack_irq ALIGN(__heap_end__ , 4) (NOLOAD) :
  {
    __stack_irq_start__ = .;
    *(.stack_irq)
    . = MAX(__stack_irq_start__ + __STACKSIZE_IRQ__ , .);
  }
  __stack_irq_end__ = __stack_irq_start__ + SIZEOF(.stack_irq);

  . = ASSERT(__stack_irq_end__ >= __SRAM_segment_start__ && __stack_irq_end__ <= (__SRAM_segment_start__ + 0x00008000) , "error: .stack_irq is too large to fit in SRAM memory segment");

  __stack_fiq_load_start__ = ALIGN(__stack_irq_end__ , 4);
  .stack_fiq ALIGN(__stack_irq_end__ , 4) (NOLOAD) :
  {
    __stack_fiq_start__ = .;
    *(.stack_fiq)
    . = MAX(__stack_fiq_start__ + __STACKSIZE_FIQ__ , .);
  }
  __stack_fiq_end__ = __stack_fiq_start__ + SIZEOF(.stack_fiq);

  . = ASSERT(__stack_fiq_end__ >= __SRAM_segment_start__ && __stack_fiq_end__ <= (__SRAM_segment_start__ + 0x00008000) , "error: .stack_fiq is too large to fit in SRAM memory segment");

  __stack_svc_load_start__ = ALIGN(__stack_fiq_end__ , 4);
  .stack_svc ALIGN(__stack_fiq_end__ , 4) (NOLOAD) :
  {
    __stack_svc_start__ = .;
    *(.stack_svc)
    . = MAX(__stack_svc_start__ + __STACKSIZE_SVC__ , .);
  }
  __stack_svc_end__ = __stack_svc_start__ + SIZEOF(.stack_svc);

  . = ASSERT(__stack_svc_end__ >= __SRAM_segment_start__ && __stack_svc_end__ <= (__SRAM_segment_start__ + 0x00008000) , "error: .stack_svc is too large to fit in SRAM memory segment");

  __stack_abt_load_start__ = ALIGN(__stack_svc_end__ , 4);
  .stack_abt ALIGN(__stack_svc_end__ , 4) (NOLOAD) :
  {
    __stack_abt_start__ = .;
    *(.stack_abt)
    . = MAX(__stack_abt_start__ + __STACKSIZE_ABT__ , .);
  }
  __stack_abt_end__ = __stack_abt_start__ + SIZEOF(.stack_abt);

  . = ASSERT(__stack_abt_end__ >= __SRAM_segment_start__ && __stack_abt_end__ <= (__SRAM_segment_start__ + 0x00008000) , "error: .stack_abt is too large to fit in SRAM memory segment");

  __stack_und_load_start__ = ALIGN(__stack_abt_end__ , 4);
  .stack_und ALIGN(__stack_abt_end__ , 4) (NOLOAD) :
  {
    __stack_und_start__ = .;
    *(.stack_und)
    . = MAX(__stack_und_start__ + __STACKSIZE_UND__ , .);
  }
  __stack_und_end__ = __stack_und_start__ + SIZEOF(.stack_und);

  __SRAM_segment_used_end__ = ALIGN(__stack_abt_end__ , 4) + SIZEOF(.stack_und);

  . = ASSERT(__stack_und_end__ >= __SRAM_segment_start__ && __stack_und_end__ <= (__SRAM_segment_start__ + 0x00008000) , "error: .stack_und is too large to fit in SRAM memory segment");

  __stack_load_start__ = ALIGN(__USB_DMA_RAM_segment_start__ , 4);
  .stack ALIGN(__USB_DMA_RAM_segment_start__ , 4) (NOLOAD) :
  {
    __stack_start__ = .;
    *(.stack)
    . = MAX(__stack_start__ + __STACKSIZE__ , .);
  }
  __stack_end__ = __stack_start__ + SIZEOF(.stack);

  __USB_DMA_RAM_segment_used_end__ = ALIGN(__USB_DMA_RAM_segment_start__ , 4) + SIZEOF(.stack);

  . = ASSERT(__stack_end__ >= __USB_DMA_RAM_segment_start__ && __stack_end__ <= (__USB_DMA_RAM_segment_start__ + 0x00002000) , "error: .stack is too large to fit in USB_DMA_RAM memory segment");

}

