#include "vpx_image.h"

vpx_image_t * (*vpx_img_alloc)(vpx_image_t *img, vpx_img_fmt_t fmt,
                           unsigned int d_w, unsigned int d_h,
                           unsigned int align);

vpx_image_t *(*vpx_img_wrap)(vpx_image_t *img, vpx_img_fmt_t fmt, unsigned int d_w,
                          unsigned int d_h, unsigned int stride_align,
                          unsigned char *img_data);
int (*vpx_img_set_rect)(vpx_image_t *img, unsigned int x, unsigned int y,
                     unsigned int w, unsigned int h);
void (*vpx_img_flip)(vpx_image_t *img);
void (*vpx_img_free)(vpx_image_t *img);
