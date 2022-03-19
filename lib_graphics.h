//! @file lib_graphics.h

/*
 * lib_graphics.h
 *
*  Created on: Mar 10, 2022
 *      Author: micahbly
 */

#ifndef LIB_GRAPHICS_H_
#define LIB_GRAPHICS_H_


/* about this library: Graphics
 *
 * This provides functionality for accessing the VICKY's bitmaps, sprites, etc.
 *
 *** things this library needs to be able to do
 * initialize/enter graphics mode
 * exit graphics mode
 * fill an entire screen of bitmap
 * fill a rect
 * draw a rect
 * allocate a bitmap
 * copy a bitmap
 * load a bitmap from disk
 * draw a circle
 * fill an enclosed area
 * draw a round rect
 * draw a line
 * get the value of a pixel, from specified x/y coords
 * set the value of a pixel, from specified x/y coords
 * copy a rect of pixel mem from one bitmap to another
 * copy a rect of pixel mem from place to place within the same bitmap
 *
 * STRETCH GOALS
 * load a graphical (proportional width or fixed width) font from disk or memory
 * draw string using graphical font on screen, at specified x/y
 * draw string using graphical font on screen, wrapping and fitting to specified rectangle
 *
 * SUPER STRETCH GOALS
 * have a clipping system that prevents drawing to non-clipped parts of the screen
 * have a layers system that prevents drawing to portions of layers that are under other layers
 * 
 */


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes

// A2560 includes
#include <mcp/syscalls.h>
#include <mb/a2560_platform.h>
#include <mb/lib_general.h>
#include <mb/lib_text.h>


/*****************************************************************************/
/*                            Macro Definitions                              */
/*****************************************************************************/

#define PARAM_DRAW_NE		true	//!< for Graphics_DrawCircleQuadrants, do draw NE quadrant
#define PARAM_DRAW_SE		true	//!< for Graphics_DrawCircleQuadrants, do draw SE quadrant
#define PARAM_DRAW_SW		true	//!< for Graphics_DrawCircleQuadrants, do draw SW quadrant
#define PARAM_DRAW_NW		true	//!< for Graphics_DrawCircleQuadrants, do draw NW quadrant
#define PARAM_SKIP_NE		false	//!< for Graphics_DrawCircleQuadrants, do NOT draw NE quadrant
#define PARAM_SKIP_SE		false	//!< for Graphics_DrawCircleQuadrants, do NOT draw SE quadrant
#define PARAM_SKIP_SW		false	//!< for Graphics_DrawCircleQuadrants, do NOT draw SW quadrant
#define PARAM_SKIP_NW		false	//!< for Graphics_DrawCircleQuadrants, do NOT draw NW quadrant

#define PARAM_DO_FILL		true	//!< for various graphic routines
#define PARAM_DO_NOT_FILL	false	//!< for various graphic routines

/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/



/*****************************************************************************/
/*                                 Structs                                   */
/*****************************************************************************/

typedef struct Bitmap
{
	unsigned char*	addr_;		//!< address of the start of the bitmap, within the machine's global address space. This is not the VICKY's local address for this bitmap. This address MUST be within the VRAM, however, it cannot be in non-VRAM memory space.
	signed int		width_;		//!< width of the bitmap in pixels
	signed int		height_;	//!< height of the bitmap in pixels
	signed int		x_;			//!< H position within this bitmap, of the "pen", for functions that draw from that point
	signed int		y_;			//!< V position within this bitmap, of the "pen", for functions that draw from that point
	uint8_t			color_;		//!< color value to use for next "pen" based operation in this bitmap
	Font*			font_;		//!< the currently selected font. All text drawing activities will use this font face.
} Bitmap;


/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/


/*****************************************************************************/
/*                       Public Function Prototypes                          */
/*****************************************************************************/


// ** NOTE: there is no destructor or constructor for this library, as it does not track any allocated memory.


// **** Block copy functions ****

//! Blit from source bitmap to distination bitmap. 
//! The source and destination bitmaps can be the same: you can use this to copy a chunk of pixels from one part of a screen to another. If the destination location cannot fit the entirety of the copied rectangle, the copy will be truncated, but will not return an error. 
//! @param src_bm: the source bitmap. It must have a valid address within the VRAM memory space.
//! @param dst_bm: the destination bitmap. It must have a valid address within the VRAM memory space. It can be the same bitmap as the source.
//! @param src_x, src_y: the upper left coordinate within the source bitmap, for the rectangle you want to copy. May be negative.
//! @param dst_x, dst_y: the location within the destination bitmap to copy pixels to. May be negative.
//! @param width, height: the scope of the copy, in pixels.
boolean Graphics_BlitBitMap(Screen* the_screen, Bitmap* src_bm, int src_x, int src_y, Bitmap* dst_bm, int dst_x, int dst_y, int width, int height);


// **** Block fill functions ****

// Fill graphics memory with specified value
// calling function must validate the screen ID before passing!
//! @return	returns false on any error/invalid input.
boolean Graphics_FillMemory(Screen* the_screen, unsigned char the_color);

// Fill pixel values for a specific box area
// calling function must validate screen id, coords!
//! @return	returns false on any error/invalid input.
boolean Graphics_FillBox(Screen* the_screen, signed int x, signed int y, signed int width, signed int height, unsigned char the_color);




// **** Bitmap functions *****

//! Set the "pen" color
//! This is the color that the next pen-based graphics function will use
//! This only affects functions that use the pen: any graphics function that specifies a color will use that instead
boolean Bitmap_SetCurrentColor(Bitmap* the_bitmap, uint8_t the_color);

//! Set the "pen" position
//! This is the location that the next pen-based graphics function will use for a starting location
//! This only affects functions that use the pen: any graphics function that specifies an X, Y coordinate will use that instead
boolean Bitmap_SetCurrentXY(Bitmap* the_bitmap, signed int x, signed int y);

//! Get the current color of the pen
//! @return returns 0 on any error
uint8_t Bitmap_GetCurrentColor(Bitmap* the_bitmap);

//! Get the current X position of the pen
//! @return returns -1 on any error
signed int Bitmap_GetCurrentX(Bitmap* the_bitmap);

//! Get the current Y position of the pen
//! @return returns -1 on any error
signed int Bitmap_GetCurrentY(Bitmap* the_bitmap);

//! Calculate the VRAM location of the specified coordinate within the bitmap
unsigned char* Bitmap_GetMemLocForXY(Bitmap* the_bitmap, signed int x, signed int y);

//! Calculate the VRAM location of the current coordinate within the bitmap
unsigned char* Bitmap_GetCurrentMemLoc(Bitmap* the_bitmap);




// **** Set pixel functions *****


//! Set a char at a specified x, y coord
//! @param	the_color: a 1-byte index to the current LUT
//! @return	returns false on any error/invalid input.
boolean Graphics_SetPixelAtXY(Screen* the_screen, signed int x, signed int y, unsigned char the_color);



// **** Get pixel functions *****


//! Get the char at a specified x, y coord
//! @return	returns a character code
unsigned char Graphics_GetPixelAtXY(Screen* the_screen, signed int x, signed int y);



// **** Drawing functions *****


//! Draws a line between 2 passed coordinates.
//! Use for any line that is not perfectly vertical or perfectly horizontal
//! Based on http://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C. Used in C128 Lich King. 
boolean Graphics_DrawLine(Screen* the_screen, signed int x1, signed int y1, signed int x2, signed int y2, unsigned char the_color);

//! Draws a horizontal line from specified coords, for n pixels, using the specified pixel value
//! @param	the_color: a 1-byte index to the current LUT
//! @return	returns false on any error/invalid input.
boolean Graphics_DrawHLine(Screen* the_screen, signed int x, signed int y, signed int the_line_len, unsigned char the_color);

//! Draws a vertical line from specified coords, for n pixels
//! @param	the_color: a 1-byte index to the current LUT
//! @return	returns false on any error/invalid input.
boolean Graphics_DrawVLine(Screen* the_screen, signed int x, signed int y, signed int the_line_len, unsigned char the_color);

//! Draws a rectangle based on 2 sets of coords, using the specified LUT value
//! @param	the_color: a 1-byte index to the current LUT
//! @return	returns false on any error/invalid input.
boolean Graphics_DrawBoxCoords(Screen* the_screen, signed int x1, signed int y1, signed int x2, signed int y2, unsigned char the_color);

//! Draws a rectangle based on start coords and width/height, and optionally fills the rectangle.
//! @param	width: width, in pixels, of the rectangle to be drawn
//! @param	height: height, in pixels, of the rectangle to be drawn
//! @param	the_color: a 1-byte index to the current LUT
//! @param	do_fill: If true, the box will be filled with the provided color. If false, the box will only draw the outline.
//! @return	returns false on any error/invalid input.
boolean Graphics_DrawBox(Screen* the_screen, signed int x, signed int y, signed int width, signed int height, unsigned char the_color, boolean do_fill);

//! Draws a rounded rectangle with the specified size and radius, and optionally fills the rectangle.
//! @param	width: width, in pixels, of the rectangle to be drawn
//! @param	height: height, in pixels, of the rectangle to be drawn
//! @param	radius: radius, in pixels, of the arc to be applied to the rectangle's corners. Minimum 3, maximum 20.
//! @param	the_color: a 1-byte index to the current color LUT
//! @param	do_fill: If true, the box will be filled with the provided color. If false, the box will only draw the outline.
//! @return	returns false on any error/invalid input.
boolean Graphics_DrawRoundBox(Screen* the_screen, signed int x, signed int y, signed int width, signed int height, signed int radius, unsigned char the_color, boolean do_fill);

//! Draw a circle
//! Based on http://rosettacode.org/wiki/Bitmap/Midpoint_circle_algorithm#C
boolean Graphics_DrawCircle(Screen* the_screen, signed int x1, signed int y1, signed int radius, unsigned char the_color);




// **** Screen mode/resolution/size functions *****


//! Switch machine into graphics mode
boolean Graphics_SetModeGraphics(Screen* the_screen);

//! Switch machine into text mode
//! @param as_overlay: If true, sets text overlay mode (text over graphics). If false, sets full text mode (no graphics);
boolean Graphics_SetModeText(Screen* the_screen, boolean as_overlay);



//
// Discord, 3022/03/10
//
// PJW
// Generally speaking (the order does not really matter on this):
// 1. Set the mode bits in the master control register (GRAPHICS + BITMAP), as well as the resolution.
// 2. Load the color table for the bitmap into one of the graphics LUTs (start at $B4:2000 on the A2560U, $FEC8:2000 on the A2560K)
// 3. Load the bitmap pixel data into video RAM (starts at $C0:0000 on the A2560U, $0080:0000 on the A2560K)
// 4. Set the enable bit and LUT # in the bitmap control register for the layer you want
// 5. Set the address of the pixel data in video RAM in the VRAM address pointer for the layer you want. This must be set relative to the beginning of VRAM, so if you're using the A2560U and the pixel data starts at $C2:0000, you'd store $02:0000 in the address register.
// Paul Scott Robson — Today at 12:45 PM
// TextInitialise:
//         clr.l     d0
//         lea     vicky3,a0                     ; start register writes
//         move.l     #$0000000C,(a0)             ; graphics mode & bitmap on.
//         move.l     d0,$4(a0)                     ; zero border size/scroll/colour
//         move.l     d0,$8(a0)                     ; clear border colour
//         move.l     d0,$C(a0)
//         move.l     d0,$10(a0)                     ; cursor off
//         move.l     d0,$14(a0)                     ; no line interrupts
//         move.l     d0,$18(a0)
// 
//         move.l     #$00000001,$100(a0)         ; LUT 0, BMP 1 on
//         move.l     d0,$104(a0)                 ; BMP 1 address
//         move.l     d0,$108(a0)                    ; BMP 2 off
// 
//         move.l     #$0000FF00,$2004(a0)         ; colour 1 red
//         move.l     #$00FF00FF,$2008(a0)         ; colour 2 green
//         move.l     #$00FFFFFF,$200C(a0)         ; colour 3 yellow
//         move.l     #$FF0000FF,$2010(a0)         ; colour 4 blue
//         move.l     #$FF00FFFF,$2014(a0)         ; colour 5 magenta
//         move.l     #$FFFF00FF,$2018(a0)         ; colour 6 cyan
//         move.l     #$FFFFFFFF,$201C(a0)         ; colour 7 white
// 
//         rts
// Bit Q&D but I think it's right

#endif /* LIB_GRAPHICS_H_ */


