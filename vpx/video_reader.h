/*
 *  Copyright (c) 2014 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VPX_VIDEO_READER_H_
#define VPX_VIDEO_READER_H_

#include "./video_common.h"

class CIVFVideoReader {
public:
	CIVFVideoReader();
	CIVFVideoReader(const char* filename);
	~CIVFVideoReader();
	bool Open(const char* filename);
	void Close();
	bool ReadFrame();
	void ResetToBegine();
	bool IsValid();
	const uint8_t* GetFrame(size_t* size);
	const VpxVideoInfo* GetInfo();
private:
	VpxVideoInfo m_pInfo;
	std::FILE* m_pFile = nullptr;
	uint8_t* m_pBuffer = nullptr;
	std::fpos_t m_pFpos;
	size_t m_iBufferSize = 0;
	size_t m_iFrameSize = 0;
};
#endif  // VPX_VIDEO_READER_H_
