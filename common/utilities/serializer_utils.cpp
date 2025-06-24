#include "serializer_utils.h"


namespace Utilities {

    void serialize_uint16(std::vector<uint8_t>& buf, uint16_t value) {
        uint16_t value_net = htons(value);
        buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&value_net), reinterpret_cast<uint8_t*>(&value_net) + sizeof(value_net));
    }

    void serialize_uint32(std::vector<uint8_t>& buf, uint32_t value) {
        uint32_t value_net = htonl(value);
        buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&value_net), reinterpret_cast<uint8_t*>(&value_net) + sizeof(value_net));
    }

    void serialize_int(std::vector<uint8_t>& buf, int value) {
        int value_net = htonl(value);
        buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&value_net), reinterpret_cast<uint8_t*>(&value_net) + sizeof(value_net));
    }

    void serialize_float(std::vector<uint8_t>& buf, float value) {
        uint32_t value_net;
        std::memcpy(&value_net, &value, sizeof(float));
        value_net = htonl(value_net);
        buf.insert(buf.end(), reinterpret_cast<uint8_t*>(&value_net), reinterpret_cast<uint8_t*>(&value_net) + sizeof(value_net));
    }

    void serialize_string(std::vector<uint8_t>& buf, const std::string& str) {
        uint16_t size = static_cast<uint16_t>(str.size());
        serialize_uint16(buf, size);
        buf.insert(buf.end(), str.begin(), str.end());
    }

    uint8_t deserialize_uint8(Socket& skt) {
        uint8_t value_net;
        if (skt.recvall(&value_net, sizeof(value_net)) == 0) {
            throw std::runtime_error("Error receiving uint16_t");
        }
        return value_net; 
    }

    uint16_t deserialize_uint16(Socket& skt) {
        uint16_t value_net;
        if (skt.recvall(&value_net, sizeof(value_net)) == 0) {
            throw std::runtime_error("Error receiving uint16_t");
        }
        return ntohs(value_net);
    }

    uint32_t deserialize_uint32(Socket& skt){
        uint32_t value_net;
        if (skt.recvall(&value_net, sizeof(value_net)) == 0) {
            throw std::runtime_error("Error receiving uint16_t");
        }
        return ntohl(value_net);
    }

    int deserialize_int(Socket& skt){
        int value_net;
        if (skt.recvall(&value_net, sizeof(value_net)) == 0) {
            throw std::runtime_error("Error receiving uint16_t");
        }
        return ntohl(value_net);
    }

    float deserialize_float(Socket& skt) {
        uint32_t value_net;
        if (skt.recvall(&value_net, sizeof(value_net)) == 0) {
            throw std::runtime_error("Error receiving uint32_t");
        }
        value_net = ntohl(value_net);

        float value;
        std::memcpy(&value, &value_net, sizeof(float));

        return value;
    }

    std::string deserialize_string(Socket& skt){
        uint16_t size = deserialize_uint16(skt);

        if (size == 0) {
            return "";
        }

        std::vector<char> buffer(size);
        if (skt.recvall(buffer.data(), size) == 0) {
            throw std::runtime_error("Error receiving string data");
        }
        return std::string(buffer.begin(), buffer.end());
    }

}