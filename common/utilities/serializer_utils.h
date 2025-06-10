#ifndef UTILITIES_SERIALIZER_UTILS_H
#define UTILITIES_SERIALIZER_UTILS_H

#include <vector>
#include <string>
#include <arpa/inet.h>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include "../communication/socket.h"

namespace Utilities {

    void serialize_uint16(std::vector<uint8_t>& buf, uint16_t value);
    void serialize_uint32(std::vector<uint8_t>& buf, uint32_t value);
    void serialize_int(std::vector<uint8_t>& buf, int value);
    void serialize_float(std::vector<uint8_t>& buf, float value);
    void serialize_string(std::vector<uint8_t>& buf, const std::string& str);

    uint8_t deserialize_uint8(Socket& skt);
    uint16_t deserialize_uint16(Socket& skt);
    uint32_t deserialize_uint32(Socket& skt);
    int deserialize_int(Socket& skt);
    float deserialize_float(Socket& skt);
    std::string deserialize_string(Socket& skt);

}

#endif