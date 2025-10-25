#ifndef WAYLAND_W
#define WAYLAND_W
#include "wlr-layer-shell-unstable-v1-client-protocol.h" // I have this installed on my stuff system wide and the c are in source folder to build
#include <wayland-client-protocol.h>
#include <wayland-client.h>

typedef struct {
  struct wl_display *dsp;              // connection to wayland compositor
  struct wl_registry *reg;             // registration before using anything
  struct wl_compositor *comp;          // creating regular surface
  struct wl_output *out;               // monitor
  struct zwlr_layer_shell_v1 *lshell;  // layer shell interface
  struct wl_surface *surf;             // our window
  struct zwlr_layer_surface_v1 *lsurf; // attachment

  int w, h, cfg, run; // cfg: configuration flag
} WL;

int wl_init(WL *wl);
void wl_loop(WL *wl);
void wl_quit(WL *wl);

#endif // !WAYLAND_W
