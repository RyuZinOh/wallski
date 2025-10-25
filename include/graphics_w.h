#ifndef GRAPHICS_W
#define GRAPHICS_W
#include "wayland_w.h"
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <wayland-egl-core.h>
#include <wayland-egl.h>

typedef struct {
  EGLDisplay dsp;            // egl display connection
  EGLContext cont;           // open gl context, and states
  EGLSurface surf;           // drawing surface
  struct wl_egl_window *win; // connection of wayland to egl
} GL;

/*
connection to disp, egl initilization, config,creating the context, surface
tied to wayland window
*/
int gl_init(GL *g, WL *wl);
// making current context, openegl drawing stuffs, displaying
void gl_draw(GL *g);
void gl_quit(GL *g); // destroy egl surface, context, and the window

#endif // !GRAPHICS_W
