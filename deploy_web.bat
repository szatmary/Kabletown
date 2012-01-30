; PATH=%PATH%;c:\Qt\2010.05\qt\bin;C:\Qt\2010.05\mingw\bin

mingw32-make clean

del /Q release
del /Q debug
rmdir release
rmdir debug

qmake.exe comcast.pro "CONFIG+=release"
mingw32-make
mingw32-make clean
