#include "NetworkMessageReader.h"

NetworkMessageReader::NetworkMessageReader(const void* buf, size_t size)
    : m_buffer(reinterpret_cast<const uint8_t*>(buf)),
    m_bufferSize(size) {
    if (m_buffer == nullptr && m_bufferSize > 0) {
        m_hasError = true;
    }
}

std::optional<int8_t> NetworkMessageReader::readChar() {
    if (m_readPos + 1 > m_bufferSize) {
        m_hasError = true;
        return std::nullopt;
    }
    return static_cast<int8_t>(m_buffer[m_readPos++]);
}

std::optional<uint8_t> NetworkMessageReader::readByte() {
    if (m_readPos + 1 > m_bufferSize) {
        m_hasError = true;
        return std::nullopt;
    }
    return m_buffer[m_readPos++];
}

std::optional<int16_t> NetworkMessageReader::readShort() {
    if (m_readPos + 2 > m_bufferSize) {
        m_hasError = true;
        return std::nullopt;
    }
    int16_t val = static_cast<int16_t>(
        m_buffer[m_readPos] |
        (m_buffer[m_readPos + 1] << 8)
        );
    m_readPos += 2;
    return val;
}

std::optional<int32_t> NetworkMessageReader::readLong() {
    if (m_readPos + 4 > m_bufferSize) {
        m_hasError = true;
        return std::nullopt;
    }
    int32_t val = m_buffer[m_readPos] |
        (m_buffer[m_readPos + 1] << 8) |
        (m_buffer[m_readPos + 2] << 16) |
        (m_buffer[m_readPos + 3] << 24);
    m_readPos += 4;
    return val;
}

std::optional<float> NetworkMessageReader::readFloat() {
    if (m_readPos + 4 > m_bufferSize) {
        m_hasError = true;
        return std::nullopt;
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
        if (!c || *c == '\0') {
            break;
        }
        result += static_cast<char>(*c);
    }
    return result;
}

std::optional<float> NetworkMessageReader::readCoord() {
    //GoldSrc should be readShort()
    auto val = readLong();
    if (!val) {
        return std::nullopt;
    }
    return static_cast<float>(*val) * (1.0f / 8.0f);
}

std::optional<float> NetworkMessageReader::readAngle() {
    auto c = readChar();
    if (!c) {
        return std::nullopt;
    }
    return static_cast<float>(*c) * (360.0f / 256.0f);
}

std::optional<float> NetworkMessageReader::readHiresAngle() {
    auto s = readShort();
    if (!s) {
        return std::nullopt;
    }
    return static_cast<float>(*s) * (360.0f / 65536.0f);
}