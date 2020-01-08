#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// udp communication
// offer / request

#define STR_MAX 64
#define SOCK_MAX 32

struct client
{
    struct client *fp;
    struct client *bp;
    short status;
    int ttlcounter;
    // bellow : network byte order
    struct in_addr id;
    struct in_addr addr;
    struct in_addr netmask;
    uint16_t ttl;
    //struct time val // 再送のための送信時刻
};

struct client client_list;

struct dhcph {
    uint8_t type;
    uint8_t code;
    uint16_t ttl;
    in_addr_t address;
    in_addr_t netmask;
};

struct in_addr {
    in_addr_t s_addr;
};

struct ip_addr
{
    struct ip_addr *fp;
    struct ip_addr *bp;
    struct in_addr ip;
    struct in_addr netmask;
};





void insert_ip_addr(struct ip_addr *h, struct ip_addr *p);
void remove_ip_addr(struct ip_addr *p);