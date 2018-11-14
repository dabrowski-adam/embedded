///////////////////////////////////////////////////////////////////////
//	Sample Code NOTES
//	NXP Semiconductors
///////////////////////////////////////////////////////////////////////

This code was tested on the LPC2138 (512kb Flash).
Each project is configured to use the 512kb flash; however, since the
size of each project will fit in the smallest of the LPC213x/4x series
this should not cause any issues. Using a different MCU might require
a change in the sample code.

Each project is has it's own subfolder and is labeled as such (ADC, I2C,
DAC, etc...). Since some projects require some common code, an
additional subfolder called "Common" is used.

In each folder there resides a "RAM" and "FLASH" folder, as well as a
<ProjectName>.ini file. The RAM and FLASH folders contain the binaries
needed to load the project into the MCU's RAM and FLASH respectively.

The .ini file located in each project folder is required for the RAM
project to load properly. The .ini file will perform four items:
	1. It loads the <ProjectName>.axf file into the MCU's RAM
	2. It sets the Program counter to the start address in RAM
	3. It sets the MEMMAP register to remap interrupts to RAM.
	4. Goto main

\Directory Root
|--Readme.txt
|--.\Common\
   |--.\inc\
   |--.\src\
|--.\ADC\
   |--ADC.ini
   |--.\Flash\
   |--.\RAM\
|--.\I2C\
   |--I2C.ini
   |--.\Flash\
   |--.\RAM\
|--.\DAC\
   |--DAC.ini
   |--.\Flash\
   |--.\RAM\
|
etc...
