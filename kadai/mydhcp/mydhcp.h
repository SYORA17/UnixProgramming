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
void send_offer_ok(int s, struct sockaddr_in skt);
void send_offer_ng(int s, struct sockaddr_in skt);
void send_ack_ok(int s, struct sockaddr_in skt);
void send_ack_ng(int s, struct sockaddr_in skt);
void resend_offer(int s, struct sockaddr_in skt);
void extent(int s, struct sockaddr_in skt);
void terminate(int s, struct sockaddr_in skt);
void terminated(int s, struct sockaddr_in skt);

// client fsm
// client status
// #define INIT 1
#define WAIT_OFF 112
#define WAIT_ACK 113
// #define IN_USE 4
#define WAIT_EXT_ACK 115
#define EXIT 999

// client event
// #define NO_EVENT 0
#define R_OFF_OK 1000
#define R_OFF_NG 1001
// #define R_TIMEOUT_1st 22
// #define R_TIMEOUT_2nd 23
#define R_ACK_OK 2000
#define R_ACK_NG 2001
#define TTL_2_PASSED 2004
#define R_SIGHUP 3000

// client function
void send_discover(int s, struct sockaddr_in skt);
void send_request(int s, struct sockaddr_in skt);
void send_ext(int s, struct sockaddr_in skt);
void in_use(int s, struct sockaddr_in skt);
void release(int s, struct sockaddr_in skt);
void exited(int s, struct sockaddr_in skt);

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
void print_ip_addr(struct ip_addr *h);
void insert_client_top(struct client *h, struct client *p);
void remove_client(struct client *p);
