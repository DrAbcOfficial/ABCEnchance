/*
 *  Copyright (c) 2013 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef VPX_IVFDEC_H_
#define VPX_IVFDEC_H_
#include "./tools_common.h"
bool file_is_ivf(struct VpxInputContext *input);
bool ivf_read_frame(std::FILE *infile, uint8_t **buffer, size_t *bytes_read, size_t *buffer_size);

#endif  // VPX_IVFDEC_H_
