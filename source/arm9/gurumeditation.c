/*---------------------------------------------------------------------------------
  $Id: gurumeditation.c,v 1.1 2006-06-19 14:09:37 wntrmute Exp $

  Copyright (C) 2005
  	Dave Murphy (WinterMute)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you
     must not claim that you wrote the original software. If you use
     this software in a product, an acknowledgment in the product
     documentation would be appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and
     must not be misrepresented as being the original software.
  3. This notice may not be removed or altered from any source
     distribution.

  $Log: not supported by cvs2svn $
  Revision 1.1  2006/06/18 21:16:26  wntrmute
  added arm9 exception handler API


---------------------------------------------------------------------------------*/

#include <nds/jtypes.h>
#include <nds/arm9/video.h>
#include <nds/arm9/console.h>
#include <nds/memory.h>
#include <nds/arm9/exceptions.h>
#include <stdio.h>


static const char *registerNames[] =
	{	"r0","r1","r2","r3","r4","r5","r6","r7",
		"r8 ","r9 ","r10","r11","r12","sp ","lr ","pc " };

//---------------------------------------------------------------------------------
unsigned long ARMShift(unsigned long value,unsigned char shift) {
//---------------------------------------------------------------------------------
	// no shift at all
	if (shift == 0x0B) return value ;
	int index ;
	if (shift & 0x01) {
		// shift index is a register
		index = exceptionRegisters[(shift >> 4) & 0x0F];
	} else {
		// constant shift index
		index = ((shift >> 3) & 0x1F) ;
	} ;
	int i ;
	bool isN ;
	switch (shift & 0x06) {
		case 0x00:
			// logical left
			return (value << index) ;
		case 0x02:
			// logical right
			return (value >> index) ;
		case 0x04:
			// arithmetical right
			isN = (value & 0x80000000) ;
			value = value >> index ;
			if (isN) {
				for (i=31;i>31-index;i--) {
					value = value | (1 << i) ;
				} ;
			} ;
			return value ;
		case 0x06:
			// rotate right
			index = index & 0x1F;
			value = (value >> index) | (value << (32-index));
			return value;
	};
	return value;
}


//---------------------------------------------------------------------------------
u32 getExceptionAddress( u32 opcodeAddress, u32 thumbState) {
//---------------------------------------------------------------------------------

	int Rf, Rb, Rd, Rn, Rm;

	if (thumbState) {
		// Thumb

		unsigned short opcode = *(unsigned short *)opcodeAddress ;
		// ldr r,[pc,###]			01001ddd ffffffff
		// ldr r,[r,r]				0101xx0f ffbbbddd
		// ldrsh					0101xx1f ffbbbddd
		// ldr r,[r,imm]			011xxfff ffbbbddd
		// ldrh						1000xfff ffbbbddd
		// ldr r,[sp,###]			1001xddd ffffffff
		// push						1011x10l llllllll
		// ldm						1100xbbb llllllll


		if ((opcode & 0xF800) == 0x4800) {
			// ldr r,[pc,###]

		} else if ((opcode & 0xF200) == 0x5000) {
			// ldr r,[r,r]
			Rb = (opcode >> 3) & 0x07 ;
			Rf = (opcode >> 6) & 0x07 ;
			return exceptionRegisters[Rb] + exceptionRegisters[Rf];

		} else if ((opcode & 0xF200) == 0x5200) {
			// ldrsh

		} else if ((opcode & 0xE000) == 0x6000) {
			// ldr r,[r,imm]
			Rb = (opcode >> 3) & 0x07;
			Rf = (opcode >> 6) & 0x1F ;
			return exceptionRegisters[Rb] + (Rf << 2);
		} else if ((opcode & 0xF000) == 0x8000) {
			// ldrh
			Rb = (opcode >> 3) & 0x07 ;
			Rf = (opcode >> 6) & 0x1F ;
			return exceptionRegisters[Rb] + (Rf << 2);
		} else if ((opcode & 0xF000) == 0x9000) {
			// ldr r,[sp,#imm]
		} else if ((opcode & 0xF700) == 0xB500) {
			// push/pop
		} else if ((opcode & 0xF000) == 0xC000) {
			// ldm/stm
		}
	} else {
		// arm32
		unsigned long opcode = *(unsigned long *)opcodeAddress ;

		// SWP			xxxx0001 0x00nnnn dddd0000 1001mmmm
		// STR/LDR		xxxx01xx xxxxnnnn ddddffff ffffffff
		// STRH/LDRH	xxxx000x x0xxnnnn dddd0000 1xx1mmmm
		// STRH/LDRH	xxxx000x x1xxnnnn ddddffff 1xx1ffff
		// STM/LDM		xxxx100x xxxxnnnn llllllll llllllll

		if ((opcode & 0x0FB00FF0) == 0x01000090) {
			// SWP
			Rn = (opcode >> 16) & 0x0F;
			return exceptionRegisters[Rn];
		} else if ((opcode & 0x0C000000) == 0x04000000) {
			// STR/LDR
			Rn = (opcode >> 16) & 0x0F;
			if (opcode & 0x02000000) {
				// Register offset
				Rm = opcode & 0x0F;
				if (opcode & 0x01000000) {
					unsigned short shift = (unsigned short)((opcode >> 4) & 0xFF) ;
					// pre indexing
					long Offset = ARMShift(exceptionRegisters[Rm],shift);
					// add or sub the offset depending on the U-Bit
					return exceptionRegisters[Rn] + ((opcode & 0x00800000)?Offset:-Offset);
				} else {
					// post indexing
					return exceptionRegisters[Rn];
				}
			} else {
				// Immediate offset
				unsigned long Offset = (opcode & 0xFFF) ;
				if (opcode & 0x01000000) {
					// pre indexing
					// add or sub the offset depending on the U-Bit
					return exceptionRegisters[Rn] + ((opcode & 0x00800000)?Offset:-Offset);
				} else {
					// post indexing
					return exceptionRegisters[Rn];
				}
			}
		} else if ((opcode & 0x0E400F90) == 0x00000090) {
			// LDRH/STRH with register Rm
			Rn = (opcode >> 16) & 0x0F;
			Rd = (opcode >> 12) & 0x0F;
			Rm = opcode & 0x0F;
			unsigned short shift = (unsigned short)((opcode >> 4) & 0xFF);
			long Offset = ARMShift(exceptionRegisters[Rm],shift);
			// add or sub the offset depending on the U-Bit
			return exceptionRegisters[Rn] + ((opcode & 0x00800000)?Offset:-Offset);
		} else if ((opcode & 0x0E400F90) == 0x00400090) {
			// LDRH/STRH with immediate offset
			Rn = (opcode >> 16) & 0x0F;
			Rd = (opcode >> 12) & 0x0F;
			unsigned long Offset = (opcode & 0xF) | ((opcode & 0xF00)>>8) ;
			// add or sub the offset depending on the U-Bit
			return exceptionRegisters[Rn] + ((opcode & 0x00800000)?Offset:-Offset) ;
		} else if ((opcode & 0x0E000000) == 0x08000000) {
			// LDM/STM
			Rn = (opcode >> 16) & 0x0F;
		}
	}
	return 0;
}
//---------------------------------------------------------------------------------
void defaultExceptionHandler() {
//---------------------------------------------------------------------------------
	videoSetMode(0);	//not using the main screen
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);	//sub bg 0 will be used to print text
	vramSetBankC(VRAM_C_SUB_BG);

	SUB_BG0_CR = BG_MAP_BASE(31);

	BG_PALETTE_SUB[0] = RGB15(31,0,0);
	BG_PALETTE_SUB[255] = RGB15(31,31,31);	//by default font will be rendered with color 255

	//consoleInit() is a lot more flexible but this gets you up and running quick
	consoleInitDefault((u16*)SCREEN_BASE_BLOCK_SUB(31), (u16*)CHAR_BASE_BLOCK_SUB(0), 16);

	iprintf("\n\x1b[5CGuru Meditation Error!\n\n");
	u32	currentMode = getCPSR() & 0x1f;
	u32 thumbState = ((*(u32*)0x027FFD90) & 0x20);

	exceptionRegisters[15] = *(u32*)0x027FFD98;

	u32 codeAddress, exceptionAddress = 0;

	int offset = 8;

	if ( currentMode == 0x17 ) {
		iprintf ("\x1b[10Cdata abort!\n\n");
		codeAddress = (*(u32*)0x027FFD98) - offset;
		exceptionAddress = getExceptionAddress( codeAddress, thumbState);
	} else {
		if (thumbState)
			offset = 2;
		else
			offset = 4;
		iprintf("\x1b[5Cundefined instruction!\n\n");
		codeAddress = (*(u32*)0x027FFD98) - offset;
		exceptionAddress = codeAddress;
	}


	iprintf("  pc: %08X addr: %08X\n\n",codeAddress,exceptionAddress);

	int i;
	for ( i=0; i < 8; i++ ) {
		iprintf(	"  %s: %08X   %s: %08X\n",
					registerNames[i], exceptionRegisters[i],
					registerNames[i+8],exceptionRegisters[i+8]);
	}

	while(1);

}
