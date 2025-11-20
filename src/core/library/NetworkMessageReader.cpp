#include "metahook.h"
#include "utility/util.h"
#include "NetworkMessageReader.h"

NetworkMessageReader::NetworkMessageReader(const void* buf, size_t size)
    : m_buffer(reinterpret_cast<const uint8_t*>(buf)),
    m_bufferSize(size) {
    if (m_buffer == nullptr && m_bufferSize > 0) {
        m_hasError = true;
    }
}

int8_t NetworkMessageReader::readChar() {
    if (m_readPos + 1 > m_bufferSize) {
        m_hasError = true;
		DebugLog("try read char from %d, but size is %d\n", m_readPos, m_bufferSize);
        return 0;
    }
    return static_cast<int8_t>(m_buffer[m_readPos++]);
}

uint8_t NetworkMessageReader::readByte() {
    if (m_readPos + 1 > m_bufferSize) {
        m_hasError = true;
        DebugLog("try read byte from %d, but size is %d\n", m_readPos, m_bufferSize);
        return 0;
    }
    return m_buffer[m_readPos++];
}

int16_t NetworkMessageReader::readShort() {
    if (m_readPos + 2 > m_bufferSize) {
        m_hasError = true;
        DebugLog("try read short from %d, but size is %d\n", m_readPos, m_bufferSize);
        return 0;
    }
    int16_t val = static_cast<int16_t>(
        m_buffer[m_readPos] |
        (m_buffer[m_readPos + 1] << 8)
        );
    m_readPos += 2;
    return val;
}

int32_t NetworkMessageReader::readLong() {
    if (m_readPos + 4 > m_bufferSize) {
        m_hasError = true;
        DebugLog("try read long from %d, but size is %d\n", m_readPos, m_bufferSize);
        return 0;
    }
    int32_t val = m_buffer[m_readPos] |
        (m_buffer[m_readPos + 1] << 8) |
        (m_buffer[m_readPos + 2] << 16) |
        (m_buffer[m_readPos + 3] << 24);
    m_readPos += 4;
    return val;
}

float NetworkMessageReader::readFloat() {
    if (m_readPos + 4 > m_bufferSize) {
        m_hasError = true;
        DebugLog("try read float from %d, but size is %d\n", m_readPos, m_bufferSize);
        return 0;
    }
    uint32_t bytes = static_cast<uint32_t>(
        m_buffer[m_readPos] |
        (m_buffer[m_readPos + 1] << 8) |
        (m_buffer[m_readPos + 2] << 16) |
        (m_buffer[m_readPos + 3] << 24)
        );
    m_readPos += 4;
    return std::bit_cast<float>(bytes);
}

std::string NetworkMessageReader::readString() {
    std::string result;
    while (m_readPos < m_bufferSize) {
        auto c = readChar();
        if (!c || c == '\0') {
            break;
        }
        result += static_cast<char>(c);
    }
    return result;
}

float NetworkMessageReader::readCoord() {
    //GoldSrc should be readShort()
    auto val = readLong();
    if (!val) {
        DebugLog("try read coord from %d, but size is %d\n", m_readPos, m_bufferSize);
        return NAN;
    }
    return static_cast<float>(val) * (1.0f / 8.0f);
}

float NetworkMessageReader::readAngle() {
    auto c = readChar();
    if (!c) {
        DebugLog("try read angle from %d, but size is %d\n", m_readPos, m_bufferSize);
        return NAN;
    }
    return static_cast<float>(c) * (360.0f / 256.0f);
}

float NetworkMessageReader::readHiresAngle() {
    auto s = readShort();
    if (!s) {
        DebugLog("try read hires angle from %d, but size is %d\n", m_readPos, m_bufferSize);
        return NAN;
    }
    return static_cast<float>(s) * (360.0f / 65536.0f);
}