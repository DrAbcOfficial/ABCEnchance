/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include <string>
#include <corecrt_malloc.h>
#include "ivfdec.h"
#include "memle.h"

constexpr char*IVF_SIGNATURE = "DKIF";

bool file_is_ivf(struct VpxInputContext *input_ctx) {
    char raw_hdr[32];
    bool is_ivf = false;
    if (std::fread(raw_hdr, 1, 32, input_ctx->file) == 32) {
        if (!memcmp(IVF_SIGNATURE, raw_hdr, 4)) {
            is_ivf = true;
            if (mem_get_le16<int>(raw_hdr + 4) != 0) {
                std::fprintf(stderr,
                    "Error: Unrecognized IVF version! This file may not"
                    " decode properly.");
            }
            input_ctx->fourcc = mem_get_le32<uint32_t>(raw_hdr + 8);
            input_ctx->width = mem_get_le16<int>(raw_hdr + 12);
            input_ctx->height = mem_get_le16<int>(raw_hdr + 14);
            input_ctx->framerate.numerator = mem_get_le32<int>(raw_hdr + 16);
            input_ctx->framerate.denominator = mem_get_le32<int>(raw_hdr + 20);
            const static constexpr auto fix_framerate = [](int* num, int* den) {
                // Some versions of vpxenc used 1/(2*fps) for the timebase, so
                // we can guess the framerate using only the timebase in this
                // case. Other files would require reading ahead to guess the
                // timebase, like we do for webm.
                if (*den > 0 && *den < 1000000000 && *num > 0 && *num < 1000) {
                    // Correct for the factor of 2 applied to the timebase in the encoder.
                    if (*num & 1)
                        *den *= 2;
                    else
                        *num /= 2;
                }
                else {
                    // Don't know FPS for sure, and don't have readahead code
                    // (yet?), so just default to 30fps.
                    *num = 30;
                    *den = 1;
                }
            };
            fix_framerate(&input_ctx->framerate.numerator, &input_ctx->framerate.denominator);
        }
    }

    if (!is_ivf) {
        std::rewind(input_ctx->file);
        input_ctx->detect.buf_read = 0;
    } 
    else
        input_ctx->detect.position = 4;
    return is_ivf;
}
bool ivf_read_frame(std::FILE *infile, uint8_t **buffer, size_t *bytes_read, size_t *buffer_size) {
    char raw_header[IVF_FRAME_HDR_SZ] = { 0 };
    size_t frame_size = 0;

    if (std::fread(raw_header, IVF_FRAME_HDR_SZ, 1, infile) != 1) {
        if (!std::feof(infile)) 
            warn("Failed to read frame size");
    } 
    else {
        frame_size = mem_get_le32<size_t>(raw_header);
        if (frame_size > 256 * 1024 * 1024) {
            warn("Read invalid frame size (%u)", (unsigned int)frame_size);
            frame_size = 0;
        }
        if (frame_size > *buffer_size) {
            uint8_t *new_buffer = static_cast<uint8_t*>(realloc(*buffer, 2 * frame_size));
            if (new_buffer) {
                *buffer = new_buffer;
                *buffer_size = 2 * frame_size;
            } 
            else {
                warn("Failed to allocate compressed data buffer");
                frame_size = 0;
            }
        }
    }
    if (!std::feof(infile)) {
        if (std::fread(*buffer, 1, frame_size, infile) != frame_size) {
            warn("Failed to read full frame");
            return true;
        }
        *bytes_read = frame_size;
        return false;
    }
    return true;
}