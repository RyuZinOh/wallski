#ifndef GRAPHICS_W
#define GRAPHICS_W
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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

  GLuint prog;
  GLuint vao;
  GLuint vbo;
  GLuint ebo;
} GL;

/*
connection to disp, egl initilization, config,creating the context, surface
tied to wayland window
*/
int gl_init(GL *g, WL *wl);
// making current context, openegl drawing stuffs, displaying
void gl_draw(GL *g);
void gl_quit(GL *g); // destroy egl surface, context, and the window

// transition globals
GLuint wallpaper_tex_old = 0;
GLuint wallpaper_tex_new = 0;
float transition_progress = 1.0f;
int transitioning = 0;
int has_oldtex = 0;

// loading the image from path to openegl as texture
int gl_load_texture(GL *g, const char *path) {
  int img_w, img_h, img_ch;
  unsigned char *pixels = stbi_load(path, &img_w, &img_h, &img_ch, 4);
  if (!pixels) {
    fprintf(stderr, "failed to load the image from %s\n", path);
    return 0;
  }
  // preserving the old pic
  if (wallpaper_tex_new != 0) {
    wallpaper_tex_old = wallpaper_tex_new;
    has_oldtex = 1;
  } else {
    wallpaper_tex_old = 0; // loading first
    has_oldtex = 0;
  }

  glGenTextures(1, &wallpaper_tex_new);
  glBindTexture(GL_TEXTURE_2D, wallpaper_tex_new);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_w, img_h, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(pixels);

  // starting transition
  if (has_oldtex) {
    transition_progress = 0.0f;
    transitioning = 1;
    printf("Transition started! progress=0.0\n"); // DEBUG
  } else {
    transition_progress = 1.0f;
    transitioning = 0;
  }
  return 1;
}
// compiling shaders from source code
GLuint compile_shader(GLenum t, const char *s) {
  GLuint shader = glCreateShader(t);
  glShaderSource(shader, 1, &s, NULL);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char log[512];
    glGetShaderInfoLog(shader, 512, NULL, log);
    fprintf(stderr, "error: %s\n", log);
  }
  return shader;
}

// linking vertex and fragment shader for a usefulness
GLuint create_program(const char *vsrc, const char *fsrc) {
  GLuint v = compile_shader(GL_VERTEX_SHADER, vsrc);
  GLuint f = compile_shader(GL_FRAGMENT_SHADER, fsrc);

  GLuint prog = glCreateProgram();
  glAttachShader(prog, v);
  glAttachShader(prog, f);
  glLinkProgram(prog);

  GLint success;
  glGetProgramiv(prog, GL_LINK_STATUS, &success);
  if (!success) {
    char log[512];
    glGetShaderInfoLog(prog, 512, NULL, log);
    fprintf(stderr, "error: %s\n", log);
  }
  glDeleteShader(v);
  glDeleteShader(f);
  return prog;
}

#endif // !GRAPHICS_W
