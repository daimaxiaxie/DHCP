//
// Created by lenovo on 2021/11/12.
//

#include "dhcp_message.h"

std::ostream &operator<<(std::ostream &out, const Message &message) {
    switch (message.op) {
        case 1:
            out << "Request Message";
            break;
        case 2:
            out << "Response Message";
            break;
        default:
            out << "Unknown op type : " << message.op;
            break;
    }
    out << std::endl << "DHCP server ip : " << inet_ntoa(message.saddr) << std::endl;
    out << "server name : " << std::string(message.sname) << std::endl;
    return out;
}

DHCPParse::DHCPParse() : size(512) {
    buf = new char[512];
    msg = (Message *) buf;
}

DHCPParse::DHCPParse(int size) : size(512) {
    buf = new char[size];
    msg = (Message *) buf;
}

DHCPParse::~DHCPParse() {
    msg = nullptr;
    delete[] buf;
}

void DHCPParse::clear() {
    memset(buf, 0, size);
}

const Message *DHCPParse::parse(char *data, int len) {
    clear();
    if (len > size) {
        throw std::out_of_range("DHCP parse : too large");
        //return;
    }
    memcpy(buf, data, len);
    Debug((unsigned char *) buf, len);
    //std::cout << std::dec << buf[20] << " " << buf[21] << " " << buf[22] << " " << buf[23] << std::endl;
    return msg;
}

void Debug(unsigned char *data, int len) {
    std::cout << "Debug data : " << len << std::endl;
    for (int i = 0; i < len; ++i) {
        if (i % 4 == 0 && i != 0) {
            if (i == 108) {
                std::cout << std::endl << "---- file ----";
            } else if (i == 236) {
                std::cout << std::endl << "---- options ----";
            }
            std::cout << std::endl;
        }
        std::cout << int(data[i]) << "\t ";
    }
    std::cout << std::endl;
}



