// unit testing framework
#include "minunit.h"

// project includes

// class being tested
#include "lib_graphics.h"

// C includes


// A2560 includes
#include <mb/a2560_platform.h>
#include <mb/lib_general.h>
#include <mb/lib_text.h>



// variables needed for testing
Screen	global_screen[2];



void text_test_setup(void)	// this is called EVERY test
{
// 	foo = 7;
// 	bar = 4;
// 	
}


void text_test_teardown(void)	// this is called EVERY test
{

}






// **** speed tests

MU_TEST(text_test_hline_speed)
{
	long start1;
	long end1;
	long start2;
	long end2;
	signed int		x;
	signed int		y;
	signed int		line_len;
	unsigned char	the_char;
	signed int		i;
	signed int		num_passes = 90;
	signed int		j;
	signed int		num_cycles = 10;

	x = 1;
	y = 1;
	line_len = 120;
	
	// test speed of first variant
	start1 = mu_timer_real();
	


	// speed test go here
	
	
	end2 = mu_timer_real();
	
	printf("\nSpeed results: first routine completed in %li ticks; second in %li ticks\n", end1 - start1, end2 - start2);
}



	// speed tests
MU_TEST_SUITE(text_test_suite_speed)
{	
	MU_SUITE_CONFIGURE(&text_test_setup, &text_test_teardown);
	
// 	MU_RUN_TEST(text_test_hline_speed);
}


// unit tests
MU_TEST_SUITE(text_test_suite_units)
{	
	MU_SUITE_CONFIGURE(&text_test_setup, &text_test_teardown);
	
// 	MU_RUN_TEST(font_replace_test);
}



int Text_RunTests(void)
{	
	MU_RUN_SUITE(text_test_suite_units);
//  	MU_RUN_SUITE(text_test_suite_speed);
	MU_REPORT();
	
	return MU_EXIT_CODE;
}



int main(int argc, char* argv[])
{

	// find out what kind of machine the software is running on, and configure global screens accordingly
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
	
	printf("Hiya from graphic world.");
	
	Graphics_SetModeGraphics(&global_screen[ID_CHANNEL_B]);
	printf("now in graphics mode");
	getchar();
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 1, 4, (char*)"graphics mode?", FG_COLOR_YELLOW, BG_COLOR_BLACK);
	Graphics_SetModeText(&global_screen[ID_CHANNEL_B], false);
	printf("now in normal text mode");
	getchar();
	Text_DrawStringAtXY(&global_screen[ID_CHANNEL_B], 1, 4, (char*)"overlay mode??", FG_COLOR_YELLOW, BG_COLOR_BLACK);
	Graphics_SetModeText(&global_screen[ID_CHANNEL_B], true);
	printf("now in overlay text mode");

	

	#if defined(RUN_TESTS)
// 		Text_RunTests();
	#endif

}
