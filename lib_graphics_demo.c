/*
 * lib_graphics_demo.c
 *
 *  Created on: Mar 10, 2022
 *      Author: micahbly
 *
 *  Demonstrates many of the features of the graphics library
 *
 */


/*****************************************************************************/
/*                                Includes                                   */
/*****************************************************************************/


// project includes


// C includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// A2560 includes
#include <mcp/syscalls.h>
#include <mb/a2560_platform.h>
#include <mb/lib_general.h>
#include <mb/lib_text.h>
#include <mb/lib_graphics.h>


/*****************************************************************************/
/*                               Definitions                                 */
/*****************************************************************************/



/*****************************************************************************/
/*                             Global Variables                              */
/*****************************************************************************/

Screen	global_screen[2];

/*****************************************************************************/
/*                       Private Function Prototypes                         */
/*****************************************************************************/

// pre-configure screen data. TODO: use sys info to populate based on what's in hardware.
void InitScreen(void);

// have user hit a key, then clear screens
void WaitForUser(void);

// Draw fancy box on the B screen and display demo description
void ShowDescription(char* the_message);

// run all the demos
void RunDemo(void);

// various demos
void Demo_Graphics_FillMemory1(void);
void Demo_Graphics_FillMemory2(void);
void Demo_Graphics_FillBox1(void);
void Demo_Graphics_FillBox2(void);
void Demo_Graphics_FillBox3(void);
void Demo_Graphics_SetPixelAtXY(void);
void Demo_Graphics_DrawHLine1(void);
void Demo_Graphics_DrawLine(void);
void Demo_Graphics_DrawBox(void);
void Demo_Graphics_DrawBoxCoords(void);
void Demo_Graphics_DrawCircle(void);
void Demo_Graphics_Blit1(void);
void Demo_Graphics_ScreenResolution1(void);
void Demo_Graphics_ScreenResolution2(void);


/*****************************************************************************/
/*                       Private Function Definitions                        */
/*****************************************************************************/


// have user hit a key, then clear screens
void WaitForUser(void)
{
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_A], 1, 4, (char*)"Press any key to continue", FG_COLOR_YELLOW, BG_COLOR_DK_BLUE);
	
	getchar();
	
	Graphics_FillMemory(&global_screen[ID_CHANNEL_B], 0x0f);
	Text_FillCharMem(&global_screen[ID_CHANNEL_A], ' ');
	Text_FillAttrMem(&global_screen[ID_CHANNEL_A], 159);
}

// Draw fancy box on the B screen and display demo description
void ShowDescription(char* the_message)
{
	signed int	x1 = 0;
	signed int	x2 = global_screen[ID_CHANNEL_A].text_cols_vis_ - 1;
	signed int	y1 = 0;
	signed int	y2 = 5;

	// draw box and fill contents in prep for next demo description
	Text_DrawBoxCoordsFancy(&global_screen[ID_CHANNEL_A], x1, y1, x2, y2, FG_COLOR_BLUE, BG_COLOR_DK_BLUE);
	Text_FillBox(&global_screen[ID_CHANNEL_A], x1+1, y1+1, x2-1, y2-1, ' ', FG_COLOR_WHITE, BG_COLOR_DK_BLUE);
	
	// wrap text into the message box, leaving one row at the bottom for "press any key"
	Text_DrawStringInBox(&global_screen[ID_CHANNEL_A], x1+1, y1+1, x2-1, y2-1, the_message, FG_COLOR_WHITE, BG_COLOR_DK_BLUE, NULL);
}


void Demo_Graphics_FillMemory1(void)
{
	Graphics_FillMemory(&global_screen[ID_CHANNEL_B], 0x05);
	ShowDescription("Graphics_FillMemory -> fill bitmap screen with value 0x05");	
	WaitForUser();
}


void Demo_Graphics_FillMemory2(void)
{
	Graphics_FillMemory(&global_screen[ID_CHANNEL_B], 0xff);
	ShowDescription("Graphics_FillMemory -> fill bitmap screen with value 0xff");	
	WaitForUser();
}


void Demo_Graphics_FillBox1(void)
{
	int x = 5;
	int	y = 8*6;
	int	width = global_screen[ID_CHANNEL_B].width_ - x - 30;
	int height = global_screen[ID_CHANNEL_B].height_ - y - 100;
	
	ShowDescription("Graphics_FillBox -> fill a square on screen with 0x00");	
	Graphics_FillBox(&global_screen[ID_CHANNEL_B], x, y, width, height, 0x00);
	WaitForUser();
}


void Demo_Graphics_FillBox2(void)
{
	int x = 500;
	int	y = 8*6+100;
	int	width = global_screen[ID_CHANNEL_B].width_ - x - 30;
	int height = global_screen[ID_CHANNEL_B].height_ - y - 100;
	
	ShowDescription("Graphics_FillBox -> fill a square on screen with 0x05");	
	Graphics_FillBox(&global_screen[ID_CHANNEL_B], x, y, width, height, 0x05);
	WaitForUser();
}


void Demo_Graphics_FillBox3(void)
{
	int x = 5;
	int	y = 8*6;
// 	int	width = global_screen[ID_CHANNEL_B].width_ - x - 30;
// 	int height = global_screen[ID_CHANNEL_B].height_ - y - 100;
	
	ShowDescription("Graphics_FillBox -> fill various squares with different color values");	
	Graphics_FillBox(&global_screen[ID_CHANNEL_B], x + 30, y, 250, 100, 0x55);
	Graphics_FillBox(&global_screen[ID_CHANNEL_B], x, y, 25, 25, 0xf5);
	Graphics_FillBox(&global_screen[ID_CHANNEL_B], x, y + 50, 350, 200, 0x3f);
	WaitForUser();
}


void Demo_Graphics_SetPixelAtXY(void)
{
	unsigned char	i;
	unsigned char*	junk_value = (unsigned char*)EA_USER;	// we'll just use whatever bytes we find in our code as x and y
	
	ShowDescription("Graphics_SetPixelAtXY -> Draw pixels in various colors at various locations on screen");	
 	
 	for (i = 0; i < 254; i++)
 	{
	 	Graphics_SetPixelAtXY(&global_screen[ID_CHANNEL_B], *(junk_value++) + i, *(junk_value++) + i, i);
 	}
	WaitForUser();
}


void Demo_Graphics_DrawHLine1(void)
{
	signed int		x;
	signed int		y;
	signed int		line_len;

// 	Graphics_FillMemory(&global_screen[ID_CHANNEL_B], '.');
// 	Text_FillAttrMem(&global_screen[ID_CHANNEL_B], 31);

	ShowDescription("Text_DrawHLine / Text_DrawVLine -> Draw straight lines using a specified color");	
	
	x = 20;
	y = 8*(10);
	line_len = 200;
	Graphics_DrawHLine(&global_screen[ID_CHANNEL_B], x, y, line_len, 0xff);
	Graphics_DrawHLine(&global_screen[ID_CHANNEL_B], x, y + 20, line_len, 0xff);
	Graphics_DrawHLine(&global_screen[ID_CHANNEL_B], x, y + 40, line_len, 0xff);
	Graphics_DrawVLine(&global_screen[ID_CHANNEL_B], x + 100, y - 10, line_len, 0xff);
	Graphics_DrawVLine(&global_screen[ID_CHANNEL_B], x + 150, y - 10, line_len + 50, 0xff);

	WaitForUser();
}


void Demo_Graphics_DrawLine(void)
{
	signed int		x1 = 45;
	signed int		y1 = 100;
	signed int		x2 = 630;
	signed int		y2 = 350;
	signed int		i;

	ShowDescription("Graphics_DrawLine -> Draw a line from any coordinate to any coordinate");	

	for (i = 0; i < 256; i++)
	{
		Graphics_DrawLine(&global_screen[ID_CHANNEL_B], x1, y1, x2, y2, i);
		
		x1 += 2;
		x2 -= 2;
	}

// 	Graphics_DrawLine(&global_screen[ID_CHANNEL_B], x1, y1, x2, y2, 0xee);
// 	Graphics_DrawLine(&global_screen[ID_CHANNEL_B], x2, y1, x1, y2, 0xce);
// 	Graphics_DrawLine(&global_screen[ID_CHANNEL_B], x2, y1+20, x1, y2-20, 0x88);
// 	Graphics_DrawLine(&global_screen[ID_CHANNEL_B], x2, y1+40, x1, y2-40, 0x55);
	
	WaitForUser();
}


void Demo_Graphics_DrawBox(void)
{
	int x = 60;
	int	y = 8*8;
	int	width = global_screen[ID_CHANNEL_B].width_ - x - 60;
	int height = 300;

	ShowDescription("Graphics_DrawBox -> Draw an unfilled box using start coordinates + width and height.");	

	if (Graphics_DrawBox(&global_screen[ID_CHANNEL_B], x, y, width, height, 0xff) == false)
	{
		LOG_ERR(("%s %d: Graphics_DrawBox failed with x=%i, y=%i, width=%i, height=%i", __func__, __LINE__, x, y, width, height));
	}
	
	WaitForUser();
}


void Demo_Graphics_DrawBoxCoords(void)
{
	signed int		x1 = 45;
	signed int		y1 = 100;
	signed int		x2 = 630;
	signed int		y2 = 350;

	ShowDescription("Graphics_DrawBoxCoords -> Draw a box using 4 coordinates.");	

	Graphics_DrawBoxCoords(&global_screen[ID_CHANNEL_B], x1, y1, x2, y2, 0xff);
	WaitForUser();
}


void Demo_Graphics_DrawCircle(void)
{
	signed int		x1 = 320;
	signed int		y1 = 200;
	signed int		radius = 6;
	signed int		i;

	ShowDescription("Graphics_DrawCircle -> Draw a circle");	

	for (i = 0; i < 256 && radius < 238; i += 3)
	{
		Graphics_DrawCircle(&global_screen[ID_CHANNEL_B], x1, y1, radius, i);
		
		radius += 3;
	}

// 	Graphics_DrawCircle(&global_screen[ID_CHANNEL_B], x1, y1, radius, 0xff);
	Graphics_DrawCircle(&global_screen[ID_CHANNEL_B], 25, 25, 12, 0xff);
	Graphics_DrawCircle(&global_screen[ID_CHANNEL_B], 25, 25, 15, 0xff);

	WaitForUser();
}


void Demo_Graphics_Blit1(void)
{
	signed int		x1 = 320;
	signed int		y1 = 200;
	signed int		radius = 6;
	signed int		box_height = 16;
	signed int		box_width = global_screen[ID_CHANNEL_B].width_;
	signed int		color = 0x20;
	signed int		i;
	Bitmap			src_bm;
	Bitmap			dst_bm;

	ShowDescription("Graphics_BlitBitMap -> Copy a rectange of pixels from one bitmap to another");	

	// draw 30 horizontal bands of color to help judge blit effects
	
	for (i = 0; i < 30; i++)
	{
		Graphics_FillBox(&global_screen[ID_CHANNEL_B], 0, box_height * i, box_width, box_height, color);		
		color += 7;
	}
	
 	Graphics_DrawCircle(&global_screen[ID_CHANNEL_B], 25, 25, 12, 0x88);
	Graphics_DrawCircle(&global_screen[ID_CHANNEL_B], 25, 25, 15, 0xcc);
	Graphics_DrawCircle(&global_screen[ID_CHANNEL_B], 25, 25, 20, 0xff);

	// copy bits of this screen to other parts of the Screen
	src_bm.addr_ = (unsigned char*)VRAM_BUFFER_A;
	src_bm.width_ = global_screen[ID_CHANNEL_B].width_;
	src_bm.height_ = global_screen[ID_CHANNEL_B].height_;
	dst_bm.addr_ = (unsigned char*)VRAM_BUFFER_A;
	dst_bm.width_ = global_screen[ID_CHANNEL_B].width_;
	dst_bm.height_ = global_screen[ID_CHANNEL_B].height_;

	Graphics_BlitBitMap(&global_screen[ID_CHANNEL_B], &src_bm, 0, 0, &dst_bm, 100, 0, 50, 50);
	Graphics_BlitBitMap(&global_screen[ID_CHANNEL_B], &src_bm, 0, 0, &dst_bm, 100, 100, 50, 50);
	Graphics_BlitBitMap(&global_screen[ID_CHANNEL_B], &src_bm, 0, 0, &dst_bm, 100, 200, 50, 50);
	Graphics_BlitBitMap(&global_screen[ID_CHANNEL_B], &src_bm, 0, 0, &dst_bm, 100, 300, 50, 50);
	Graphics_BlitBitMap(&global_screen[ID_CHANNEL_B], &src_bm, 0, 0, &dst_bm, 200, 100, 50, 50);
	Graphics_BlitBitMap(&global_screen[ID_CHANNEL_B], &src_bm, 0, 0, &dst_bm, 200, 200, 50, 50);
	Graphics_BlitBitMap(&global_screen[ID_CHANNEL_B], &src_bm, 0, 0, &dst_bm, 200, 300, 50, 50);
	Graphics_BlitBitMap(&global_screen[ID_CHANNEL_B], &src_bm, 0, 0, &dst_bm, 300, 100, 50, 50);
	Graphics_BlitBitMap(&global_screen[ID_CHANNEL_B], &src_bm, 0, 0, &dst_bm, 300, 200, 50, 50);
	Graphics_BlitBitMap(&global_screen[ID_CHANNEL_B], &src_bm, 0, 0, &dst_bm, 300, 300, 50, 50);
// 	Graphics_BlitBitMap(&global_screen[ID_CHANNEL_B], &src_bm, x1 - 100, y1 - 100, &dst_bm, 0, 0, 100, 100);
// 	Graphics_BlitBitMap(&global_screen[ID_CHANNEL_B], &src_bm, x1 - 100, y1 - 100, &dst_bm, 400, 300, 100, 100);
	
	// do a 'dragon' effect
	Graphics_DrawBox(&global_screen[ID_CHANNEL_B], 550, 350, 20, 20, 0xff);
	Graphics_DrawBox(&global_screen[ID_CHANNEL_B], 550, 350, 30, 30, 0xcc);
	Graphics_DrawBox(&global_screen[ID_CHANNEL_B], 550, 350, 40, 40, 0xbb);
	Graphics_DrawBox(&global_screen[ID_CHANNEL_B], 550, 350, 50, 50, 0x99);
	
	for (i = 0; i < 25; i++)
	{
		Graphics_BlitBitMap(&global_screen[ID_CHANNEL_B], &src_bm, 550, 350, &dst_bm, i*10, 350 - i*10, 50, 50);
	}
	
	WaitForUser();
}


// boolean Graphics_BlitBitMap(Screen* the_screen, Bitmap* src_bm, int src_x, int src_y, Bitmap* dst_bm, int dst_x, int dst_y, int width, int height);



void Demo_Graphics_ScreenResolution1(void)
{
	char			msg_buffer[80*3];
	char*			the_message = msg_buffer;
	int				y = 7;
	
	Text_SetVideoMode(&global_screen[ID_CHANNEL_B], RES_800X600);
	ShowDescription("Text_SetVideoMode -> (RES_800X600) Changes resolution to 800x600 if available for this screen/channel.");	

	sprintf(the_message, "Requested 800x600. Actual: %i x %i, %i x %i text, %i x %i visible text", 
		global_screen[ID_CHANNEL_B].width_, 
		global_screen[ID_CHANNEL_B].height_, 
		global_screen[ID_CHANNEL_B].text_mem_cols_, 
		global_screen[ID_CHANNEL_B].text_mem_rows_, 
		global_screen[ID_CHANNEL_B].text_cols_vis_, 
		global_screen[ID_CHANNEL_B].text_rows_vis_
		);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, y, the_message, FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, y + 1, (char*)"0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, y + 2, (char*)"<-START OF LINE", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 70, (char*)"ROW70", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_ShowFontChars(&global_screen[ID_CHANNEL_B], y + 3);

	WaitForUser();
}


void Demo_Graphics_ScreenResolution2(void)
{
	char			msg_buffer[80*3];
	char*			the_message = msg_buffer;
	int				y = 7;
	
	Text_SetVideoMode(&global_screen[ID_CHANNEL_B], RES_640X480);
	ShowDescription("Text_SetVideoMode -> (RES_640X480) Changes resolution to 640x480 if available for this screen/channel.");	

	sprintf(the_message, "Requested 640x480. Actual: %i x %i, %i x %i text, %i x %i visible text", 
		global_screen[ID_CHANNEL_B].width_, 
		global_screen[ID_CHANNEL_B].height_, 
		global_screen[ID_CHANNEL_B].text_mem_cols_, 
		global_screen[ID_CHANNEL_B].text_mem_rows_, 
		global_screen[ID_CHANNEL_B].text_cols_vis_, 
		global_screen[ID_CHANNEL_B].text_rows_vis_
		);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, y, the_message, FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, y + 1, (char*)"01234567890123456789012345678901234567890123456789012345678901234567890123456789", FG_COLOR_DK_BLUE, BG_COLOR_YELLOW);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, y + 2, (char*)"<-START OF LINE", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 0, 55, (char*)"ROW55", FG_COLOR_BLACK, BG_COLOR_GREEN);
	Text_ShowFontChars(&global_screen[ID_CHANNEL_B], y + 3);

	WaitForUser();
}




void RunDemo(void)
{
	Text_FillCharMem(&global_screen[ID_CHANNEL_B], ' ');
	Text_FillAttrMem(&global_screen[ID_CHANNEL_B], 160);

	ShowDescription("Welcome to the A2560 Graphics Library Demo!");	
	WaitForUser();
	
	Demo_Graphics_FillMemory1();
// 	Demo_Graphics_FillMemory2();
// 	
// 	Demo_Graphics_FillBox1();
// 	Demo_Graphics_FillBox2();
// 	Demo_Graphics_FillBox3();
// 
// 	Demo_Graphics_SetPixelAtXY();
// 
// 	Demo_Graphics_DrawHLine1();
	
// 	Demo_Graphics_DrawLine();
	
	Demo_Graphics_DrawBox();
	Demo_Graphics_DrawBoxCoords();

	Demo_Graphics_DrawCircle();
	
	Demo_Graphics_Blit1();
	
	Demo_Graphics_ScreenResolution1();
	Demo_Graphics_ScreenResolution2();
	
}


/*****************************************************************************/
/*                        Public Function Definitions                        */
/*****************************************************************************/




int main(int argc, char* argv[])
{
	
	global_screen[ID_CHANNEL_A].id_ = ID_CHANNEL_A;
	global_screen[ID_CHANNEL_B].id_ = ID_CHANNEL_B;

	if (Text_AutoConfigureScreen(&global_screen[ID_CHANNEL_A]) == false)
	{
		DEBUG_OUT(("%s %d: Auto configure failed for screen A", __func__, __LINE__));
		exit(0);
	}
	
	if (Text_AutoConfigureScreen(&global_screen[ID_CHANNEL_B]) == false)
	{
		DEBUG_OUT(("%s %d: Auto configure failed for screen B", __func__, __LINE__));
		exit(0);
	}
	
	Graphics_SetModeGraphics(&global_screen[ID_CHANNEL_B]);
	
	RunDemo();
	
	exit(0);
}