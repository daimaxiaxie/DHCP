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
        while (pos < len) {
            int length = 0;
            //std::cout << int(buf[pos]) << std::endl;
            switch (buf[pos]) {
                case 1: {
                    std::vector<unsigned short> mask(&buf[pos + 2], &buf[pos + 5]);
                    std::cout << "Subnet mask : ";
                    std::copy(mask.begin(), mask.end(), std::ostream_iterator<unsigned short>(std::cout, "."));
                    std::cout << std::endl;
                    pos += 6;
                    break;
                }
                case 3: {
                    std::cout << "Default gateway : ";
                    for (int i = 0; i < buf[pos + 1]; i += 4) {
                        std::vector<unsigned short> gateway(&buf[pos + 2 + i], &buf[pos + 5 + i]);
                        std::copy(gateway.begin(), gateway.end(),
                                  std::ostream_iterator<unsigned short>(std::cout, "."));
                        std::cout << std::endl;
                    }
                    pos += buf[pos + 1] + 2;
                    break;
                }
                case 6: {
                    std::cout << "DNS server : ";
                    for (int i = 0; i < buf[pos + 1]; i += 4) {
                        std::vector<unsigned short> dns(&buf[pos + 2 + i], &buf[pos + 5 + i]);
                        std::copy(dns.begin(), dns.end(), std::ostream_iterator<unsigned short>(std::cout, "."));
                        std::cout << std::endl;
                    }
                    pos += buf[pos + 1] + 2;
                    break;
                }
                case 12: {
                    std::cout << "Host Name : " << std::string(&buf[pos + 2], &buf[pos + buf[pos + 1] + 2])
                              << std::endl;
                    pos += buf[pos + 1] + 2;
                    break;
                }
                case 15: {
                    std::cout << "Domain name : ";
                    std::cout << std::string(&buf[pos + 2], &buf[pos + buf[pos + 1] + 2]) << std::endl;
                    pos += buf[pos + 1] + 2;
                    break;
                }
                case 42: {
                    std::cout << "NTP server : ";
                    for (int i = 0; i < buf[pos + 1]; i += 4) {
                        std::vector<unsigned short> tmp(&buf[pos + 2 + i], &buf[pos + 5 + i]);
                        std::copy(tmp.begin(), tmp.end(), std::ostream_iterator<unsigned short>(std::cout, "."));
                        std::cout << std::endl;
                    }
                    pos += buf[pos + 1] + 2;
                    break;
                }
                case 44: {
                    std::cout << "WINS server : ";
                    for (int i = 0; i < buf[pos + 1]; i += 4) {
                        std::vector<unsigned short> tmp(&buf[pos + 2 + i], &buf[pos + 5 + i]);
                        std::copy(tmp.begin(), tmp.end(), std::ostream_iterator<unsigned short>(std::cout, "."));
                        std::cout << std::endl;
                    }
                    pos += buf[pos + 1] + 2;
                    break;
                }
                case 50: {
                    in_addr addr;
                    std::memcpy(&addr, &buf[pos + 2], 4);
                    std::cout << "Requested IP Address : " << inet_ntoa(addr) << std::endl;
                    pos += 6;
                    break;
                }
                case 51: {
                    std::cout << "Valid lease : " << ntohl(*((unsigned int *) &buf[pos + 2])) << " s" << std::endl;
                    pos += 6;
                    break;
                }
                case 53: {
                    switch (buf[pos + 2]) {
                        case 1:
                            std::cout << "DHCP Discover" << std::endl;
                            break;
                        case 2:
                            std::cout << "DHCP Offer" << std::endl;
                            break;
                        case 3:
                            std::cout << "DHCP Request" << std::endl;
                            break;
                        case 4:
                            std::cout << "DHCP Decline" << std::endl;
                            break;
                        case 5:
                            break;
                        case 6:
                            break;
                        case 7:
                            break;
                        case 8:
                            break;
                        default:
                            std::clog << "DHCP parse : unknown DHCP msg type" << std::endl;
                            break;
                    }
                    pos += 3;
                    break;
                }
                case 55: {
                    std::cout << "Parameter Request List : ";
                    for (int i = 0; i < buf[pos + 1]; ++i) {
                        if (i % 8 == 0 && i != 0) {
                            std::cout << std::endl;
                        }
                        std::cout << (unsigned short) buf[pos + 2 + i] << "\t ";
                    }
                    std::cout << std::endl;
                    pos += buf[pos + 1] + 2;
                    break;
                }
                case 57: {
                    std::cout << "Maximum DHCP Message Size : " << ntohl(*((unsigned short *) &buf[pos + 2]))
                              << std::endl;
                    pos += 4;
                    break;
                }
                case 58: {
                    std::cout << "Renewal Time Value : " << ntohl(*((unsigned int *) &buf[pos + 2])) << " s"
                              << std::endl;
                    pos += 6;
                    break;
                }
                case 60: {
                    std::cout << "Vendor class identifier : ";
                    for (int i = 0; i < buf[pos + 1]; ++i) {
                        if (i % 8 == 0 && i != 0) {
                            std::cout << std::endl;
                        }
                        std::cout << (unsigned short) buf[pos + 2 + i] << "\t ";
                    }
                    std::cout << std::endl;
                    pos += buf[pos + 1] + 2;
                    break;
                }
                case 61: {
                    std::cout << "Client identifier : ";
                    for (int i = 0; i < buf[pos + 1]; ++i) {
                        if (i % 8 == 0 && i != 0) {
                            std::cout << std::endl;
                        }
                        std::cout << (unsigned short) buf[pos + 2 + i] << "\t ";
                    }
                    std::cout << std::endl;
                    pos += buf[pos + 1] + 2;
                    break;
                }
                case 255:
                    ++pos;
                    break;
                default:
                    length = int(buf[pos + 1]);
                    std::clog << "DHCP parse : unknown option type " << int(buf[pos]) << " : ";
                    for (int i = 0; i < length; ++i) {
                        std::clog << int(buf[pos + 2 + i]) << " ";
                    }
                    std::clog << std::endl;
                    pos += length;
                    return msg;
            }
        }
    }
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
        std::cout << (unsigned int) data[i] << "\t ";
    }
    std::cout << std::endl;
}



