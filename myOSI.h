#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <numeric>
#pragma comment(lib, "ws2_32.lib")es like uint8_t (an unsigned 8-bit integer, i.e.,
// exactly one byte, values 0–255). This matters a lot in networking code: a plain 
// int might be 2, 4, or 8 bytes depending on the platform/compiler, but network
// protocols care about exact byte sizes. uint8_t guarantees "this is one byte, 
// no more, no less" — which is why it's the standard type for representing raw 
// bytes/bits in networking simulations.

//this is simulating raw bits getting received off the wire
//taar pe se data aa raha hai

// // ----------------------------------old without using socket---------------------
// std::vector<uint8_t> physicalLayerSend(const std::vector<uint8_t>& frame)
// {
//     std::cout << "[Physical] Sending " << frame.size() << " bytes to the wire.\n";
//     return frame;
// }
//-----------------------------------old code ------------------------------------

bool physicalLayerSend(SOCKET sock , const std::vector<uint8_t>& frame){
    uint32_t len = htonl(static_cast<uint32_t>(frame.size()));
    send(sock,reinterpret_cast<const char*>(&len) , sizeof(len),0);
    send(sock, reinterpret_cast<const char*> (frame.data()), static_cast<int>(frame.size()), 0);
    std::cout << "[Physical] sending" << frame.size() << "bytes to the wire.\n";
    return true;
}
//------------------------------------------old osi model -----------
// std::vector<uint8_t> physicalLayerReceive(const std::vector<uint8_t>& wireData)
// {
//     std::cout << "[Physical] Received " << wireData.size() << " bytes from the wire.\n";
//     return wireData;
// }
//-----------------------------------------new socket model---------------

std::vector<uint8_t> physicalLayerReceive(SOCKET sock)
{
    uint32_t len = 0;
    recv(sock, reinterpret_cast<char*>(&len),sizeof(len),0);
    len = ntohl(len);

    std::vector<uint8_t> frame(len);
    recv(sock, reinterpret_cast<char*>(frame.data()),static_cast<int>(len), 0);
    

    std::cout << "[Phusical] Recieved" << frame.size() << "bytes from the wire.\n";
    return frame;
}
// ----------------------------------------------------------------------------------------------------------------------------
//data Link layer
uint8_t calculateChecksum(const std::vector<uint8_t>& data)
{
    uint32_t sum = std::accumulate(data.begin(), data.end(), 0u);
    return static_cast<uint8_t>(sum%256);
}

// std::vector<uint8_t> dataLinkSend(const std::vector<uint8_t>& payload)
// {
//     std::vector<uint8_t> rawBytes(payload.begin(),payload.end());
//     uint8_t checksum = calculateChecksum(rawBytes);

//     std::vector<uint8_t> frame;
//     frame.push_back(static_cast<uint8_t>(rawBytes.size()));
//     frame.push_back(checksum);
//     frame.insert(frame.end(),rawBytes.begin(),rawBytes.end());


//     std::cout <<"[DataLink] framed" << rawBytes.size()<<"bytes with Checksum" << (int) checksum <<"\n";
//     return physicalLayerSend(frame);
    
// }

bool dataLinkSend(SOCKET sock, const std::vector<uint8_t>& payload)
{
    std::vector<uint8_t> rawBytes(payload.begin(), payload.end());
    uint8_t checksum = calculateChecksum(rawBytes);

    std::vector<uint8_t> frame;
    frame.push_back(static_cast<uint8_t>(rawBytes.size()));
    frame.push_back(checksum);
    frame.insert(frame.end(), rawBytes.begin(), rawBytes.end());

    std::cout << "[DataLink] framed" << rawBytes.size() << "bytes with Checksum" << (int)checksum << "\n";
    return physicalLayerSend(sock, frame); // just pass sock along
}
// unraping the frame and checksum and return the orgiin string
// ---------------------------------------old code ---------------------------
// std ::vector<uint8_t> dataLinkReceive(const std :: vector <uint8_t>& wireFrame)
// {
//     std:: vector<uint8_t> frame = physicalLayerReceive(wireFrame);

//     uint8_t length = frame[0];
//     uint8_t receivedChecksum = frame[1];
//     std::vector<uint8_t> payload(frame.begin()+2,frame.end());

//     uint8_t computedChecksum = calculateChecksum(payload);

//     if(computedChecksum != receivedChecksum)
//     {
//         std::cout << "[Datalink] ERROR: checksum mismatch! Frame corrupted.\n";

//     }else{
//         std::cout << "[DATAlink] checkSum ok. Frame length matches" << (int)length << "\n";
//     }

//     return payload;

    
// }
// -----------------------------------------------new socket---------------------------
std::vector<uint8_t> dataLinkReceive(SOCKET sock)
{
    std::vector<uint8_t> frame = physicalLayerReceive(sock);

    uint8_t length = frame[0];
    uint8_t receivedChecksum = frame[1];
    std::vector<uint8_t> payload(frame.begin()+2, frame.end());

    uint8_t comptedChecksum = calculateChecksum(payload);

    if(comptedChecksum != receivedChecksum)
    {   
        std::cout<<"[DataLink] ERROR: checksum mismatch: frame corrupted. \n";
    }
    else{
        std::cout <<"[DataLink] checkSum ok. Frame length Match" <<(int)length << "\n";

    }
    return payload;
}

// //--------------------------------------------purana kuda-----------------
// std::vector<uint8_t> networkSend(const std::string& data, uint8_t srcAddr ,uint8_t destAddr)
// {
//         std::vector<uint8_t> rawBytes(data.begin(), data.end());
//         std::vector<uint8_t> packet;
//         packet.push_back(srcAddr);
//         packet.push_back(destAddr);
//         packet.push_back(64);
//         packet.insert(packet.end(),rawBytes.begin(),rawBytes.end());

//         std::cout<<"[Network] Packet built" << (int)srcAddr << "->" << (int)destAddr
//                 <<", TTL = 64, payload=" << rawBytes.size() << "bytes\n";
        
//         return dataLinkSend(packet);
// }
//-----------------------------------------new one-----------------
bool networkSend(SOCKET sock ,const std::string& data, uint8_t srcAddr, uint8_t destAddr)
{
    std::vector<uint8_t> rawBytes(data.begin(), data.end());
    std::vector<uint8_t> packet;
    packet.push_back(srcAddr);
    packet.push_back(destAddr);
    packet.push_back(64);
    packet.insert(packet.end(), rawBytes.begin(),rawBytes.end());

    std::cout<<"[NETWORK] PacketBuilt" <<(int)srcAddr <<"->" <<(int)destAddr << ", TTL = 64, payload= " << rawBytes.size() << "bytes\n";

    return dataLinkSend(sock,packet); // pass sock along
}
//---------------------------------------old receive data in bytes ----------------
// std:: string networkReceive(const std::vector<uint8_t>& wireFrame)
// {
//     std::vector<uint8_t> packet = dataLinkReceive(wireFrame);

//     uint8_t srcAddr = packet[0];
//     uint8_t destAddr = packet[1];
//     uint8_t tt1 = packet[2];
//     std::vector<uint8_t> payload(packet.begin() + 3, packet.end());
//     std::cout << "[NETWORK] Packet received:" << (int)srcAddr << "->" << (int)destAddr
//                 << ", TTL= " << (int)tt1 << "\n";

//     if(tt1 == 0)
//     {
//         std::cout << "[Network] ERROR : TTL expired, packet droped .\n";
//         return "";
//     }

//     return std::string(payload.begin(),payload.end());
// }
//-------------------------------------new socket method------------------------------

std::string networkReceive(SOCKET sock)
{
    std::vector<uint8_t> packet = dataLinkReceive(sock);
    uint8_t srcAddr = packet[0];
    uint8_t destAddr = packet[1];
    uint8_t tt1 = packet[2];
    std::vector<uint8_t> payload(packet.begin()+3, packet.end());
    std::cout<<"[NETWORK] Packet recieved" << (int)srcAddr << "->" << (int)destAddr <<", TTL = " << (int)tt1 << "\n";

    if(tt1 == 0)
    {
        std::cout<<"[NETWORK] ERROR : TTL expired, packet droped ,\n";
        return "";
    }
    return std::string(payload.begin(), payload.end());
}

//-----------------------------------old code-------------------------------------------------------
// std::vector<uint8_t> transportSend(const std::string data, uint8_t srcAddr,
//     uint8_t destAddr, uint16_t srcPort, uint16_t destPort)
// {
//     // std::vector<uint8_t> rawBytes(data.begin() , data.end());
//     // std::vector<uint8_t> segment;
//     // segment.push_back(srcPort);
//     // segment.push_back(destPort);
//     // segment.insert(segment.end(),rawBytes.begin(),rawBytes.end());

//     // std::cout<<"[Transport] segment built, port" <<(int)srcPort << "-> " <<(int)destPort <<"\n";

//     // std::string segmentStr(segment.begin(), segment.end());
//     // return networkSend(segmentStr,srcAddr,destAddr);

//     std::vector<uint8_t> rawBytes(data.begin(),data.end());
//     std::vector<uint8_t> segment;

//     segment.push_back(static_cast<uint8_t>(srcPort >> 8)); // high bytes
//     segment.push_back(static_cast<uint8_t>(srcPort & 0xFF)); // low bytes
//     segment.push_back(static_cast<uint8_t>(destPort >> 8));
//     segment.push_back(static_cast<uint8_t>(destPort & 0xFF));
//     segment.insert(segment.end(), rawBytes.begin(), rawBytes.end());

//     std::cout << "[transport] Segment built, port" << srcPort <<"->" << destPort<< "\n";
    
//     std::string segmentStr(segment.begin(),segment.end());
//     return networkSend(segmentStr, srcAddr , destAddr);
// }
//---------------------------------------new code -------------------------------
bool transportSend(SOCKET sock,const std::string& data,uint8_t srcAddr, uint8_t destAddr , uint16_t srcPort, uint16_t destPort)
{
    std::vector<uint8_t> rawBytes(data.begin(), data.end());
    std::vector<uint8_t> segment;

    segment.push_back(static_cast<uint8_t>(srcPort>>8));
    segment.push_back(static_cast<uint8_t>(srcPort & 0xFF));
    segment.push_back(static_cast<uint8_t>(destPort >>8));
    segment.push_back(static_cast<uint8_t>(destPort & 0xFF));
    segment.insert(segment.end(), rawBytes.begin(), rawBytes.end());

    std::cout<<"[Transport] Segment built, port" << srcPort <<"->" <<destPort<< "\n";
    std::string segmentStr(segment.begin(), segment.end());
    return networkSend(sock, segmentStr, srcAddr, destAddr);
}
//========================================old code--------------------------------------

// std::string transportReceive(const std::vector<uint8_t>& wireFrame)
// {
//     std::string packetStr = networkReceive(wireFrame);
//     std::vector<uint8_t> segment(packetStr.begin(), packetStr.end());

//     uint16_t srcPort = (static_cast<uint16_t>(segment[0] << 8)) | segment[1];
//     uint16_t destPort = (static_cast<uint16_t>(segment[2] << 8)) | segment[3];

//     std::vector<uint8_t> payload(segment.begin() + 4, segment.end());
//     std::cout <<"[Transport] Segment received port" <<srcPort << "->" << destPort <<"\n";


//     return std::string(payload.begin(), payload.end());
// }
// -------------------------------------socket code ----------------------------
std::string transportReceive(SOCKET sock)
{
    std::string packetStr = networkReceive(sock);
    std::vector<uint8_t> segment(packetStr.begin(), packetStr.end());

    uint16_t srcPort = (static_cast<uint16_t>(segment[0] << 8)) | segment[1];
    uint16_t destPort = (static_cast<uint16_t>(segment[2] << 8)) | segment[3];

    std::vector<uint8_t> payload(segment.begin() +4 ,segment.end());
    std::cout<<"[Transport] Segment received port" << srcPort << "->" << destPort << "\n";

    return std::string(payload.begin(), payload.end());
}

// std::string transportReceive(const std:: vector<uint8_t>& wireFrame)
// {
//     std::string packetStr = networkReceive(wireFrame);
//     std::vector<uint8_t> segment(packetStr.begin(),packetStr.end());

    // uint8_t arcPort = segment[0];
    // uint8_t destPort = segment[1];
    // std::vector<uint8_t> segment(segment.begin(),packetStr.end());

//     uint8_t srcPort = segment[0];
//     uint8_t destPort = segment[1];
//     std::vector<uint8_t> payload(segment.begin()+2 , segment.end());

//     std::cout << "[Transport] Segment received port" <<(int)srcPort << "->" <<(int)destPort << "\n";

//     return std::string(payload.begin(),payload.end());
    
// }

// std::vector<uint8_t> transportSend()

// ----------------------session layer using normal bytes ----------------------------------
// std::vector<uint8_t> sessionSend(const std::string& data, uint8_t srcAddr, uint8_t destAddr,
//                                   uint16_t srcPort, uint16_t destPort, uint8_t sessionId)
// {
//     std::cout << "[Session] Establishing session ID " << (int)sessionId << "...\n";

//     // Build a new string: sessionId byte + original message
//     std::string sessionData;
//     sessionData.push_back(static_cast<char>(sessionId));
//     sessionData += data;

//     return transportSend(sessionData, srcAddr, destAddr, srcPort, destPort);
// }

// std::string sessionReceive(const std::vector<uint8_t>& wireFrame)
// {
//     std::string sessionData = transportReceive(wireFrame);

//     uint8_t sessionId = static_cast<uint8_t>(sessionData[0]);
//     std::string data = sessionData.substr(1); // everything after the first byte

//     std::cout << "[Session] Session ID " << (int)sessionId << " confirmed. Closing session.\n";

//     return data;
// }
//-------------------------------session layer using sockets--------------------
bool sessionSend(SOCKET sock, const std::string& data, uint8_t srcAddr, uint8_t destAddr, uint16_t srcPort, uint16_t destPort, uint8_t sessionId)
{
    std::cout<<"[SESSION] Establishing session ID" << (int)sessionId<<"...\n";
    std::string sessionData;
    sessionData.push_back(static_cast<char>(sessionId));
    sessionData = data;
    return transportSend(sock, sessionData, srcAddr, destAddr, srcPort, destPort);
}

std::string sessionReceive(SOCKET sock)
{
    std::string sessionData = transportReceive(sock);
    uint8_t sessionId = static_cast<uint8_t>(sessionData[0]);
    std::string data = sessionData.substr(1);
    std::cout<<"[SESSION] Session ID" << (int)sessionId << "confirmed.Closing session.\n";
    return data;
}
const uint8_t XOR_KEY = 0x5A;
// //-----------------------------------old presentation layer-----------------------



////----------------------------------------new Presentaiton layer-------------------

std::string presentationEncode(const std::string& data)
{
    std::string transformed = data;
    for(char& c : transformed)
    {
        c = static_cast<char>(static_cast<uint8_t>(c)^XOR_KEY);
    }
    std:: cout << "[Presentation] Applied  XOR to"<< data.size() << "bytes\n";


    return transformed;

}

std:: string presentationDecode(const std::string& data)
{
    std::string original = data;
    for(char& c : original)
    {
        c = static_cast<char>(static_cast<uint8_t>(c) ^ XOR_KEY);

    }

    std::cout<<"[Presentation] Reversed XOR on" << data.size() << "bytes\n";
    return original;
}
bool presentationSend(SOCKET sock, const std::string& data,uint8_t srcAddr, uint8_t destAddr, uint16_t srcPort, uint16_t destPort, uint8_t sessionID)
{
    std::string encoded = presentationEncode(data);
    return sessionSend(sock,encoded,srcAddr, destAddr, srcPort,destPort, sessionID);
}
std::string presentationReceive(SOCKET sock)
{
    std::string encoded = sessionReceive(sock);
    return presentationDecode(encoded);
}


// ----------------------------------old application layer-----------------

// void applicationReceive(const std::vector<uint8_t>& wireFrame); // forward declaration

// void applicationSend(const std::string& message, uint8_t srcAddr, uint8_t destAddr,
//                       uint16_t srcPort, uint16_t destPort, uint8_t sessionId)
// {
//     std::cout << "[Application] Sending message: \"" << message << "\"\n";
//     std::vector<uint8_t> wire = presentationSend(message, srcAddr, destAddr, srcPort, destPort, sessionId);
//     applicationReceive(wire);
// }

// void applicationReceive(const std::vector<uint8_t>& wireFrame)
// {
//     std::string message = presentationReceive(wireFrame);
//     std::cout << "[Application] Message received: \"" << message << "\"\n";
// }

// -------------------------------------------new application layer ----------------------------

void applicationSend(SOCKET sock, const std::string& message,uint8_t srcAddr, uint8_t destAddr, uint16_t srcPort, uint16_t destPort, uint8_t sessionId)
{
    std::cout<<"[Application] Sending message: \" " << message << "\"\n";
    presentationSend(sock,message,srcAddr, destAddr,srcPort, destPort, sessionId);
}

std::string applicationReceive(SOCKET sock)
{
    std::string message = presentationReceive(sock);
    std::cout<<"[APPLICATION] Message received: \"" << message <<"\"\n";
    return message;
}

