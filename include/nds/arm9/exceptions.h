/*---------------------------------------------------------------------------------
  $Id: exceptions.h,v 1.1 2006-06-18 21:16:26 wntrmute Exp $

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

---------------------------------------------------------------------------------*/
#ifndef _exceptionTest_h_
#define _exceptionTest_h_
//---------------------------------------------------------------------------------

#define EXCEPTION_VECTOR	(*(VoidFunctionPointer *)(0x27FFD9C))


extern unsigned long exceptionC ;
extern unsigned long exceptionStack ;
extern unsigned long exceptionRegisters[];

void enterException(void);
void setExceptionHandler(VoidFunctionPointer handler);
void defaultExceptionHandler();

u32 getCPSR();

//---------------------------------------------------------------------------------
#endif // _ExceptionTest_h_
//---------------------------------------------------------------------------------
