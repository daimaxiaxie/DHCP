//
// Created by lenovo on 2021/11/16.
//

#ifndef DHCP_OPTIONS_H
#define DHCP_OPTIONS_H

#include <unordered_map>
#include <tuple>

const std::unordered_map<int, std::string> MessageType{
        {1, "Discover"},
        {2, "Offer"},
        {3, "Request"},
        {4, "Decline"},
        {5, "ACK"},
        {6, "NAK"},
        {7, "Release"},
        {8, "Inform"}
};

enum DataType {
    Raw,
    IP,
    String,
    Short,
    Int,
    DHCPType,
    End,
    Pad,
};

enum LengthType {
    L0,
    L1,
    L2,
    L4,
    L4X,
    N,
};

const std::unordered_map<int, std::tuple<LengthType, std::string, DataType>> Options{
        {0,   std::make_tuple<LengthType, std::string, DataType>(L0, "", Pad)},
        {1,   std::make_tuple<LengthType, std::string, DataType>(L4, "Subnet mask", IP)},
        {3,   std::make_tuple<LengthType, std::string, DataType>(L4X, "Default gateway", IP)},
        {6,   std::make_tuple<LengthType, std::string, DataType>(L4X, "DNS server", IP)},
        {12,  std::make_tuple<LengthType, std::string, DataType>(N, "Client host Name", String)},
        {15,  std::make_tuple<LengthType, std::string, DataType>(N, "Domain name", String)},
        {42,  std::make_tuple<LengthType, std::string, DataType>(L4X, "NTP server", IP)},
        {44,  std::make_tuple<LengthType, std::string, DataType>(L4X, "WINS server", IP)},
        {50,  std::make_tuple<LengthType, std::string, DataType>(L4, "Requested IP Address", IP)},
        {51,  {L4, "Valid lease",               Int}},
        {53,  {L1, "",                          DHCPType}},
        {55,  {N,  "Parameter Request List",    Raw}},
        {57,  {L2, "Maximum DHCP Message Size", Short}},
        {58,  {L4, "Renewal Time Value",        Int}},
        {60,  {N,  "Vendor class identifier",   Raw}},
        {61,  {N,  "Client identifier",         Raw}},
        {255, {L0, "",                          End}}
};

#endif //DHCP_OPTIONS_H
