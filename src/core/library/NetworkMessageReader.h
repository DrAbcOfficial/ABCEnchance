#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <bit>

class NetworkMessageReader {
public:
    NetworkMessageReader(const void* buf, size_t size);

    bool readOK() const noexcept { return !m_hasError; }

    int8_t readChar();    // 有符号字符
    uint8_t readByte();   // 无符号字节
    int16_t readShort();  // 短整数
    int16_t readWord() { return readShort(); } // 同short
    int32_t readLong();   // 长整数
    float readFloat();    // 浮点数
    std::string readString();            // 字符串
    float readCoord();    // 坐标
    float readAngle();    // 角度
    float readHiresAngle(); // 高精度角度

private:
    const uint8_t* m_buffer = nullptr;   // 缓冲区
    size_t m_bufferSize = 0;             // 缓冲区大小
    size_t m_readPos = 0;                // 当前读取位置
    bool m_hasError = false;             // 错误状态
};