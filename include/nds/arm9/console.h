/*---------------------------------------------------------------------------------
	$Id: console.h,v 1.6 2005-09-12 06:50:23 wntrmute Exp $

	console functions

	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.

	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:

	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
		distribution.

	$Log: not supported by cvs2svn $
	Revision 1.5  2005/09/04 00:24:44  wntrmute
	exposed consoleSetPos
	move iprintAt and printAt to separate files

	Revision 1.4  2005/08/31 01:10:33  wntrmute
	reworked console into stdio

	Revision 1.3  2005/08/23 17:06:10  wntrmute
	converted all endings to unix

	Revision 1.2  2005/08/01 23:18:22  wntrmute
	adjusted headers for logging


---------------------------------------------------------------------------------*/
#ifndef CONSOLE_H
#define CONSOLE_H

#define CONSOLE_USE_COLOR255 16

#ifdef __cplusplus
extern "C" {
#endif

void consoleInit(u16* font, u16* charBase, u16 numCharacters, u8 charStart, u16* map, u8 pal, u8 bitDepth);
void consoleInitDefault(u16* map, u16* charBase, u8 bitDepth);

#ifdef __cplusplus
}
#endif

#endif
