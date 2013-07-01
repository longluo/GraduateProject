/*
 *  Copyright 2006 by Spectrum Digital Incorporated.
 *  All rights reserved. Property of Spectrum Digital Incorporated.
 */

/*
 *  Linker command file
 *
 */

-l rts64plus.lib
-l evmdm6437bsl.lib

-stack          0x00001000      /* Stack Size */
-heap           0x00001000      /* Heap Size */

MEMORY
{
    L2RAM:      o = 0x10800000  l = 0x00020000
    DDR2:       o = 0x80000000  l = 0x10000000
}

SECTIONS
{
    .bss        >   L2RAM
    .cinit      >   L2RAM
    .cio        >   L2RAM
    .const      >   L2RAM
    .data       >   L2RAM
    .far        >   L2RAM
    .stack      >   L2RAM
    .switch     >   L2RAM
    .sysmem     >   L2RAM
    .text       >   L2RAM
    .ddr2       >   DDR2
}
