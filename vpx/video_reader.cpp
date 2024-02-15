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
#include "video_reader.h"
#include "memle.h"

extern void ConsoleWarnMessage(char* format, ...);
constexpr char *const kIVFSignature = "DKIF";

CIVFVideoReader::CIVFVideoReader(){
}
CIVFVideoReader::CIVFVideoReader(const char* filename){
    Open(filename);
}
void CIVFVideoReader::Close() {
    std::fclose(m_pFile);
    free(m_pBuffer);
}
CIVFVideoReader::~CIVFVideoReader(){
    //Close();
}
bool CIVFVideoReader::Open(const char* filename){
    char header[32];
    m_pFile = std::fopen(filename, "rb");
    if (!m_pFile) {
        std::fprintf(stderr, "%s can't be opened.\n", filename);  // Can't open file
        return false;
    }
    if (std::fread(header, 1, 32, m_pFile) != 32) {
        std::fprintf(stderr, "File header on %s can't be read.\n", filename);  // Can't read file header
        return false;
    }
    if (memcmp(kIVFSignature, header, 4) != 0) {
        std::fprintf(stderr, "The IVF signature on %s is wrong.\n", filename);  // Wrong IVF signature
        return false;
    }
    if (mem_get_le16<int>(header + 4) != 0) {
        std::fprintf(stderr, "%s uses the wrong IVF version.\n", filename);  // Wrong IVF version
        return false;
    }
    std::fgetpos(m_pFile, &m_pFpos);
    m_pInfo.codec_fourcc = mem_get_le32<uint32_t>(header + 8);
    m_pInfo.frame_width = mem_get_le16<int>(header + 12);
    m_pInfo.frame_height = mem_get_le16<int>(header + 14);
    m_pInfo.time_base.numerator = mem_get_le32<int>(header + 16);
    m_pInfo.time_base.denominator = mem_get_le32<int>(header + 20);
    m_pInfo.frame_count = mem_get_le32<int>(header + 24);
    return true;
}
bool CIVFVideoReader::ReadFrame(){
    char raw_header[IVF_FRAME_HDR_SZ] = { 0 };
    size_t frame_size = 0;

    if (std::fread(raw_header, IVF_FRAME_HDR_SZ, 1, m_pFile) != 1) {
        if (!std::feof(m_pFile))
            ConsoleWarnMessage("VPX: Failed to read frame size");
    }
    else {
        frame_size = mem_get_le32<size_t>(raw_header);
        if (frame_size > 256 * 1024 * 1024) {
            ConsoleWarnMessage("VPX: Read invalid frame size (%u)", (unsigned int)frame_size);
            frame_size = 0;
        }
        if (frame_size > m_iBufferSize) {
            uint8_t* new_buffer = static_cast<uint8_t*>(realloc(m_pBuffer, 2 * frame_size));
            if (new_buffer) {
                m_pBuffer = new_buffer;
                m_iBufferSize = 2 * frame_size;
            }
            else {
                ConsoleWarnMessage("VPX: Failed to allocate compressed data buffer");
                frame_size = 0;
            }
        }
    }
    if (!std::feof(m_pFile)) {
        if (std::fread(m_pBuffer, 1, frame_size, m_pFile) != frame_size) {
            ConsoleWarnMessage("VPX: Failed to read full frame");
            return false;
        }
        m_iFrameSize = frame_size;
        return true;
    }
    return false;
}
void CIVFVideoReader::ResetToBegine(){
    if(IsValid())
        std::fsetpos(m_pFile, &m_pFpos);
}
bool CIVFVideoReader::IsValid(){
    return m_pFile != nullptr;
}
const uint8_t* CIVFVideoReader::GetFrame(size_t* size){
    if (size)
        *size = m_iFrameSize;
    return m_pBuffer;
}
const VpxVideoInfo* CIVFVideoReader::GetInfo(){
    return &m_pInfo;
}
