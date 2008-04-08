/*---------------------------------------------------------------------------------

	keyboard.h -- stdin integration for a simple keyboard

	Copyright (C) 2007
		Jason Rogers (Dovoto) 

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

---------------------------------------------------------------------------------*/
/*! \file keyboard.h
    \brief nds stdio keyboard integration.
*/

	


#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <nds/ndstypes.h>

//!KeyChangedCallback
typedef void (*KeyChangeCallback)(int key);

/**
 * \enum KeyboardState
 * \brief states the keyboard can be in, currently only Lower and Upper supported
 */
typedef enum
{
	Lower, /*!< Normal keyboard display (lowercase letters) >*/
	Upper, /*!< Capslock Held >*/
	Numeric, /*!< Numeric only keypad (not provided by the default keyboard) >*/
	Reduced /*!< Reduced footprint keyboard (not provided by the default keyboard) >*/
}KeyboardState;

/**
 * \struct KeyMap
 * \brief defines a key mapping
 */
typedef struct 
{
   const u16* mapDataPressed;/*!< the map for keys pressed >*/
   const u16* mapDataReleased;/*!< the map for keys released >*/
   const int* keymap; /*!< the lookup table for x y grid location to corresponding key >*/
   int width; /*!< width of the keyboard in grid spaces >*/
   int height; /*!< height of the keyboard in grid spaces>*/
}KeyMap;

/**
 * \struct Keyboard
 * \brief describes a keyboard
 */
typedef struct 
{
	int background;/*!< Background number to use, after init() this contains the background ID >*/
	int keyboardOnSub; /*!< boolean to determine if keyboard is on sub screen or main >*/
    int offset_x; /*!< x offset of the map, can be used to center a custom keyboard >*/
	int offset_y; /*!< y offset of the map, can be used to center a custom keyboard >*/
	int grid_width; /*!< the grid width, this size will be used to translate x coordinate of touch to x coordinate in keymap >*/
	int grid_height;/*!< the grid height, this size will be used to translate y coordinate of touch to y coordinate in keymap >*/
	KeyboardState state; /*!< the state of the keyboard>*/
    int shifted;  /*!< true if shifted>*/
    int visible; /*!< true if visible>*/
	KeyMap* lower; /*!< keymapping for lower case normal keyboard>*/
    KeyMap* upper;/*!< keymapping for shifted upper case normal keyboard>*/
    KeyMap* numeric; /*!< keymapping for numeric keypad>*/
    KeyMap* reduced; /*!< keymapping for reduced footprint keyboard>*/
	const u16* tiles; /*!< pointer to graphics tiles, cannot exceed 44KB with default base>*/
    u32 tileLen; /*!< length in bytes of graphics data>*/
    const u16* palette; /*!< pointer to the palette>*/
    u32 paletteLen; /*!< length in bytes of the palette data>*/
    int mapBase; /*!< map base to be used by the keyboard>*/
	int tileBase; /*!< tile base to be used by keyboard graphics>*/
	int tileOffset; /*!< tile offset (in bytes) to load graphics (map must be pre adjusted) >*/
	u32 scrollSpeed; /*!<keyboard scroll speed on hide and show in pixels per fram (must be possitive and 0 == instant on) */
   KeyChangeCallback OnKeyPressed; /*!< will be called on key press>*/
	KeyChangeCallback OnKeyReleased; /*!< will be called on key release >*/
}Keyboard;

#ifdef __cplusplus
extern "C" {
#endif

/*! \fn Keyboard* keyboardGetDefault(void)
	\brief Gets the default keyboard
*/
Keyboard* keyboardGetDefault(void);

/*! \fn keyboardInit(Keyboard* keyboard)
	\brief initializes the keyboard system with the supplied keyboard
*/
void keyboardInit(Keyboard* keyboard);

/*! \fn void keyboardShow(void)
	\brief Displays the keyboard
*/
void keyboardShow(void);

/*! \fn void keyboardHide(void)
	\brief Hides the keyboard
*/
void keyboardHide(void);

/*! \fn int keyboardGetKey(int x, int y)
	\brief returns the key located at the supplied x and y
	\param x the pixel x location
	\param y the pixel y location
	\return the key pressed or NOKEY if user pressed outside the keypad
*/
int keyboardGetKey(int x, int y);

/*! \fn void keyboardGetString(char * buffer, int maxLen)
	\brief reads the input until a the return key is pressed
	\param buffer a buffer to hold the input string
	\param maxLen the maximum length to read
*/
void keyboardGetString(char * buffer, int maxLen);

/*! \fn int keboardGetChar(void)
	\brief Waits for user to press a key and returns the key pressed
*/
int keboardGetChar(void);

#ifdef __cplusplus
}
#endif

#endif
