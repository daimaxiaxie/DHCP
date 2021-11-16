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

DHCPParse::DHCPParse() : size(512), cookie(false) {
    buf = new unsigned char[512];
    msg = (Message *) buf;
}

DHCPParse::DHCPParse(int size) : size(512), cookie(false) {
    buf = new unsigned char[size];
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
    if (len < sizeof(Message)) {
        throw std::length_error("DHCP parse : data incomplete");
    }
    memcpy(buf, data, len);
    Debug((unsigned char *) buf, len);
    if (len > sizeof(Message)) {
        int pos = 236;
        if (len > sizeof(Message) + 4 && ntohl(*((unsigned int *) &buf[pos])) == Magic) {
            cookie = true;
            pos += 4;
        }
        //std::cout << int(buf[pos]) << " " << int(buf[pos + 1]) << " " << int(buf[pos + 2]) << std::endl;
        option_parse(pos, len);
    }
    return msg;
}

void DHCPParse::option_parse(int start, int end) {
    int cur = start;
    while (cur < end) {
        int length = 0;
        short code = buf[cur];
        if (Options.count(code)) {
            auto val = Options.at(code);
            if (!std::get<1>(val).empty()) {
                std::cout << std::get<1>(val) << " : ";
            }

            switch (std::get<2>(val)) {
                case DataType::Raw: {
                    for (int i = 0; i < buf[cur + 1]; ++i) {
                        if (i % 8 == 0 && i != 0) {
                            std::cout << std::endl;
                        }
                        std::cout << (unsigned short) buf[cur + 2 + i] << "\t";
                    }
                    cur += buf[cur + 1] + 2;
                    break;
                }
                case DataType::IP: {
                    in_addr addr;
                    for (int i = 0; i < buf[cur + 1]; i += 4) {
                        std::memcpy(&addr, &buf[cur + 2 + i], 4);
                        std::cout << inet_ntoa(addr) << std::endl;
                    }
                    cur += buf[cur + 1] + 2;
                    break;
                }
                case DataType::String: {
                    std::cout << std::string(&buf[cur + 2], &buf[cur + buf[cur + 1] + 2]);
                    cur += buf[cur + 1] + 2;
                    break;
                }
                case DataType::Short: {
                    std::cout << ntohs(*((unsigned short *) &buf[cur + 2]));
                    cur += 4;
                    break;
                }
                case DataType::Int: {
                    std::cout << ntohl(*((unsigned int *) &buf[cur + 2]));
                    cur += 6;
                    break;
                }
                case DataType::DHCPType: {
                    if (MessageType.count(buf[cur + 2])) {
                        int type = buf[cur + 2];
                        std::cout << "DHCP " << MessageType.at(type) << std::endl;
                    } else {
                        std::cerr << "DHCP parse : unknown DHCP msg type";
                    }
                    cur += 3;
                    break;
                }
                case DataType::End: {
                    return;
                }
                case DataType::Pad: {
                    ++cur;
                    break;
                }
                default:
                    break;
            }
            std::cout << std::endl;
        } else {
            length = int(buf[cur + 1]);
            std::clog << "DHCP parse : unknown option type " << int(buf[cur]) << " : ";
            for (int i = 0; i < length; ++i) {
                std::clog << short(buf[cur + 2 + i]) << " ";
            }
            std::clog << std::endl;
            //cur += length;
            return;
        }
    }

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
        std::cout << (unsigned int) data[i] << "\t ";
    }
    std::cout << std::endl;
}



