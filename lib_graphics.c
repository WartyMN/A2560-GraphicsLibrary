/*
 * lib_graphics.c
 *
 *  Created on: Mar 10, 2022
 *      Author: micahbly
 */





/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/

// project includes
#include "lib_graphics.h"

// C includes
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// A2560 includes
#include <mcp/syscalls.h>
#include <mb/a2560_platform.h>
#include <mb/lib_general.h>
#include <mb/lib_text.h>


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/



/*****************************************************************************/
/*                               Enumerations                                */
/*****************************************************************************/



/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/



/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

//! \cond PRIVATE

// validate screen id, x, y, and colors
boolean Graphics_ValidateAll(Screen* the_screen, signed int x, signed int y);

// validate the coordinates are within the bounds of the specified screen
boolean Graphics_ValidateXY(Screen* the_screen, signed int x, signed int y);

// calculate the VRAM location of the specified coordinate
char* Graphics_GetMemLocForXY(Screen* the_screen, signed int x, signed int y);

//! Draw 1 to 4 quadrants of a circle
//! Only the specified quadrants will be drawn. This makes it possible to use this to make round rects, by only passing 1 quadrant.
//! Based on http://rosettacode.org/wiki/Bitmap/Midpoint_circle_algorithm#C
boolean Graphics_DrawCircleQuadrants(Screen* the_screen, signed int x1, signed int y1, signed int radius, unsigned char the_color, boolean ne, boolean se, boolean sw, boolean nw);

//! Perform a flood fill starting at the coordinate passed. 
boolean Graphics_Fill(Screen* the_screen, signed int x, signed int y, unsigned char the_color);

//! \endcond


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/

// **** NOTE: all functions in private section REQUIRE pre-validated parameters. 
// **** NEVER call these from your own functions. Always use the public interface. You have been warned!


//! \cond PRIVATE

//! Validate screen id, x, y, and colors
//! @param	fore_color: Index to the desired foreground color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @param	back_color: Index to the desired background color (0-15). The predefined macro constants may be used (COLOR_DK_RED, etc.), but be aware that the colors are not fixed, and may not correspond to the names if the LUT in RAM has been modified.
//! @return	returns false on any error/invalid input.
boolean Graphics_ValidateAll(Screen* the_screen, signed int x, signed int y)
{
	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}
			
	if (Graphics_ValidateXY(the_screen, x, y) == false)
	{
		LOG_ERR(("%s %d: illegal coordinates %li, %li", __func__, __LINE__, x, y));
		return false;
	}

	return true;
}

//! Validate the coordinates are within the bounds of the specified screen. 
boolean Graphics_ValidateXY(Screen* the_screen, signed int x, signed int y)
{
	signed int		max_row;
	signed int		max_col;
	
	max_col = the_screen->width_ - 1;
	max_row = the_screen->height_ - 1;
	
	if (x < 0 || x > max_col || y < 0 || y > max_row)
	{
		return false;
	}

	return true;
}


//! Calculate the VRAM location of the specified coordinate
char* Graphics_GetMemLocForXY(Screen* the_screen, signed int x, signed int y)
{
	char*			the_write_loc;
	unsigned long	vram0_abs_addr;
	unsigned long	vram0_vicky_addr;	// address from point of view of the vicky. what is stored in V 0x0104
	
	// LOGIC:
	//   A2560 has 2 bitmap layers, each with own VRAM address
	//   The Vicky knows the VRAM addr as a relative location to the place the VRAM exists in the system memory map
	//   VRAM buffer A starts at 0x00B0 0000
	//   If vicky bitmap layer 0 addr says "0", that's 0x00B0 0000. Any other value, add to that starting address.
	
	vram0_vicky_addr = R32(the_screen->vicky_ + BITMAP_L0_VRAM_ADDR_L);
	vram0_abs_addr = vram0_vicky_addr + VRAM_BUFFER_A;
	
	the_write_loc = (char*)vram0_abs_addr + (the_screen->width_ * y) + x;
	
	//DEBUG_OUT(("%s %d: screen=%i, x=%i, y=%i, for-attr=%i, calc=%i, loc=%p", __func__, __LINE__, (signed int)the_screen_id, x, y, for_attr, (the_screen->text_mem_cols_ * y) + x, the_write_loc));
	
	return the_write_loc;
}


//! Draw 1 to 4 quadrants of a circle
//! Only the specified quadrants will be drawn. This makes it possible to use this to make round rects, by only passing 1 quadrant.
//! NO VALIDATION PERFORMEND ON PARAMETERS. CALLING METHOD MUST VALIDATE.
//! Based on http://rosettacode.org/wiki/Bitmap/Midpoint_circle_algorithm#C
boolean Graphics_DrawCircleQuadrants(Screen* the_screen, signed int x1, signed int y1, signed int radius, unsigned char the_color, boolean ne, boolean se, boolean sw, boolean nw)
{
    int	f;
    int	ddF_x;
    int	ddF_y;
    int	x;
    int	y;
 
	f = 1 - radius;
	ddF_x = 0;
	ddF_y = -2 * radius;
	x = 0;
	y = radius;

	if (se || sw)
	{
		Graphics_SetPixelAtXY(the_screen, x1, y1 + radius, the_color);
	}
	if (ne || nw)
	{
		Graphics_SetPixelAtXY(the_screen, x1, y1 - radius, the_color);
	}
	if (se || ne)
	{
		Graphics_SetPixelAtXY(the_screen, x1 + radius, y1, the_color);
	}
	if (nw || sw)
	{
		Graphics_SetPixelAtXY(the_screen, x1 - radius, y1, the_color);
	}
 
    while(x < y) 
	{
		if(f >= 0) 
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}

		x++;
		ddF_x += 2;
		f += ddF_x + 1;  
 
 		if (se)
        {
			Graphics_SetPixelAtXY(the_screen, x1 + x, y1 + y, the_color);
			Graphics_SetPixelAtXY(the_screen, x1 + y, y1 + x, the_color);
        }
 
 		if (sw)
        {
			Graphics_SetPixelAtXY(the_screen, x1 - x, y1 + y, the_color);
			Graphics_SetPixelAtXY(the_screen, x1 - y, y1 + x, the_color);
        }
 
 		if (ne)
        {
			Graphics_SetPixelAtXY(the_screen, x1 + x, y1 - y, the_color);
			Graphics_SetPixelAtXY(the_screen, x1 + y, y1 - x, the_color);
        }
 
 		if (nw)
        {
			Graphics_SetPixelAtXY(the_screen, x1 - x, y1 - y, the_color);
			Graphics_SetPixelAtXY(the_screen, x1 - y, y1 - x, the_color);
        }
    }
    
    return true;
}


//! Perform a flood fill starting at the coordinate passed. 
//! WARNING: this function is recursive, and if applied to a size even 1/10th the size of the screen, it can eat the stack. Either do not use this, or control its usage to just situations you can control. Or set an enormous stack size when building your app.
boolean Graphics_Fill(Screen* the_screen, signed int x, signed int y, unsigned char the_color)
{   
	int		height;
	int		width;
	unsigned char	color_here;
	
	width = the_screen->width_;
	height = the_screen->height_;

//	color_here = Graphics_GetPixelAtXY(the_screen, x, y);
	
// 	DEBUG_OUT(("%s %d: x=%i, y=%i, the_color=%i, value-at-spot=%i", __func__, __LINE__, x, y, the_color, color_here));
	
    if ( 0 <= y && y < height && 0 <= x && x < width && Graphics_GetPixelAtXY(the_screen, x, y) != the_color )
    {
        Graphics_SetPixelAtXY(the_screen, x, y, the_color);
        Graphics_Fill(the_screen, x-1, y, the_color);
        Graphics_Fill(the_screen, x+1, y, the_color);
        Graphics_Fill(the_screen, x, y-1, the_color);
        Graphics_Fill(the_screen, x, y+1, the_color);
    }
//     else
//     {
// 		DEBUG_OUT(("%s %d: skipping x=%i, y=%i, the_color=%i, value-at-spot=%i", __func__, __LINE__, x, y, the_color, color_here));
//     }
    
    
    return true;
}


//! \endcond



/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/

// ** NOTE: there is no destructor or constructor for this library, as it does not track any allocated memory. It works on the basis of a screen ID, which corresponds to the text memory for Vicky's Channel A and Channel B video memory.


// **** Block copy functions ****

//! Blit from source bitmap to distination bitmap. 
//! The source and destination bitmaps can be the same: you can use this to copy a chunk of pixels from one part of a screen to another. If the destination location cannot fit the entirety of the copied rectangle, the copy will be truncated, but will not return an error. 
//! @param src_bm: the source bitmap. It must have a valid address within the VRAM memory space.
//! @param dst_bm: the destination bitmap. It must have a valid address within the VRAM memory space. It can be the same bitmap as the source.
//! @param src_x, src_y: the upper left coordinate within the source bitmap, for the rectangle you want to copy. May be negative.
//! @param dst_x, dst_y: the location within the destination bitmap to copy pixels to. May be negative.
//! @param width, height: the scope of the copy, in pixels.
boolean Graphics_BlitBitMap(Screen* the_screen, Bitmap* src_bm, int src_x, int src_y, Bitmap* dst_bm, int dst_x, int dst_y, int width, int height)
{
	unsigned char*		the_read_loc;
	unsigned char*		the_write_loc;
	int					i;
	
	// TODO: move the 3 checks below to a private common function if other blit functions are added
	
	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}

	if (src_bm == NULL || dst_bm == NULL)
	{
		LOG_ERR(("%s %d: passed source or destination bitmap was NULL", __func__, __LINE__));
		return false;
	}
	
	if (src_bm->addr_ == NULL || dst_bm->addr_ == NULL)
	{
		LOG_ERR(("%s %d: passed source or destination bitmap had a NULL address", __func__, __LINE__));
		return false;
	}
	
	// LOGIC:
	//   check if any part of the desired rectangle is within visible space in the source.
	//   We want to allow negative starting locations as long as height/width means some part of the rectange is actually in the bitmap. 
	
	if (src_x + width < 0 || src_y + height < 0)
	{
		LOG_INFO(("%s %d: No part of the copy-from rectangle was on screen. No copy performed. src_x=%i, src_y=%i, width=%i, height=%i.", __func__, __LINE__, src_x, src_y, width, height));
		return false;
	}
	
	// LOGIC:
	//   check if any part of the desired rectangle is within visible space in the target.
	//   We want to allow near-right-edge/bottom-edge starting locations as long as some part of the rectange is actually in the bitmap. 
	
	if (dst_x >= dst_bm->width_ || dst_y >= dst_bm->height_)
	{
		LOG_INFO(("%s %d: No part of the copy-to rectangle was on screen. No copy performed. dst_x=%i, dst_y=%i, width=%i, height=%i.", __func__, __LINE__, dst_x, dst_y, width, height));
		return false;
	}

	// adjust copy width/height if the whole image wouldn't fit on target screen anyway
	width = (dst_x + width >= dst_bm->width_) ? dst_bm->width_ - dst_x : width;
	height = (dst_y + height >= dst_bm->height_) ? dst_bm->height_ - dst_y : height;
	
	// adjust copy width/height if the starting x,y are offscreen
	if (src_x < 0)
	{
		width -= src_x;
		src_x = 0;
	}

	if (src_y < 0)
	{
		height -= src_y;
		src_y = 0;
	}

	//DEBUG_OUT(("%s %d: final parameters: src_x=%i, src_y=%i, dst_x=%i, dst_y=%i, width=%i, height=%i.", __func__, __LINE__, src_x, src_y, dst_x, dst_y, width, height));

	// checks complete. ready to copy. 
	the_read_loc = src_bm->addr_ + (src_bm->width_ * src_y) + src_x;
	the_write_loc = dst_bm->addr_ + (src_bm->width_ * dst_y) + dst_x;
	
	for (i = 0; i < height; i++)
	{
		memcpy(the_write_loc, the_read_loc, width);
		
		the_write_loc += dst_bm->width_;
		the_read_loc += src_bm->width_;
	}

	return true;
}


// **** Block fill functions ****


// Fill graphics memory with specified value
// calling function must validate the screen ID before passing!
//! @return	returns false on any error/invalid input.
boolean Graphics_FillMemory(Screen* the_screen, unsigned char the_color)
{
	char*			the_write_loc;
	unsigned long	the_write_len;
	
	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}

	the_write_loc = Graphics_GetMemLocForXY(the_screen, 0, 0);

	the_write_len = the_screen->width_ * the_screen->height_;
	
	memset(the_write_loc, the_color, the_write_len);

	return true;
}


//! Fill pixel values for a specific box area
//! calling function must validate screen id, coords!
//! @param	width: width, in pixels, of the rectangle to be filled
//! @param	height: height, in pixels, of the rectangle to be filled
//! @param	the_color: a 1-byte index to the current LUT
//! @return	returns false on any error/invalid input.
boolean Graphics_FillBox(Screen* the_screen, signed int x, signed int y, signed int width, signed int height, unsigned char the_color)
{
	char*			the_write_loc;
	signed int		max_row;

	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}

	//DEBUG_OUT(("%s %d: x=%i, y=%i, width=%i, height=%i, the_color=%i", __func__, __LINE__, x, y, width, height, the_color));
	
	// set up initial loc
	the_write_loc = Graphics_GetMemLocForXY(the_screen, x, y);
	
	max_row = y + height;
	
	for (; y <= max_row; y++)
	{
		memset(the_write_loc, the_color, width);
		the_write_loc += the_screen->width_;
	}
			
	return true;
}





// **** Bitmap functions *****

//! Set the "pen" color
//! This is the color that the next pen-based graphics function will use
//! This only affects functions that use the pen: any graphics function that specifies a color will use that instead
boolean Bitmap_SetCurrentColor(Bitmap* the_bitmap, uint8_t the_color)
{
	if (the_bitmap == NULL)
	{
		LOG_ERR(("%s %d: passed bitmap was NULL", __func__, __LINE__));
		return false;
	}

	the_bitmap->color_ = the_color;
	
	return true;
}


//! Set the "pen" position
//! This is the location that the next pen-based graphics function will use for a starting location
//! NOTE: you are allowed to set negative values, but not values greater than the height/width of the screen. This is to allow for functions that may have portions visible on the screen, such as a row of text that starts 2 pixels to the left of the bitmap's left edge. 
//! This only affects functions that use the pen: any graphics function that specifies an X, Y coordinate will use that instead
boolean Bitmap_SetCurrentXY(Bitmap* the_bitmap, signed int x, signed int y)
{
	if (the_bitmap == NULL)
	{
		LOG_ERR(("%s %d: passed bitmap was NULL", __func__, __LINE__));
		return false;
	}

	if (x >= the_bitmap->width_ || y >= the_bitmap->height_)
	{
		LOG_ERR(("%s %d: invalid coordinates passed (%i, %i)", __func__, __LINE__, x, y));
		return false;
	}
	
	the_bitmap->x_ = x;
	the_bitmap->y_ = y;
	
	return true;
}


//! Get the current color of the pen
//! @return returns 0 on any error
uint8_t Bitmap_GetCurrentColor(Bitmap* the_bitmap)
{
	if (the_bitmap == NULL)
	{
		LOG_ERR(("%s %d: passed bitmap was NULL", __func__, __LINE__));
		return 0;
	}

	return the_bitmap->color_;
}


//! Get the current X position of the pen
//! @return returns -1 on any error
signed int Bitmap_GetCurrentX(Bitmap* the_bitmap)
{
	if (the_bitmap == NULL)
	{
		LOG_ERR(("%s %d: passed bitmap was NULL", __func__, __LINE__));
		return -1;
	}

	return the_bitmap->x_;
}

//! Get the current Y position of the pen
//! @return returns -1 on any error
signed int Bitmap_GetCurrentY(Bitmap* the_bitmap)
{
	if (the_bitmap == NULL)
	{
		LOG_ERR(("%s %d: passed bitmap was NULL", __func__, __LINE__));
		return -1;
	}

	return the_bitmap->y_;
}


// calculate the VRAM location of the specified coordinate within the bitmap
unsigned char* Bitmap_GetMemLocForXY(Bitmap* the_bitmap, signed int x, signed int y)
{
	unsigned char*			the_write_loc;

	if (the_bitmap == NULL)
	{
		LOG_ERR(("%s %d: passed bitmap was NULL", __func__, __LINE__));
		return NULL;
	}
	
	if (the_bitmap->addr_ == NULL)
	{
		LOG_ERR(("%s %d: passed bitmap had a NULL address", __func__, __LINE__));
		return NULL;
	}
	
	// LOGIC:
	//   check that x and y are within the bitmap's coordinate box. if not, we can't calculate a memory loc for them.
	
	if (0 > x >= the_bitmap->width_ || 0 > y >= the_bitmap->height_)
	{
		LOG_ERR(("%s %d: invalid coordinates passed (%i, %i)", __func__, __LINE__, x, y));
		return NULL;
	}
	
	return the_bitmap->addr_ + (the_bitmap->width_ * y) + x;
}


// calculate the VRAM location of the current coordinate within the bitmap
unsigned char* Bitmap_GetCurrentMemLoc(Bitmap* the_bitmap)
{
	return Bitmap_GetMemLocForXY(the_bitmap, the_bitmap->x_, the_bitmap->y_);
}






// **** Set pixel functions *****


//! Set a char at a specified x, y coord
//! @param	the_color: a 1-byte index to the current LUT
//! @return	returns false on any error/invalid input.
boolean Graphics_SetPixelAtXY(Screen* the_screen, signed int x, signed int y, unsigned char the_color)
{
	char*	the_write_loc;
	
	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}

	if (!Graphics_ValidateAll(the_screen, x, y))
	{
		LOG_ERR(("%s %d: illegal screen id or coordinate", __func__, __LINE__));
		return false;
	}
	
	the_write_loc = Graphics_GetMemLocForXY(the_screen, x, y);	
 	*the_write_loc = the_color;
	
	return true;
}




// **** Get pixel functions *****


//! Get the char at a specified x, y coord
//! @return	returns a character code
unsigned char Graphics_GetPixelAtXY(Screen* the_screen, signed int x, signed int y)
{
	char*			the_read_loc;
	unsigned char	the_color;
	
	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}

	if (!Graphics_ValidateAll(the_screen, x, y))
	{
		LOG_ERR(("%s %d: illegal screen id or coordinate", __func__, __LINE__));
		return false;
	}
	
	the_read_loc = Graphics_GetMemLocForXY(the_screen, x, y);	
 	the_color = (unsigned char)*the_read_loc;
	
	return the_color;
}





// **** Drawing functions *****


//! Draws a line between 2 passed coordinates.
//! Use for any line that is not perfectly vertical or perfectly horizontal
//! Based on http://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C. Used in C128 Lich King. 
boolean Graphics_DrawLine(Screen* the_screen, signed int x1, signed int y1, signed int x2, signed int y2, unsigned char the_color)
{
	signed int dx;
	signed int sx;
	signed int dy;
	signed int sy;
	signed int err;
	signed int e2;

	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}

	if (!Graphics_ValidateAll(the_screen, x1, y1))
	{
		LOG_ERR(("%s %d: illegal screen id or coordinate", __func__, __LINE__));
		return false;
	}
	
	dx = abs(x2 - x1);
	sx = x1 < x2 ? 1 : -1;
	dy = abs(y2 - y1);
	sy = y1 < y2 ? 1 : -1;
	err = (dx > dy ? dx : -dy)/2;

	for(;;)
	{
		Graphics_SetPixelAtXY(the_screen, x1, y1, the_color);

		if (x1==x2 && y1==y2)
		{
			break;
		}

		e2 = err;

		if (e2 >-dx)
		{
			err -= dy;
			x1 += sx;
		}

		if (e2 < dy)
		{
			err += dx;
			y1 += sy;
		}
	}
	
	return true;
}

//! Draws a horizontal line from specified coords, for n pixels
//! @param	the_color: a 1-byte index to the current LUT
//! @return	returns false on any error/invalid input.
boolean Graphics_DrawHLine(Screen* the_screen, signed int x, signed int y, signed int the_line_len, unsigned char the_color)
{
	signed int		dx;
	unsigned char	the_attribute_value;
	boolean			result;
	
	// LOGIC: 
	//   an H line is just a box with 1 row, so we can re-use Graphics_FillBox(). These routines use memset, so are quicker than for loops. 

	//DEBUG_OUT(("%s %d: x=%i, y=%i, the_line_len=%i, the_color=%i", __func__, __LINE__, x, y, the_line_len, the_color));
	
	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}

	if (!Graphics_ValidateAll(the_screen, x, y))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}

	result = Graphics_FillBox(the_screen, x, y, the_line_len, 0, the_color);

	return result;
}


//! Draws a vertical line from specified coords, for n pixels
//! @param	the_color: a 1-byte index to the current LUT
//! @return	returns false on any error/invalid input.
boolean Graphics_DrawVLine(Screen* the_screen, signed int x, signed int y, signed int the_line_len, unsigned char the_color)
{
	unsigned int dy;

	//DEBUG_OUT(("%s %d: x=%i, y=%i, the_line_len=%i, the_color=%i", __func__, __LINE__, x, y, the_line_len, the_color));
	
	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}

	if (!Graphics_ValidateAll(the_screen, x, y))
	{
		LOG_ERR(("%s %d: illegal screen id or coordinate", __func__, __LINE__));
		return false;
	}
	
	for (dy = 0; dy < the_line_len; dy++)
	{
		Graphics_SetPixelAtXY(the_screen, x, y + dy, the_color);
	}
	
	return true;
}


//! Draws a rectangle based on 2 sets of coords, using the specified LUT value
//! @param	the_color: a 1-byte index to the current LUT
//! @return	returns false on any error/invalid input.
boolean Graphics_DrawBoxCoords(Screen* the_screen, signed int x1, signed int y1, signed int x2, signed int y2, unsigned char the_color)
{
	signed int	dy;
	signed int	dx;

	//DEBUG_OUT(("%s %d: x1=%i, y1=%i, x2=%i, y2=%i, the_color=%i", __func__, __LINE__, x1, y1, x2, y2, the_color));
	
	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}

	if (!Graphics_ValidateAll(the_screen, x1, y1))
	{
		LOG_ERR(("%s %d: illegal screen id or coordinate", __func__, __LINE__));
		return false;
	}
	
	if (!Graphics_ValidateXY(the_screen, x2, y2))
	{
		LOG_ERR(("%s %d: illegal coordinate", __func__, __LINE__));
		return false;
	}

	if (x1 > x2 || y1 > y2)
	{
		LOG_ERR(("%s %d: illegal coordinates", __func__, __LINE__));
		return false;
	}

	// add 1 to line len, because desired behavior is a box that connects fully to the passed coords
	dx = x2 - x1 + 1;
	dy = y2 - y1 + 1;
	
	if (!Graphics_DrawHLine(the_screen, x1, y1, dx, the_color))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	if (!Graphics_DrawVLine(the_screen, x2, y1, dy, the_color))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	if (!Graphics_DrawHLine(the_screen, x1, y2, dx, the_color))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	if (!Graphics_DrawVLine(the_screen, x1, y1, dy, the_color))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
		
	return true;
}


//! Draws a rectangle based on start coords and width/height, and optionally fills the rectangle.
//! @param	width: width, in pixels, of the rectangle to be drawn
//! @param	height: height, in pixels, of the rectangle to be drawn
//! @param	the_color: a 1-byte index to the current LUT
//! @param	do_fill: If true, the box will be filled with the provided color. If false, the box will only draw the outline.
//! @return	returns false on any error/invalid input.
boolean Graphics_DrawBox(Screen* the_screen, signed int x, signed int y, signed int width, signed int height, unsigned char the_color, boolean do_fill)
{	

	//DEBUG_OUT(("%s %d: x=%i, y=%i, width=%i, height=%i, the_color=%i", __func__, __LINE__, x, y, width, height, the_color));

	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}

	if (!Graphics_ValidateAll(the_screen, x, y))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}
	
	if (!Graphics_ValidateXY(the_screen, x + width - 1, y + height - 1))
	{
		LOG_ERR(("%s %d: illegal coordinates. x2=%i, y2=%i", __func__, __LINE__,  x + width - 1, y + height - 1));
		return false;
	}

	// LOGIC:
	//   if fill is needed, it's faster to simply do one rect fill than draw the lines
	//   if fill is not needed, we need 4 line draw calls
	
	if (do_fill)
	{
		if (!Graphics_FillBox(the_screen, x, y, width, height - 1, the_color))
		{
			LOG_ERR(("%s %d: draw filled box failed", __func__, __LINE__));
			return false;
		}
	}
	else
	{
		if (!Graphics_DrawHLine(the_screen, x, y, width, the_color))
		{
			LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
			return false;
		}
	
		if (!Graphics_DrawVLine(the_screen, x + width - 1, y, height, the_color))
		{
			LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
			return false;
		}
	
		if (!Graphics_DrawHLine(the_screen, x, y + height - 1, width, the_color))
		{
			LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
			return false;
		}
	
		if (!Graphics_DrawVLine(the_screen, x, y, height, the_color))
		{
			LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
			return false;
		}
	}
		
	return true;
}


//! Draws a rounded rectangle with the specified size and radius, and optionally fills the rectangle.
//! @param	width: width, in pixels, of the rectangle to be drawn
//! @param	height: height, in pixels, of the rectangle to be drawn
//! @param	radius: radius, in pixels, of the arc to be applied to the rectangle's corners. Minimum 3, maximum 20.
//! @param	the_color: a 1-byte index to the current color LUT
//! @param	do_fill: If true, the box will be filled with the provided color. If false, the box will only draw the outline.
//! @return	returns false on any error/invalid input.
boolean Graphics_DrawRoundBox(Screen* the_screen, signed int x, signed int y, signed int width, signed int height, signed int radius, unsigned char the_color, boolean do_fill)
{	

	//DEBUG_OUT(("%s %d: x=%i, y=%i, width=%i, height=%i, the_color=%i", __func__, __LINE__, x, y, width, height, the_color));

	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}

	if (!Graphics_ValidateAll(the_screen, x, y))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}
	
	if (!Graphics_ValidateXY(the_screen, x + width - 1, y + height - 1))
	{
		LOG_ERR(("%s %d: illegal coordinate", __func__, __LINE__));
		return false;
	}

	if (3 > radius || radius > 20)
	{
		LOG_ERR(("%s %d: illegal roundrect radius: %i", __func__, __LINE__, radius));
		return false;
	}
	
	// adjust box x, y, width, height values to line up with the edges of the arcs
	width -= radius * 2;
	height -= radius * 2;
	x += radius;
	y += radius;
	
	// Draw 4 circle quadrants
	Graphics_DrawCircleQuadrants(the_screen, x, y, radius, the_color, PARAM_SKIP_NE, PARAM_SKIP_SE, PARAM_SKIP_SW, PARAM_DRAW_NW);
	Graphics_DrawCircleQuadrants(the_screen, x + width, y, radius, the_color, PARAM_DRAW_NE, PARAM_SKIP_SE, PARAM_SKIP_SW, PARAM_SKIP_NW);
	Graphics_DrawCircleQuadrants(the_screen, x, y + height, radius, the_color, PARAM_SKIP_NE, PARAM_SKIP_SE, PARAM_DRAW_SW, PARAM_SKIP_NW);
	Graphics_DrawCircleQuadrants(the_screen, x + width, y + height, radius, the_color, PARAM_SKIP_NE, PARAM_DRAW_SE, PARAM_SKIP_SW, PARAM_SKIP_NW);
	
	// draw 4 shortened lines that will match up with the edges of the arcs
	if (!Graphics_DrawHLine(the_screen, x, y - radius, width, the_color))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	if (!Graphics_DrawVLine(the_screen, x + width + radius, y, height, the_color))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	if (!Graphics_DrawHLine(the_screen, x, y + height + radius, width, the_color))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	if (!Graphics_DrawVLine(the_screen, x - radius, y, height, the_color))
	{
		LOG_ERR(("%s %d: draw box failed", __func__, __LINE__));
		return false;
	}
	
	// fill with same color as outline, if specified
	if (do_fill)
	{
		Graphics_FillBox(the_screen, x + radius, y + 1, width - radius*2, radius, the_color);
		Graphics_FillBox(the_screen, x + 1, y + radius, width - 1, height-radius*2, the_color);
		Graphics_FillBox(the_screen, x + radius, y + height-radius*1, width - radius*2, radius-1, the_color);
		Graphics_Fill(the_screen, x + radius - 1, y + 1, the_color);
		Graphics_Fill(the_screen, x + (width - radius) + 1, y + 1, the_color);
		Graphics_Fill(the_screen, x + radius - 1, y + (height - radius) + 1, the_color);
		Graphics_Fill(the_screen, x + (width - radius) + 1, y + (height - radius) + 1, the_color);
	}
		
	return true;
}


//! Draw a circle
//! Based on http://rosettacode.org/wiki/Bitmap/Midpoint_circle_algorithm#C
boolean Graphics_DrawCircle(Screen* the_screen, signed int x1, signed int y1, signed int radius, unsigned char the_color)
{
	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}

	if (!Graphics_ValidateAll(the_screen, x1, y1))
	{
		LOG_ERR(("%s %d: illegal screen id, coordinate, or color", __func__, __LINE__));
		return false;
	}

	return Graphics_DrawCircleQuadrants(the_screen, x1, y1, radius, the_color, PARAM_DRAW_NE, PARAM_DRAW_SE, PARAM_DRAW_SW, PARAM_DRAW_NW);
}


// **** Draw string functions *****





// **** Screen mode/resolution/size functions *****


//! Switch machine into graphics mode
boolean Graphics_SetModeGraphics(Screen* the_screen)
{	
	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}
	
	// switch to graphics mode by setting graphics mode bit, and setting bitmap engine enable bit
	//*the_screen->vicky_ = (*the_screen->vicky_ & GRAPHICS_MODE_MASK | (GRAPHICS_MODE_GRAPHICS) | GRAPHICS_MODE_EN_BITMAP);
	R32(the_screen->vicky_) = (*the_screen->vicky_ & GRAPHICS_MODE_MASK | (GRAPHICS_MODE_GRAPHICS) | GRAPHICS_MODE_EN_BITMAP);

	// enable bitmap layer0
	R32(the_screen->vicky_ + BITMAP_L0_CTRL_L) = 0x01;
}


//! Switch machine into text mode
//! @param as_overlay: If true, sets text overlay mode (text over graphics). If false, sets full text mode (no graphics);
boolean Graphics_SetModeText(Screen* the_screen, boolean as_overlay)
{	
	if (the_screen == NULL)
	{
		LOG_ERR(("%s %d: passed screen was NULL", __func__, __LINE__));
		return false;
	}
	
	// switch to yrcy mode by unsetting graphics mode bit, and setting bitmap engine enable bit
	//*the_screen->vicky_ = (*the_screen->vicky_ & GRAPHICS_MODE_MASK | (GRAPHICS_MODE_GRAPHICS) | GRAPHICS_MODE_EN_BITMAP);
	if (as_overlay)
	{
		R32(the_screen->vicky_) = (*the_screen->vicky_ & GRAPHICS_MODE_MASK | GRAPHICS_MODE_TEXT | GRAPHICS_MODE_TEXT_OVER | GRAPHICS_MODE_GRAPHICS | GRAPHICS_MODE_EN_BITMAP);
		
		// c256foenix, discord 2022/03/10
		// Normally, for example, if you setup everything to be in bitmap mode, and you download an image in VRAM and you can see it properly... If you turn on overlay, then you will see on top of that same image, your text that you had before.
		// Mstr_Ctrl_Text_Mode_En  = $01       ; Enable the Text Mode
		// Mstr_Ctrl_Text_Overlay  = $02       ; Enable the Overlay of the text mode on top of Graphic Mode (the Background Color is ignored)
		// Mstr_Ctrl_Graph_Mode_En = $04       ; Enable the Graphic Mode
		// Mstr_Ctrl_Bitmap_En     = $08       ; Enable the Bitmap Module In Vicky
		// all of these should be ON
	}
	else
	{
		R32(the_screen->vicky_) = (*the_screen->vicky_ & GRAPHICS_MODE_MASK | GRAPHICS_MODE_TEXT);
		
		// disable bitmap layer0
		R32(the_screen->vicky_ + BITMAP_L0_CTRL_L) = 0x00;
	}

}


