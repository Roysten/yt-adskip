SOURCES = src/app.c

adskip.exe: $(SOURCES) Makefile
	x86_64-w64-mingw32-g++ -mwindows -Wall -Wextra -Wpedantic -static $(SOURCES) -I/home/roysten/build/FakeInput/out/out/include -isystemsrc-external -L/home/roysten/build/FakeInput/out/out/lib/ -lfakeInput -lws2_32 -o adskip.exe
