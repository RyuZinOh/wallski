#ifndef COLORS_W_H
#define COLORS_W_H
#include "stb_image.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"

// storing at hex 0x[hex]
typedef struct {
  uint32_t primary;
  uint32_t secondary;
} Palette;

// randomly extracting palette
static inline Palette extract_palette(const char *path) {
  int w, h, ch;
  unsigned char *pix = stbi_load(path, &w, &h, &ch, 3);
  Palette pal = {0, 0}; // all black
  if (!pix) {
    return pal;
  }

  int total_pix = w * h;

  // 2 pixel randomly sampling
  if (total_pix > 0) {
    int i1 = rand() % total_pix;
    int i2 = rand() % total_pix;

    int i1_ofs = i1 * 3;
    int i2_ofs = i2 * 3;

    //[r.g.b [32bit]]-> {sixteen, eight, least significiant}
    pal.primary =
        (pix[i1_ofs] << 16) | (pix[i1_ofs + 1] << 8) | pix[i1_ofs + 2];
    pal.secondary =
        (pix[i2_ofs] << 16) | (pix[i2_ofs + 1] << 8) | pix[i2_ofs + 2];
  }

  stbi_image_free(pix);
  return pal;
}

#endif // !COLORS_W_H
