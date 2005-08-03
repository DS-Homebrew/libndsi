/*---------------------------------------------------------------------------------
	$Id: pcx.c,v 1.4 2005-08-03 05:18:06 wntrmute Exp $


  Copyright (C) 2005
			Jason Rogers (dovoto)
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
	Revision 1.3  2005/08/03 05:05:59  dovoto
	added rounding to palette conversion
	
	Revision 1.2  2005/08/01 23:18:22  wntrmute
	adjusted headers for logging
	
	Revision 1.1  2005/07/27 02:20:52  wntrmute
	add image functions

---------------------------------------------------------------------------------*/
#include <nds/jtypes.h>
#include <nds/arm9/video.h>
#include <nds/arm9/image.h>
#include <malloc.h>

//---------------------------------------------------------------------------------
int loadPCX(unsigned char* pcx, sImage* image) {
//---------------------------------------------------------------------------------
	//struct rgb {unsigned char b,g,r;};
	RGB_24* pal;
	
	PCXHeader* hdr = (PCXHeader*) pcx;

	pcx += sizeof(PCXHeader);
	
	unsigned char c;
	int size;
	int count;
	int run;
	int i;

	image->width  = hdr->xmax - hdr->xmin + 1 ;
	image->height = hdr->ymax - hdr->ymin + 1;

	size = image->width *image->height;
	
	image->data8 = (unsigned char*)malloc(size);
	image->palette = (unsigned short*)malloc(256 * 2);

	if(hdr->bitsPerPixel != 8)
		return 0;
	
	count = 0;

	while(count < size) {
		c = *pcx++;
		
		if(c < 192) {
			image->data8[count++] = c;
		} else {
			run = c - 192;
		
			c = *pcx++;
			
			for(i = 0; i < run; i++)
				image->data8[count++] = c;
		}
	}
	
	pal = (RGB_24*)(pcx + 1);

	for(i = 0; i < 256; i++)
		image->palette[i] = RGB15((pal[i].r + 4) >> 3 ,(pal[i].g + 4) >> 3 , (pal[i].b + 4) >> 3) ;
	
	return 1;
}
