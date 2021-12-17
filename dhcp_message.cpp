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

DHCPParse::DHCPParse() : size(1024), cookie(false) {
    buf = new unsigned char[1024];
    msg = (Message *) buf;
}

DHCPParse::DHCPParse(int size) : size(size), cookie(false) {
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
    //Debug((unsigned char *) buf, len);
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
                        std::cout << "DHCP " << MessageType.at(type);
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


DHCPGenerate::DHCPGenerate(int fd) : fd(fd) {
    buf = new unsigned char[2048];
    std::default_random_engine r;
    xid = r();
    RandomMAC();
    Identifier();
}

DHCPGenerate::~DHCPGenerate() {
    delete[] buf;
}

void DHCPGenerate::Show() {
    std::cout << "Xid : " << xid << std::endl;
    std::cout << "MAC : ";
    for (int i = 0; i < 6; ++i) {
        std::cout << (unsigned short) mac[i] << "\t";
    }
    std::cout << std::endl;
    std::cout << "Identifier : ";
    for (int i = 0; i < 7; ++i) {
        std::cout << (unsigned short) identifier[i] << "\t";
    }
    std::cout << std::endl;
}

void DHCPGenerate::Discover() {
    std::memset(buf, 0, 2048);
    Message *msg = (Message *) buf;
    msg->op = 1;
    msg->type = 1;
    msg->len = 6;
    msg->ops = 0;
    msg->id = xid;
    msg->secs = 0;
    msg->flag = htons(0x8000);

    msg->caddr = 0;
    msg->yaddr = 0;
    msg->gaddr = 0;
    std::memcpy(&(msg->cmac), &mac, 6);

    std::memset(&(msg->sname), 0, 64);
    std::memset(&(msg->file), 0, 128);

    int pos = 236;
    SetInt(&buf[pos], htonl(Magic));
    pos += 4;

    buf[pos] = 53;
    buf[pos + 1] = 1;
    buf[pos + 2] = 1;
    pos += 3;

    buf[pos] = 57;
    buf[pos + 1] = 2;
    SetShort(&buf[pos + 2], htons(512));
    pos += 4;

    buf[pos] = 61;
    buf[pos + 1] = 7;
    std::memcpy(&buf[pos + 2], identifier, 7);
    pos += 9;

    buf[pos] = 12;
    buf[pos + 1] = 8;
    std::memcpy(&buf[pos + 2], &"DHCPTest", 8);
    pos += 10;

    buf[pos] = 55;
    buf[pos + 1] = 8;
    buf[pos + 2] = 1;//subnet mask
    buf[pos + 3] = 6;//dns
    buf[pos + 4] = 15;//domain name
    buf[pos + 5] = 44;//wins server
    buf[pos + 6] = 3;//router
    buf[pos + 7] = 33;//static router
    buf[pos + 8] = 150;//tftp
    buf[pos + 9] = 43;//vendor
    pos += 10;

    buf[pos] = 255;
    //Debug(buf,1024);

    Send("255.255.255.255", pos + 1);
}

void DHCPGenerate::RandomMAC() {
    //std::default_random_engine e;
    for (int i = 0; i < 6; ++i) {
        mac[i] = rand() % 255;
    }
}

void DHCPGenerate::Identifier() {
    identifier[0] = 0x01;
    identifier[1] = 'S';
    identifier[2] = 'i';
    identifier[3] = 'm';
    identifier[4] = 'u';
    identifier[5] = (rand() % (122 - 48 + 1)) + 48;
    identifier[6] = (rand() % 75) + 48;
}

void DHCPGenerate::SetInt(void *dest, const int val) {
    memcpy(dest, &val, 4);
}

void DHCPGenerate::SetShort(void *dest, short val) {
    memcpy(dest, &val, 2);
}

void DHCPGenerate::Send(std::string ip, int size) {
    int port = 67;
    sockaddr_in addr{};
    socklen_t addr_len = sizeof(addr);
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (ip == "255.255.255.255") {
        addr.sin_addr.s_addr = INADDR_BROADCAST;
    } else {
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
    }
    //int val = IP_PMTUDISC_DO;
    //setsockopt(fd, IPPROTO_IP, IP_MTU_DISCOVER, &val, sizeof(val));
    sendto(fd, buf, size, 0, (sockaddr *) &addr, addr_len);
    std::cout << "UDP send " << strerror(errno) << std::endl;
}


