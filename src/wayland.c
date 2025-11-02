#include "wayland.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <linux/input-event-codes.h>

#include <wayland-client.h>
#include <wlr-virtual-pointer-unstable-v1.h>

static void output_handle_geometry(void *data, struct wl_output *wl_output, int x, int y,
		int physical_width, int physical_height, int subpixel, const char *make, const char *model,
		int transform);
static void output_handle_mode(void *data, struct wl_output *wl_output, uint32_t flags, int width,
		int height, int refresh);
static void output_handle_description(
		void *data, struct wl_output *wl_output, const char *description);
static void output_handle_name(void *data, struct wl_output *wl_output, const char *name);
static void output_handle_scale(void *data, struct wl_output *wl_output, int factor);
static void output_handle_done(void *data, struct wl_output *wl_output);

static void registry_handler(void *data, struct wl_registry *registry, uint32_t name,
		const char *interface, uint32_t version);
static void registry_remover(void *data, struct wl_registry *registry, uint32_t name);

static int timestamp();

static struct wl_registry_listener registry_listener = {
		.global = registry_handler,
		.global_remove = registry_remover,
};

static struct wl_output_listener output_listener = {
		.geometry = output_handle_geometry,
		.mode = output_handle_mode,
		.done = output_handle_done,
		.scale = output_handle_scale,
		.description = output_handle_description,
		.name = output_handle_name,
};

static void output_handle_geometry(void *data, struct wl_output *wl_output, int x, int y,
		int physical_width, int physical_height, int subpixel, const char *make, const char *model,
		int transform)
{
	(void) data;
	(void) wl_output;
	(void) x;
	(void) y;
	(void) physical_width;
	(void) physical_height;
	(void) subpixel;
	(void) make;
	(void) model;
	(void) transform;
}

static void output_handle_mode(
		void *data, struct wl_output *wl_output, uint32_t flags, int width, int height, int refresh)
{
	(void) wl_output;
	(void) flags;
	(void) refresh;

	struct Wl_ctx *ctx = data;
	ctx->width = width;
	ctx->height = height;
}

static void output_handle_description(
		void *data, struct wl_output *wl_output, const char *description)
{
	(void) data;
	(void) wl_output;
	(void) description;
}

static void output_handle_name(void *data, struct wl_output *wl_output, const char *name)
{
	(void) data;
	(void) wl_output;
	(void) name;
}

static void output_handle_scale(void *data, struct wl_output *wl_output, int factor)
{
	(void) data;
	(void) wl_output;
	(void) factor;
}

static void output_handle_done(void *data, struct wl_output *wl_output)
{
	(void) data;
	(void) wl_output;
}

static void registry_handler(void *data, struct wl_registry *registry, uint32_t name,
		const char *interface, uint32_t version)
{
	struct Wl_ctx *wl_ctx = data;
	if (strcmp(interface, zwlr_virtual_pointer_manager_v1_interface.name) == 0) {
		wl_ctx->virtual_pointer_manager = wl_registry_bind(
				registry, name, &zwlr_virtual_pointer_manager_v1_interface, version);
	} else if (strcmp(interface, wl_seat_interface.name) == 0) {
		wl_ctx->seat = wl_registry_bind(registry, name, &wl_seat_interface, version);
	} else if (strcmp(interface, wl_output_interface.name) == 0) {
		wl_ctx->output = wl_registry_bind(registry, name, &wl_output_interface, version);
		wl_output_add_listener(wl_ctx->output, &output_listener, wl_ctx);
	} else {
		// Not of interest
	}
}

static void registry_remover(void *data, struct wl_registry *registry, uint32_t name)
{
	(void) data;
	(void) registry;
	(void) name;
}

static int timestamp()
{
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);
	int ms = 1000 * tp.tv_sec + tp.tv_nsec / 1000000;
	return ms;
}

bool wlc_setup_wl_ctx(struct Wl_ctx *wl_ctx)
{
	wl_ctx->display = wl_display_connect(NULL);
	if (wl_ctx->display == NULL) {
		fprintf(stderr, "Failed to connect to Wayland display\n");
		return false;
	}

	struct wl_registry *registry = wl_display_get_registry(wl_ctx->display);
	wl_registry_add_listener(registry, &registry_listener, wl_ctx);

	wl_display_roundtrip(wl_ctx->display);

	if (wl_ctx->virtual_pointer_manager == NULL) {
		fprintf(stderr, "Could not create virtual pointer manager");
		return false;
	}

	wl_ctx->virtual_pointer = zwlr_virtual_pointer_manager_v1_create_virtual_pointer(
			wl_ctx->virtual_pointer_manager, NULL);
	if (wl_ctx->virtual_pointer == NULL) {
		fprintf(stderr, "Failed to create virtual pointer\n");
		return false;
	}
	wl_display_roundtrip(wl_ctx->display);
	return true;
}

void wlc_move_pointer(struct Wl_ctx *wl_ctx, int x, int y)
{
	zwlr_virtual_pointer_v1_motion_absolute(
			wl_ctx->virtual_pointer, 0, x, y, wl_ctx->width, wl_ctx->height);
	zwlr_virtual_pointer_v1_frame(wl_ctx->virtual_pointer);
	wl_display_flush(wl_ctx->display);
}

void wlc_click(struct Wl_ctx *wl_ctx)
{
	zwlr_virtual_pointer_v1_button(
			wl_ctx->virtual_pointer, timestamp(), BTN_LEFT, WL_POINTER_BUTTON_STATE_RELEASED);
	zwlr_virtual_pointer_v1_frame(wl_ctx->virtual_pointer);
	zwlr_virtual_pointer_v1_button(
			wl_ctx->virtual_pointer, timestamp(), BTN_LEFT, WL_POINTER_BUTTON_STATE_PRESSED);
	zwlr_virtual_pointer_v1_frame(wl_ctx->virtual_pointer);
	zwlr_virtual_pointer_v1_button(
			wl_ctx->virtual_pointer, timestamp(), BTN_LEFT, WL_POINTER_BUTTON_STATE_RELEASED);
	zwlr_virtual_pointer_v1_frame(wl_ctx->virtual_pointer);
	wl_display_flush(wl_ctx->display);
}
