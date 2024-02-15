/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef VPX_TOOLS_COMMON_H_
#define VPX_TOOLS_COMMON_H_

#include <cstdio>

#include "vpx/vpx_codec.h"
#include "vpx/vpx_image.h"
#include "vpx/vpx_integer.h"

#define LITERALU64(hi, lo) ((((uint64_t)hi) << 32) | lo)

#ifndef PATH_MAX
#define PATH_MAX 512
#endif

#define IVF_FRAME_HDR_SZ (4 + 8) /* 4 byte size + 8 byte timestamp */
#define IVF_FILE_HDR_SZ 32

#define RAW_FRAME_HDR_SZ sizeof(uint32_t)

enum DecoderFourCC {
    VP8 = 0x30385056,
    VP9 = 0x30395056
};
enum VideoFileType {
    FILE_TYPE_RAW,
    FILE_TYPE_IVF,
    FILE_TYPE_Y4M,
    FILE_TYPE_WEBM
};
struct FileTypeDetectionBuffer {
    char buf[4];
    size_t buf_read;
    size_t position;
};
struct VpxRational {
    int numerator;
    int denominator;
};
struct VpxInputContext {
    const char *filename;
    std::FILE *file;
    int64_t length;
    struct FileTypeDetectionBuffer detect;
    enum VideoFileType file_type;
    uint32_t width;
    uint32_t height;
    struct VpxRational pixel_aspect_ratio;
    vpx_img_fmt_t fmt;
    vpx_bit_depth_t bit_depth;
    int only_i420;
    uint32_t fourcc;
    struct VpxRational framerate;
};
int read_yuv_frame(struct VpxInputContext *input_ctx, vpx_image_t *yuv_frame);
size_t get_vpx_decoder_count(void);

typedef struct VpxInterface {
    const char* name;
    uint32_t fourcc;
    vpx_codec_iface_t* (*codec_interface)(void);
} VpxInterface;
const VpxInterface* get_vpx_decoder_by_index(int i);
const VpxInterface* get_vpx_decoder_by_name(const char *name);
const VpxInterface* get_vpx_decoder_by_fourcc(uint32_t fourcc);

int vpx_img_plane_width(const vpx_image_t *img, int plane);
int vpx_img_plane_height(const vpx_image_t *img, int plane);
void vpx_img_write(const vpx_image_t *img, std::FILE *file);
bool vpx_img_read(vpx_image_t *img, std::FILE *file);

double sse_to_psnr(double samples, double peak, double mse);
int compare_img(const vpx_image_t *const img1, const vpx_image_t *const img2);
void find_mismatch(const vpx_image_t *const img1, const vpx_image_t *const img2,
                   int yloc[4], int uloc[4], int vloc[4]);

#endif  // VPX_TOOLS_COMMON_H_
