/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <algorithm>
#include "tools_common.h"
#include "vpx_decoder.h"

int read_yuv_frame(struct VpxInputContext *input_ctx, vpx_image_t *yuv_frame) {
  FILE *f = input_ctx->file;
  struct FileTypeDetectionBuffer *detect = &input_ctx->detect;
  int plane = 0;
  int shortread = 0;
  const int bytespp = (yuv_frame->fmt & VPX_IMG_FMT_HIGHBITDEPTH) ? 2 : 1;

  for (plane = 0; plane < 3; ++plane) {
    uint8_t *ptr;
    int w = vpx_img_plane_width(yuv_frame, plane);
    const int h = vpx_img_plane_height(yuv_frame, plane);
    int r;
    // Assuming that for nv12 we read all chroma data at one time
    if (yuv_frame->fmt == VPX_IMG_FMT_NV12 && plane > 1) break;
    // Fixing NV12 chroma width it is odd
    if (yuv_frame->fmt == VPX_IMG_FMT_NV12 && plane == 1) w = (w + 1) & ~1;
    /* Determine the correct plane based on the image format. The for-loop
     * always counts in Y,U,V order, but this may not match the order of
     * the data on disk.
     */
    switch (plane) {
      case 1:
        ptr =
            yuv_frame->planes[yuv_frame->fmt == VPX_IMG_FMT_YV12 ? VPX_PLANE_V
                                                                 : VPX_PLANE_U];
        break;
      case 2:
        ptr =
            yuv_frame->planes[yuv_frame->fmt == VPX_IMG_FMT_YV12 ? VPX_PLANE_U
                                                                 : VPX_PLANE_V];
        break;
      default: ptr = yuv_frame->planes[plane];
    }

    for (r = 0; r < h; ++r) {
      size_t needed = w * bytespp;
      size_t buf_position = 0;
      const size_t left = detect->buf_read - detect->position;
      if (left > 0) {
        const size_t more = (left < needed) ? left : needed;
        memcpy(ptr, detect->buf + detect->position, more);
        buf_position = more;
        needed -= more;
        detect->position += more;
      }
      if (needed > 0) {
        shortread |= (fread(ptr + buf_position, 1, needed, f) < needed);
      }

      ptr += yuv_frame->stride[plane];
    }
  }

  return shortread;
}

vpx_codec_iface_t* GetVP8Decoder() {
    return vpx_codec_vp8_dx();
}

vpx_codec_iface_t* GetVP9Decoder() {
    return vpx_codec_vp9_dx();
}

constexpr VpxInterface vpx_decoders[] = {
  { "vp8", DecoderFourCC::VP8, &GetVP8Decoder },
  { "vp9", DecoderFourCC::VP9, &GetVP9Decoder },
};

size_t get_vpx_decoder_count(void) {
  return sizeof(vpx_decoders) / sizeof(vpx_decoders[0]);
}

const VpxInterface *get_vpx_decoder_by_index(int i) { return &vpx_decoders[i]; }

const VpxInterface *get_vpx_decoder_by_name(const char *name) {
    for (size_t i = 0; i < get_vpx_decoder_count(); ++i) {
        const VpxInterface *const decoder = get_vpx_decoder_by_index(i);
        if (!strcmp(decoder->name, name)) 
            return decoder;
    }
    return nullptr;
}

const VpxInterface *get_vpx_decoder_by_fourcc(uint32_t fourcc) {
    for (size_t i = 0; i < get_vpx_decoder_count(); ++i) {
        const VpxInterface *const decoder = get_vpx_decoder_by_index(i);
        if (decoder->fourcc == fourcc) 
            return decoder;
    }
    return nullptr;
}

int vpx_img_plane_width(const vpx_image_t *img, int plane) {
    if (plane > 0 && img->x_chroma_shift > 0)
        return (img->d_w + 1) >> img->x_chroma_shift;
    else
        return img->d_w;
}
int vpx_img_plane_height(const vpx_image_t *img, int plane) {
    if (plane > 0 && img->y_chroma_shift > 0)
        return (img->d_h + 1) >> img->y_chroma_shift;
    else
        return img->d_h;
}

void vpx_img_write(const vpx_image_t *img, FILE *file) {
    for (size_t plane = 0; plane < 3; ++plane) {
        const unsigned char *buf = img->planes[plane];
        const int stride = img->stride[plane];
        const int w = vpx_img_plane_width(img, plane) *
                      ((img->fmt & VPX_IMG_FMT_HIGHBITDEPTH) ? 2 : 1);
        const int h = vpx_img_plane_height(img, plane);
        int y;

        for (y = 0; y < h; ++y) {
            std::fwrite(buf, 1, w, file);
            buf += stride;
        }
    }
}
bool vpx_img_read(vpx_image_t *img, std::FILE *file) {
    for (size_t plane = 0; plane < 3; ++plane) {
        unsigned char *buf = img->planes[plane];
        const int stride = img->stride[plane];
        const int w = vpx_img_plane_width(img, plane) *
                      ((img->fmt & VPX_IMG_FMT_HIGHBITDEPTH) ? 2 : 1);
        const int h = vpx_img_plane_height(img, plane);
        int y;

        for (y = 0; y < h; ++y) {
            if (std::fread(buf, 1, w, file) != (size_t)w) 
                 return false;
            buf += stride;
        }
    }
    return true;
}

// TODO(dkovalev) change sse_to_psnr signature: double -> int64_t
double sse_to_psnr(double samples, double peak, double sse) {
    static const double kMaxPSNR = 100.0;
    if (sse > 0.0) {
        const double psnr = 10.0 * log10(samples * peak * peak / sse);
        return psnr > kMaxPSNR ? kMaxPSNR : psnr;
    } 
    else
        return kMaxPSNR;
}
int compare_img(const vpx_image_t* const img1, const vpx_image_t* const img2) {
    uint32_t l_w = img1->d_w;
    uint32_t c_w = (img1->d_w + img1->x_chroma_shift) >> img1->x_chroma_shift;
    const uint32_t c_h = (img1->d_h + img1->y_chroma_shift) >> img1->y_chroma_shift;
    int match = 1;
    match &= (img1->fmt == img2->fmt);
    match &= (img1->d_w == img2->d_w);
    match &= (img1->d_h == img2->d_h);

    size_t i;
    for (i = 0; i < img1->d_h; ++i) {
        match &= (memcmp(img1->planes[VPX_PLANE_Y] + i * img1->stride[VPX_PLANE_Y],
            img2->planes[VPX_PLANE_Y] + i * img2->stride[VPX_PLANE_Y],
            l_w) == 0);
    }
    for (i = 0; i < c_h; ++i) {
        match &= (memcmp(img1->planes[VPX_PLANE_U] + i * img1->stride[VPX_PLANE_U],
            img2->planes[VPX_PLANE_U] + i * img2->stride[VPX_PLANE_U],
            c_w) == 0);
    }
    for (i = 0; i < c_h; ++i) {
        match &= (memcmp(img1->planes[VPX_PLANE_V] + i * img1->stride[VPX_PLANE_V],
            img2->planes[VPX_PLANE_V] + i * img2->stride[VPX_PLANE_V],
            c_w) == 0);
    }
    return match;
}

#ifdef min
#undef min
#endif

void find_mismatch(const vpx_image_t* const img1, const vpx_image_t* const img2,
    int yloc[4], int uloc[4], int vloc[4]) {
    const uint32_t bsize = 64;
    const uint32_t bsizey = bsize >> img1->y_chroma_shift;
    const uint32_t bsizex = bsize >> img1->x_chroma_shift;
    const uint32_t c_w =
        (img1->d_w + img1->x_chroma_shift) >> img1->x_chroma_shift;
    const uint32_t c_h =
        (img1->d_h + img1->y_chroma_shift) >> img1->y_chroma_shift;
    int match = 1;
    uint32_t i, j;
    yloc[0] = yloc[1] = yloc[2] = yloc[3] = -1;
    for (i = 0, match = 1; match && i < img1->d_h; i += bsize) {
        for (j = 0; match && j < img1->d_w; j += bsize) {
            int k, l;
            const int si = std::min(i + bsize, img1->d_h) - i;
            const int sj = std::min(j + bsize, img1->d_w) - j;
            for (k = 0; match && k < si; ++k) {
                for (l = 0; match && l < sj; ++l) {
                    if (*(img1->planes[VPX_PLANE_Y] +
                        (i + k) * img1->stride[VPX_PLANE_Y] + j + l) !=
                        *(img2->planes[VPX_PLANE_Y] +
                            (i + k) * img2->stride[VPX_PLANE_Y] + j + l)) {
                        yloc[0] = i + k;
                        yloc[1] = j + l;
                        yloc[2] = *(img1->planes[VPX_PLANE_Y] +
                            (i + k) * img1->stride[VPX_PLANE_Y] + j + l);
                        yloc[3] = *(img2->planes[VPX_PLANE_Y] +
                            (i + k) * img2->stride[VPX_PLANE_Y] + j + l);
                        match = 0;
                        break;
                    }
                }
            }
        }
    }
    uloc[0] = uloc[1] = uloc[2] = uloc[3] = -1;
    for (i = 0, match = 1; match && i < c_h; i += bsizey) {
        for (j = 0; match && j < c_w; j += bsizex) {
            int k, l;
            const int si = std::min(i + bsizey, c_h - i);
            const int sj = std::min(j + bsizex, c_w - j);
            for (k = 0; match && k < si; ++k) {
                for (l = 0; match && l < sj; ++l) {
                    if (*(img1->planes[VPX_PLANE_U] +
                        (i + k) * img1->stride[VPX_PLANE_U] + j + l) !=
                        *(img2->planes[VPX_PLANE_U] +
                            (i + k) * img2->stride[VPX_PLANE_U] + j + l)) {
                        uloc[0] = i + k;
                        uloc[1] = j + l;
                        uloc[2] = *(img1->planes[VPX_PLANE_U] +
                            (i + k) * img1->stride[VPX_PLANE_U] + j + l);
                        uloc[3] = *(img2->planes[VPX_PLANE_U] +
                            (i + k) * img2->stride[VPX_PLANE_U] + j + l);
                        match = 0;
                        break;
                    }
                }
            }
        }
    }
    vloc[0] = vloc[1] = vloc[2] = vloc[3] = -1;
    for (i = 0, match = 1; match && i < c_h; i += bsizey) {
        for (j = 0; match && j < c_w; j += bsizex) {
            int k, l;
            const int si = std::min(i + bsizey, c_h - i);
            const int sj = std::min(j + bsizex, c_w - j);
            for (k = 0; match && k < si; ++k) {
                for (l = 0; match && l < sj; ++l) {
                    if (*(img1->planes[VPX_PLANE_V] +
                        (i + k) * img1->stride[VPX_PLANE_V] + j + l) !=
                        *(img2->planes[VPX_PLANE_V] +
                            (i + k) * img2->stride[VPX_PLANE_V] + j + l)) {
                        vloc[0] = i + k;
                        vloc[1] = j + l;
                        vloc[2] = *(img1->planes[VPX_PLANE_V] +
                            (i + k) * img1->stride[VPX_PLANE_V] + j + l);
                        vloc[3] = *(img2->planes[VPX_PLANE_V] +
                            (i + k) * img2->stride[VPX_PLANE_V] + j + l);
                        match = 0;
                        break;
                    }
                }
            }
        }
    }
}
