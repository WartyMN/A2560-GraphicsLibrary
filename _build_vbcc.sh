#!/bin/zsh

export VBCC=/opt/vbcc
export PATH=$VBCC/bin:$PATH
export DEVA2560=~/dev/bbedit-workspace-a2560

cd $DEVA2560/lib_graphics

# copy latest version of headers to VBCC
cp lib_graphics.h $VBCC/targets/a2560-micah/include/mb/

# copy headers to easy-to-share for-vbcc folder
cp lib_graphics.h for_vbcc/include/mb/

# make graphics as static lib
vc +/opt/vbcc/config/a2560-4lib-micah -o a2560_graphics.lib lib_graphics.c
cp a2560_graphics.lib for_vbcc/lib/
mv a2560_graphics.lib $VBCC/targets/a2560-micah/lib/


# renew doxygen docs
#doxygen


# *************
# build demo code using the library
vc +/opt/vbcc/config/a2560-s28-micahwlib -o graphics_demo.s28 lib_graphics_demo.c -lm
# s28 generated by vbcc is almost good enough, but the last line tells loader to run from 0x00000, instead of 0x020000. 
perl -i -0777 -pe 's/S804000000FB/S804020000FB/' "$DEVA2560/lib_graphics/graphics_demo.s28"


# *************
# build test code using the library
vc +/opt/vbcc/config/a2560-s28-micahwlib -o graphics_test.s28 lib_graphics_test.c -lm
# s28 generated by vbcc is almost good enough, but the last line tells loader to run from 0x00000, instead of 0x020000. 
perl -i -0777 -pe 's/S804000000FB/S804020000FB/' "$DEVA2560/lib_graphics/graphics_test.s28"






echo "\n**************************\nVBCC build script complete\n**************************\n"
