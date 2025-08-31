#define _WIN32_WINNT 0x0600
#include <Winsock2.h>
#include <pcap.h>
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>

struct ip_header {
    u_char  ihl_and_ver;   // Version (4 bits) + IP header length (4 bits)
    u_char  tos;           // Type of service
    u_short tlen;           // Total length
    u_short id;            
    u_short frag_and_flags; // Flags (3 bits) + Fragment offset (13 bits)
    u_char  ttl;          
    u_char  protocol;      
    u_short checksum;      // Header checksum
    u_long  ip_src;        
    u_long  ip_dst;       
};

struct tcp_header {
    u_short th_sport;      // Source Port
    u_short th_dport;      // Destination Port
    u_int   th_seq;        // Sequence Number
    u_int   th_ack;        // Acknowledgment Number
    u_char  th_off;        // Data Offset
    u_char  th_flags;      // Flags (SYN, ACK, PSH...)
    u_short th_win;        
    u_short th_sum;        
    u_short th_urp;        // Urg Pointer
};
std::string iptostr(u_long ip)
{
    return std::to_string((ip) & 0xFF) + "." +
        std::to_string((ip >> 8) & 0xFF) + "." +
        std::to_string((ip >> 16) & 0xFF) + "." +
        std::to_string((ip >> 24) & 0xFF);
}

void packet_handler(u_char* param, const pcap_pkthdr* header, const u_char* pkt_data)
{
    static int count = 0;
    const struct ip_header* ip = (const struct ip_header*)(pkt_data + 4);
    u_char ip_hl = (ip->ihl_and_ver & 0x0F) * 4;

    if (ip->protocol != 6) return; 

    const struct tcp_header* tcp = (const struct tcp_header*)((u_char*)ip + ip_hl);
    u_short srcPort = ntohs(tcp->th_sport);
    u_short dstPort = ntohs(tcp->th_dport);

    if (srcPort != 5555 && dstPort != 5555) return;

    std::time_t timestamp = header->ts.tv_sec;
    std::tm timeInfo;
    errno_t err = localtime_s(&timeInfo, &timestamp);
    if (err != 0) return;

    char timestr[16];
    std::strftime(timestr, sizeof(timestr), "%H:%M:%S", &timeInfo);
    int millis = header->ts.tv_usec / 1000;

    std::cout << "[" << ++count << "] "
        << "Time: " << timestr << "."
        << std::setfill('0') << std::setw(3) << millis
        << " | "
        << iptostr(ip->ip_src) << ":" << srcPort
        << " → "
        << iptostr(ip->ip_dst) << ":" << dstPort;
    if (tcp->th_flags & 0x02) std::cout << " [SYN]";
    if (tcp->th_flags & 0x10) std::cout << " [ACK]";
    if (tcp->th_flags & 0x08) std::cout << " [PSH]";
    if (tcp->th_flags & 0x01) std::cout << " [FIN]";
    if (tcp->th_flags & 0x04) std::cout << " [RST]";

    
    u_int dataOffset = (tcp->th_off >> 4) * 4;  
    u_int payloadLen = header->caplen - 4 - ip_hl - dataOffset; 

    if (payloadLen > 0)
    {
        const u_char* payload = (const u_char*)tcp + dataOffset;
        std::string msg((const char*)payload, payloadLen);
        std::cout << " | Payload: \"" << msg << "\"" << std::endl;
    }
    else
    {
        std::cout << " | Payload: <none>" << std::endl;
    }

    std::cout << " | Length: " << header->caplen << " bytes" << std::endl;
}

int main()
{
    pcap_if_t* alldevs;
    pcap_if_t* loopback = nullptr;
    char errbuf[PCAP_ERRBUF_SIZE];
    WSADATA wsaData;

    
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cout << "WSAStartup failed!" << std::endl;
        return -1;
    }

    
    if (pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        std::cout << "Error in pcap_findalldevs: " << errbuf << std::endl;
        WSACleanup();
        return -1;
    }
    for (pcap_if_t* d = alldevs; d != nullptr; d = d->next)
    {
        if (d->flags & PCAP_IF_LOOPBACK)
        {
            loopback = d;
            break;
        }
    }

    if (!loopback)
    {
        std::cout << "Loopback adapter not found!" << std::endl;
        pcap_freealldevs(alldevs);
        WSACleanup();
        return -1;
    }
    pcap_t* handle = pcap_open(loopback->name, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, nullptr, errbuf);
    if (!handle)
    {
        std::cout << "Unable to open adapter: " << errbuf << std::endl;
        pcap_freealldevs(alldevs);
        WSACleanup();
        return -1;
    }

    std::cout << "Capturing packets on loopback interface...\n";
    std::cout << "Send a message from the chat app to see traffic!\n\n";

    pcap_loop(handle, 0, packet_handler, nullptr);

    pcap_close(handle);
    pcap_freealldevs(alldevs);
    WSACleanup();
    return 0;
}