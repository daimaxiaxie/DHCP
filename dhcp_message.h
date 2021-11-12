//
// Created by lenovo on 2021/11/12.
//

#ifndef DHCP_DHCP_MESSAGE_H
#define DHCP_DHCP_MESSAGE_H

#include "header.h"

struct Option {
    char op;
    char len;
};

struct Message {
    char op;
    char type;
    char len;
    char ops;
    unsigned int id;
    unsigned short secs;
    unsigned short flag;
    unsigned int caddr;
    unsigned int yaddr;
    in_addr saddr; //dhcp server ip
    unsigned int gaddr; //first dhcp relay ip
    char cmac[16];
    char sname[64];         //server name
    char file[128];

    friend std::ostream &operator<<(std::ostream &out, const Message &message);
};

class DHCPParse {
public:
    DHCPParse();

    explicit DHCPParse(int size);

    ~DHCPParse();

    void clear();

    const Message *parse(char *data, int len);

private:
    char *buf;
    int size;
    Message *msg;
};


void Debug(unsigned char *data, int len);


#endif //DHCP_DHCP_MESSAGE_H
