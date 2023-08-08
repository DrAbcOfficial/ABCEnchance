/*
 *  Copyright (c) 2014 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include <cstdio>
#include <string>
#include <corecrt_malloc.h>
#include "ivfdec.h"
#include "video_reader.h"
#include "memle.h"

constexpr char *const kIVFSignature = "DKIF";

struct VpxVideoReaderStruct {
  VpxVideoInfo info;
  std::FILE *file;
  uint8_t *buffer;
  size_t buffer_size;
  size_t frame_size;
};
VpxVideoReader *vpx_video_reader_open(const char *filename) {
    char header[32];
    VpxVideoReader *reader = nullptr;
    std::FILE * file = std::fopen(filename, "rb");
    if (!file) {
        std::fprintf(stderr, "%s can't be opened.\n", filename);  // Can't open file
        return nullptr;
    }
    if (std::fread(header, 1, 32, file) != 32) {
        std::fprintf(stderr, "File header on %s can't be read.\n", filename);  // Can't read file header
        return nullptr;
    }
    if (memcmp(kIVFSignature, header, 4) != 0) {
        std::fprintf(stderr, "The IVF signature on %s is wrong.\n", filename);  // Wrong IVF signature
        return nullptr;
    }
    if (mem_get_le16<int>(header + 4) != 0) {
        std::fprintf(stderr, "%s uses the wrong IVF version.\n", filename);  // Wrong IVF version
        return nullptr;
    }
    reader = static_cast<VpxVideoReader*>(calloc(1, sizeof(*reader)));
    if (!reader) {
        std::fprintf(stderr, "Can't allocate VpxVideoReader\n");  // Can't allocate VpxVideoReader
        return nullptr;
    }
     reader->file = file;
    reader->info.codec_fourcc = mem_get_le32<uint32_t>(header + 8);
    reader->info.frame_width = mem_get_le16<int>(header + 12);
    reader->info.frame_height = mem_get_le16<int>(header + 14);
    reader->info.time_base.numerator = mem_get_le32<int>(header + 16);
    reader->info.time_base.denominator = mem_get_le32<int>(header + 20);
    reader->info.frame_count = mem_get_le32<int>(header + 24);
    return reader;
}
void vpx_video_reader_close(VpxVideoReader *reader) {
    if (reader) {
        std::fclose(reader->file);
        free(reader->buffer);
        free(reader);
    }
}
bool vpx_video_reader_read_frame(VpxVideoReader *reader) {
  return !ivf_read_frame(reader->file, &reader->buffer, &reader->frame_size, &reader->buffer_size);
}
const uint8_t *vpx_video_reader_get_frame(VpxVideoReader *reader, size_t *size) {
    if (size) 
        *size = reader->frame_size;
    return reader->buffer;
}
const VpxVideoInfo *vpx_video_reader_get_info(VpxVideoReader *reader) {
  return &reader->info;
}
