#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#define STR_MAX 64
#define SOCK_MAX 32

#define DEFAULT_PORT 51230

// server fsm
// server status
#define NO_EVENT 0
#define INIT 1
#define WAIT_REQ 2
#define IN_USE 3
#define TERMINATE 9

//server event
#define R_DISCOVER_IP 10
#define R_DISCOVER_NO_IP 11
#define R_REQUEST_ACK_OK 20
#define R_REQUEST_ACK_NG 21
#define R_TIMEOUT_1st 22
#define R_TIMEOUT_2nd 23
#define R_EXT_OK 30
#define R_EXT_NG 31
#define R_TIMEOUT 31

// server function
void send_offer_ok();
void send_offer_ng();
void send_ack_ok();
void send_ack_ng();
void resend_offer();
void extent();
void terminate();
void terminated();

// client fsm
// client status
// #define INIT 1
#define WAIT_OFF 2
#define WAIT_ACK 3
// #define IN_USE 4
#define WAIT_EXT_ACK 5
#define EXIT 9

// client event
// #define NO_EVENT 0
#define R_OFF_OK 10
#define R_OFF_NG 11
// #define R_TIMEOUT_1st 22
// #define R_TIMEOUT_2nd 23
#define R_ACK_OK 20
#define R_ACK_NG 21
#define TTL_2_PASSED 24
#define R_SIGHUP 30

// client function
void send_discover();
void send_request();
void send_ext();
void in_use();
void release();
void exited();

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

struct dhcph
{
    uint8_t type;
    uint8_t code;
    uint16_t ttl;
    in_addr_t address;
    in_addr_t netmask;
};

// struct in_addr
// {
//     in_addr_t s_addr;
// };

struct ip_addr
{
    struct ip_addr *fp;
    struct ip_addr *bp;
    struct in_addr ip;
    struct in_addr netmask;
};

void insert_ip_addr_top(struct ip_addr *h, struct ip_addr *p);
void insert_ip_addr_tail(struct ip_addr *h, struct ip_addr *p);
void remove_ip_addr(struct ip_addr *p);
