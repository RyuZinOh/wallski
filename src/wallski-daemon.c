#include "../include/colors_w.h"
#include "../include/graphics_w.h"
#include "../include/wayland_w.h"
#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <GLES2/gl2.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-egl-core.h>
#include <wayland-util.h>
#include <wlr-layer-shell-unstable-v1-client-protocol.h>
#define WLSOCK_PATH "/tmp/wallski.sock"

// types of transitioning
typedef enum {
  TRANS_WIPE = 0,
  TRANS_SHATTER,
  TRANS_FADE,
  TRANS_RIPPLE,
  TRANS_ZOOM
} TTT;
TTT current_t = TRANS_WIPE;

// setter
void gl_set_transition(const char *name) {
  if (strcmp(name, "shatter") == 0) {
    current_t = TRANS_SHATTER;
  } else if (strcmp(name, "fade") == 0) {
    current_t = TRANS_FADE;
  } else if (strcmp(name, "ripple") == 0) {
    current_t = TRANS_RIPPLE;
  } else if (strcmp(name, "zoom") == 0) {
    current_t = TRANS_ZOOM;
  } else {
    current_t = TRANS_WIPE;
  }
}

// caching
const char *get_cache_file() {
  const char *xdg = getenv("XDG_CACHE_HOME");
  static char path[512];
  // if xdg exists else we know how to hard code
  if (xdg) {
    snprintf(path, sizeof(path), "%s/wallski/current", xdg);
  } else {
    snprintf(path, sizeof(path), "%s/.cache/wallski/current", getenv("HOME"));
  }
  return path;
}

const char *get_palete_file() {
  const char *xdg = getenv("XDG_CACHE_HOME");
  static char path[512];
  // if xdg exists else we know how to hard code
  if (xdg) {
    snprintf(path, sizeof(path), "%s/wallski/palette", xdg);
  } else {
    snprintf(path, sizeof(path), "%s/.cache/wallski/palette", getenv("HOME"));
  }
  return path;
}

void save_current_wallpaper(const char *wallpaper) {
  const char *file = get_cache_file(); // get the full path returned by upper
  char dir[512];
  strncpy(dir, file, sizeof(dir));
  char *slash = strrchr(dir, '/'); // for seperation in linux dir  system
  if (slash) {
    *slash = 0;
    mkdir(dir, 0755);
  }
  FILE *f = fopen(file, "w");
  // write current stuff to that path init
  if (f) {
    fprintf(f, "%s\n", wallpaper);
    fclose(f);
  }
}

void save_palette(const char *wallpaper) {
  const char *file = get_palete_file(); // get the full path returned by upper
  char dir[512];
  strncpy(dir, file, sizeof(dir));
  char *slash = strrchr(dir, '/'); // for seperation in linux dir  system
  if (slash) {
    *slash = 0;
    mkdir(dir, 0755);
  }
  FILE *f = fopen(file, "w");
  // write current stuff to that path init
  if (!f) {
    return;
  }
  Palette pal = extract_palette(wallpaper);
  fprintf(f, "primary=#%06X\n", pal.primary);
  fprintf(f, "secondary=#%06X\n", pal.secondary);
  fclose(f);
}

// retuning stuff if found else NULL
char *load_current_wallpaper() {
  const char *file = get_cache_file();
  FILE *f = fopen(file, "r");
  if (!f) {
    return NULL;
  }

  static char buf[512];
  if (fgets(buf, sizeof(buf), f)) {
    buf[strcspn(buf, "\n")] = 0;
    fclose(f);
    return buf;
  }
  fclose(f);
  return NULL;
}

Palette load_palette() {
  Palette p = {0, 0};
  const char *file = get_palete_file();
  FILE *f = fopen(file, "r");
  if (!f) {
    return p;
  }
  char line[128];
  while (fgets(line, sizeof(line), f)) {
    if (sscanf(line, "primary=#%06X", &p.primary) == 1) {
      continue;
    }
    if (sscanf(line, "secondary=#%06X", &p.secondary) == 1) {
      continue;
    }
  }
  fclose(f);
  return p;
}
float cursor_x = 0.5f; // 0 is  left and 1 is right [we initilize at 0.5 meaning
                       // center tyakkai]
static void pointer_motion(void *data, struct wl_pointer *p, uint32_t time,
                           wl_fixed_t x, wl_fixed_t y) {
  WL *wl = (WL *)data;
  if (wl->w > 0) {
    static float target_cursor = 0.5f;
    target_cursor = wl_fixed_to_double(x) / wl->w;
    if (target_cursor < 0.0f) {
      target_cursor = 0.0f;
    }
    if (target_cursor > 1.0f) {
      target_cursor = 1.0f;
    }
    /*
    offbrand lerp, cause someone suggested [linear interpolation]
    creating smoooth motion instead than instant jumps -> moving cursor_x to
    real mouse position.
    */
    float speed = 0.05f;
    cursor_x += (target_cursor - cursor_x) * speed;
  }
}

static void pointer_enter(void *data, struct wl_pointer *p, uint32_t serial,
                          struct wl_surface *surface, wl_fixed_t sx,
                          wl_fixed_t sy) {
  (void)data;
  (void)p;
  (void)serial;
  (void)surface;
  (void)sx;
  (void)sy;
}
static void pointer_leave(void *data, struct wl_pointer *p, uint32_t serial,
                          struct wl_surface *surface) {

  (void)data;
  (void)p;
  (void)serial;
  (void)surface;
}
static void pointer_button(void *data, struct wl_pointer *p, uint32_t serial,
                           uint32_t time, uint32_t button, uint32_t state) {

  (void)data;
  (void)p;
  (void)serial;
  (void)time;
  (void)button;
  (void)state;
}
static void pointer_axis(void *data, struct wl_pointer *p, uint32_t time,
                         uint32_t axis, wl_fixed_t value) {

  (void)data;
  (void)p;
  (void)time;
  (void)axis;
  (void)value;
}

// callbacks
static void pointer_frame(void *data, struct wl_pointer *p) {
  (void)data;
  (void)p;
}

static void pointer_axis_source(void *data, struct wl_pointer *p,
                                uint32_t axis_source) {
  (void)data;
  (void)p;
  (void)axis_source;
}

static void pointer_axis_stop(void *data, struct wl_pointer *p, uint32_t time,
                              uint32_t axis) {
  (void)data;
  (void)p;
  (void)time;
  (void)axis;
}

static void pointer_axis_discrete(void *data, struct wl_pointer *p,
                                  uint32_t axis, int32_t discrete) {
  (void)data;
  (void)p;
  (void)axis;
  (void)discrete;
}

// structure of pointer events assigning to each callbacks
static const struct wl_pointer_listener pointer_listener = {
    .motion = pointer_motion,
    .enter = pointer_enter,
    .leave = pointer_leave,
    .button = pointer_button,
    .axis = pointer_axis,
    .frame = pointer_frame,
    .axis_source = pointer_axis_source,
    .axis_stop = pointer_axis_stop,
    .axis_discrete = pointer_axis_discrete};

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
  } else if (strcmp(iface, "wl_seat") == 0) {
    wl->seat = wl_registry_bind(r, name, &wl_seat_interface, 5); // v5
    wl->pointer = wl_seat_get_pointer(wl->seat);
    if (wl->pointer) {
      wl_pointer_add_listener(wl->pointer, &pointer_listener, wl);
    }
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

// reading file
char *read_file(const char *path) {
  FILE *f = fopen(path, "rb"); // binary read mode and return in f pointer
  if (!f) {
    return NULL;
  }
  fseek(f, 0, SEEK_END); // pointer to end
  long len = ftell(f);
  fseek(f, 0, SEEK_SET); // pointer back to beginning
  char *buf = malloc(len + 1);
  if (!buf) {
    fclose(f);
    return NULL;
  }
  fread(buf, 1, len, f);
  buf[len] = '\0'; // add null terminator here [end]
  fclose(f);
  return buf;
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

  // uint32_t seat_id = 0;
  // struct wl_seat *seat =
  //     wl_registry_bind(wl->reg, seat_id, &wl_seat_interface, 5); // v5
  // struct wl_pointer *pointer = wl_seat_get_pointer(seat);
  // wl_pointer_add_listener(pointer, &pointer_listener, wl);

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

// // test blue
// void gl_draw(GL *g) {
//   glClearColor(0.0, 0.0, 1.0, 1.0);
//   glClear(GL_COLOR_BUFFER_BIT);
//   eglSwapBuffers(g->dsp, g->surf);
// }

void gl_draw(GL *i, WL *w) {
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(i->prog);
  glBindBuffer(GL_ARRAY_BUFFER, i->vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i->ebo);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,
                wallpaper_tex_old ? wallpaper_tex_old : wallpaper_tex_new);
  glUniform1i(glGetUniformLocation(i->prog, "tex_old"), 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, wallpaper_tex_new);
  glUniform1i(glGetUniformLocation(i->prog, "tex_new"), 1);

  glUniform1f(glGetUniformLocation(i->prog, "u_cursor"), cursor_x);
  glUniform1f(glGetUniformLocation(i->prog, "u_img_width"),
              (float)wallpaper_width);
  glUniform1f(glGetUniformLocation(i->prog, "u_view_width"), (float)w->w);

  if (transitioning && has_oldtex) {
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, wallpaper_tex_old);
    // glUniform1i(glGetUniformLocation(i->prog, "tex_old"), 0);
    //
    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, wallpaper_tex_new);
    // glUniform1i(glGetUniformLocation(i->prog, "tex_new"), 1);

    // wipe
    transition_progress += 0.05f; // speed
    if (transition_progress >= 1.0f) {
      transition_progress = 1.0f;
      transitioning = 0;
      glDeleteTextures(1, &wallpaper_tex_old);
      wallpaper_tex_old = 0;
      has_oldtex = 0;
    }
    // glUniform1f(glGetUniformLocation(i->prog, "u_progress"),
    // transition_progress);
    // glUniform1i(glGetUniformLocation(i->prog, "u_type"),
    // (int)current_t);
  } else {
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, wallpaper_tex_new);
    // glUniform1i(glGetUniformLocation(i->prog, "tex_new"), 0);
    // glUniform1f(glGetUniformLocation(i->prog, "u_progress"), 1.0f);
    transition_progress = 1.0f;
  }
  glUniform1f(glGetUniformLocation(i->prog, "u_progress"), transition_progress);
  glUniform1i(glGetUniformLocation(i->prog, "u_type"), (int)current_t);

  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  eglSwapBuffers(i->dsp, i->surf);
}

void gl_quit(GL *g) {
  eglDestroySurface(g->dsp, g->surf);
  eglDestroyContext(g->dsp, g->cont);
  eglTerminate(g->dsp);
  wl_egl_window_destroy(g->win);
}

int main() {
  WL w = {0};
  GL g = {0};

  if (!wl_init(&w) || !gl_init(&g, &w)) {
    return 1;
  }

  // what if the wallpaper exists ?
  char *cached_wp = load_current_wallpaper();
  if (cached_wp) {
    gl_load_texture(&g, cached_wp);
  }

  // Unix Socket
  int dsock = socket(AF_UNIX, SOCK_STREAM, 0);
  struct sockaddr_un addr = {0};
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, WLSOCK_PATH, sizeof(addr.sun_path) - 1);
  unlink(WLSOCK_PATH);
  if (bind(dsock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind failure");
    close(dsock);
    return 1;
  }

  listen(dsock, 5);
  fcntl(dsock, F_SETFL, O_NONBLOCK);
  /*
   axis
   -1 = left, bottom
    1 - right, top
    i.e, so lefttop would be -1, 1

  texturemapping
  1 = right and bottom
  0 = left and top
  */

  // at each four
  float vertices[] = {-1.0f, -1.0f, 0.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f,
                      1.0f,  1.0f,  1.0f, 0.0f, -1.0f, 1.0f,  0.0f, 0.0f};
  unsigned int indicies[] = {0, 1, 2, 2, 3, 0};
  glGenBuffers(1, &g.vbo);
  glGenBuffers(1, &g.ebo);
  glBindBuffer(GL_ARRAY_BUFFER, g.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies,
               GL_STATIC_DRAW);

  // shaders
  /*
   cp  the project shaders to the relevant directory so it could match here
   while contributing [note] -> while creating production grade binary so that
   other can use , this should be uncommented
  */
  // char *vert_src = read_file("/usr/share/wallski/assets/wallpaper.vert");
  // char *frag_src = read_file("/usr/share/wallski/assets/wallpaper.frag");

  // uncomment this while development as you wil be working with shaders and u
  // cant simple always mv this and that so
  char *vert_src = read_file("../assets/wallpaper.vert");
  char *frag_src = read_file("../assets/wallpaper.frag");

  if (!vert_src || !frag_src) {
    return 1;
  }

  g.prog = create_program(vert_src, frag_src);
  free(vert_src);
  free(frag_src);

  while (w.run) {
    wl_loop(&w);
    gl_draw(&g, &w);

    // debugging
    // static int frame = 0;
    // if (++frame % 60 == 0) {
    //   printf("frame [%d]", frame);
    // }

    // check commands
    int client = accept(dsock, NULL, NULL);
    if (client != -1) {
      char buf[512];
      int n = read(client, buf, sizeof(buf) - 1);
      if (n > 0) {
        buf[n] = '\0';
        char *path = NULL;
        char *trans = "wipe";
        char *tok = strtok(buf, " ");
        // skip whitespace
        while (tok) {
          if (strcmp(tok, "--set") == 0) {
            tok = strtok(NULL, " ");
            if (tok) {
              path = tok;
            }
          } else if (strcmp(tok, "--transition") == 0) {
            tok = strtok(NULL, " ");
            if (tok) {
              trans = tok;
            }
          }
          tok = strtok(NULL, " ");
        }
        if (path) {
          gl_set_transition(trans);
          gl_load_texture(&g, path);
          save_current_wallpaper(path); // when we change wall, save it to
          save_palette(path);
        }
      }
      close(client);
    }
    if (!transitioning) {
      usleep(16666); // 60fps
    }
  }
  gl_quit(&g);
  wl_quit(&w);
  close(dsock);
  unlink(WLSOCK_PATH);
  return 0;
}
