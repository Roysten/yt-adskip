#pragma once
#include <stdbool.h>

struct Wl_ctx
{
	struct wl_seat *seat;
	struct wl_display *display;
	struct zwlr_virtual_pointer_manager_v1 *virtual_pointer_manager;
	struct zwlr_virtual_pointer_v1 *virtual_pointer;
	struct wl_output *output;
	int width;
	int height;
};

bool wlc_setup_wl_ctx(struct Wl_ctx *wl_ctx);
void wlc_move_pointer(struct Wl_ctx *wl_ctx, int x, int y);
void wlc_click(struct Wl_ctx *wl_ctx);
