SOURCES = src/app.c src/wayland.c
WAYLAND_CLIENT_HEADERS = src-external/wlr-virtual-pointer-unstable-v1.h
WAYLAND_CLIENT_SOURCES = src-external/wlr-virtual-pointer-unstable-v1.c
DEPS = wayland-client

adskip: $(WAYLAND_CLIENT_HEADERS) $(WAYLAND_CLIENT_SOURCES) $(SOURCES) Makefile
	gcc -Wall -Wextra -Wpedantic $(SOURCES) $(WAYLAND_CLIENT_SOURCES) -isystemsrc-external $(shell pkg-config --cflags --libs $(DEPS)) -o adskip

src-external/wlr-virtual-pointer-unstable-v1.h:
	wayland-scanner client-header src-external/wlr-protocols/unstable/wlr-virtual-pointer-unstable-v1.xml src-external/wlr-virtual-pointer-unstable-v1.h

src-external/wlr-virtual-pointer-unstable-v1.c:
	wayland-scanner private-code src-external/wlr-protocols/unstable/wlr-virtual-pointer-unstable-v1.xml src-external/wlr-virtual-pointer-unstable-v1.c
