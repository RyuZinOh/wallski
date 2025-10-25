#include "../include/graphics_w.h"
#include "../include/wayland_w.h"
#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <GLES2/gl2.h>
#include <stdio.h>
#include <string.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-egl-core.h>
#include <wlr-layer-shell-unstable-v1-client-protocol.h>

// registry callbacks
/*
 - userdata
 - registry object
 - unique id
 - interface
 - version
 [casting data to structure of wayland header struct , comparing it to
 interfaces, for each match assigning the usable interface on the structure ]
*/

static void reg_add(void *data, struct wl_registry *r, uint32_t name,
                    const char *iface, uint32_t ver) {
  WL *wl = data;
  if (strcmp(iface, wl_compositor_interface.name) == 0) {
    wl->comp = wl_registry_bind(r, name, &wl_compositor_interface, 4); // v4
  } else if (strcmp(iface, wl_output_interface.name) == 0) {
    wl->out = wl_registry_bind(r, name, &wl_output_interface, 3); // v3
  } else if (strcmp(iface, zwlr_layer_shell_v1_interface.name) == 0) {
    wl->lshell =
        wl_registry_bind(r, name, &zwlr_layer_shell_v1_interface, 1); // v1
  }
}

static void reg_rm(void *data, struct wl_registry *r, uint32_t name) {
  (void)data;
  (void)r;
  (void)name;
}

// layer callbacks
/*
- data
- layer surface
- yes/no
- res
*/
static void on_cfg(void *data, struct zwlr_layer_surface_v1 *s, uint32_t serial,
                   uint32_t w, uint32_t h) {
  WL *wl = data;
  wl->w = w;
  wl->h = h;
  wl->cfg = 1;
  zwlr_layer_surface_v1_ack_configure(s, serial);
  printf("Configured: %dX%d", w, h);
}

static void on_close(void *data, struct zwlr_layer_surface_v1 *s) {
  WL *wl = data;
  wl->run = 0;
}

// connection to the wayland
int wl_init(WL *wl) {
  wl->dsp = wl_display_connect(NULL);
  if (!wl->dsp) {
    fprintf(stderr, "Error connecting \n");
    return 0;
  } else {
    printf("wayland OK");
  }
  wl->reg = wl_display_get_registry(wl->dsp);
  static const struct wl_registry_listener reg_lis = {reg_add, reg_rm};
  wl_registry_add_listener(wl->reg, &reg_lis, wl);
  wl_display_roundtrip(wl->dsp); // populate the reg_ad with compositor
                                 // interface, layershell, output interface
  wl->surf = wl_compositor_create_surface(
      wl->comp); // wayland surface for rendering stuff
  wl->lsurf = zwlr_layer_shell_v1_get_layer_surface(
      wl->lshell, wl->surf, wl->out, ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND,
      "wallski"); // layer surface shell, with wallski class for client

  static const struct zwlr_layer_surface_v1_listener lis = {on_cfg, on_close};
  zwlr_layer_surface_v1_add_listener(wl->lsurf, &lis, wl);
  // fullscreen stretching from center
  zwlr_layer_surface_v1_set_anchor(wl->lsurf,
                                   ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP |
                                       ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM |
                                       ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT |
                                       ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT);
  zwlr_layer_surface_v1_set_exclusive_zone(wl->lsurf, -1);
  wl_surface_commit(wl->surf);
  while (!wl->cfg) {
    wl_display_dispatch(wl->dsp);
  }
  wl->run = 1;
  return 1;
}

void wl_loop(WL *wl) {
  wl_display_dispatch_pending(wl->dsp);
} // check if the wayland server running
void wl_quit(WL *wl) { wl_display_disconnect(wl->dsp); }

// setting up rendering context on wl surface[linking things with opengl]
int gl_init(GL *g, WL *w) {
  EGLint maj, min, n;
  EGLConfig cfg;
  EGLint attr[] = {EGL_SURFACE_TYPE,
                   EGL_WINDOW_BIT,
                   EGL_RED_SIZE,
                   8,
                   EGL_GREEN_SIZE,
                   8,
                   EGL_BLUE_SIZE,
                   8,
                   EGL_ALPHA_SIZE,
                   8,
                   EGL_RENDERABLE_TYPE,
                   EGL_OPENGL_ES2_BIT,
                   EGL_NONE};
  EGLint ctx_attr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

  g->dsp = eglGetDisplay((EGLNativeDisplayType)w->dsp); // link wayland to egl
  eglInitialize(g->dsp, &maj, &min);
  eglChooseConfig(g->dsp, attr, &cfg, 1, &n);
  // rendering context
  g->cont = eglCreateContext(g->dsp, cfg, EGL_NO_CONTEXT, ctx_attr);
  g->win = wl_egl_window_create(w->surf, w->w, w->h);
  g->surf =
      eglCreateWindowSurface(g->dsp, cfg, (EGLNativeWindowType)g->win, NULL);
  eglMakeCurrent(g->dsp, g->surf, g->surf,
                 g->cont); // bind the context to surface
  printf("%d.%d", maj, min);
  return 1;
}

// test blue
void gl_draw(GL *g) {
  glClearColor(0.0, 0.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  eglSwapBuffers(g->dsp, g->surf);
}

void gl_quit(GL *g) {
  eglDestroySurface(g->dsp, g->surf);
  eglDestroyContext(g->dsp, g->cont);
  eglTerminate(g->dsp);
  wl_egl_window_destroy(g->win);
}

int main() {
  printf("wallski");
  // initliize at NULL and storing every object on their corresponding strucuts
  WL w = {0};
  GL g = {0};
  if (!wl_init(&w) || !gl_init(&g, &w)) {
    return 1;
  }
  while (w.run) {
    wl_loop(&w);
    gl_draw(&g);
  }
  gl_quit(&g);
  wl_quit(&w);
  return 0;
}
