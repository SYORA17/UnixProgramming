#include "mydhcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

// ソケットの生成
// int socket(int domain, int type, int protocol);

struct proctable
{
    int status;
    int event;
    void (*func)();
} ptab[] = {
    {INIT, R_DISCOVER_IP, send_offer_ok},
    {INIT, R_DISCOVER_NO_IP, send_offer_ng},
    {WAIT_REQ, R_REQUEST_ACK_OK, send_ack_ok},
    {WAIT_REQ, R_REQUEST_ACK_NG, send_ack_ng},
    {WAIT_REQ, R_TIMEOUT_1st, send_offer_ok},
    {WAIT_REQ, R_TIMEOUT_2nd, terminate},
    {IN_USE, R_EXT_OK, extent},
    {IN_USE, R_EXT_NG, terminate},
    {IN_USE, R_TIMEOUT, terminate},
    {TERMINATE, NO_EVENT, terminated},
    {0, 0, NULL},
};

int status, alrmflag = 0;
int ttl = 0;
struct client *c;

void alrm_func()
{
    alrmflag++;
}

int wait_event(int s, struct sockaddr_in *skt)
{
    // メッセージ受信のタイムアウトをチェック
    // 使用期限タイムアウトをチェック
    // メッセージ受信をまつ

    // タイムアウトチェック

    char rbuf[STR_MAX]; // 受信用バッファ
    int count;
    // struct client *c;

    struct dhcph *r_dh;

    if ((count = recvfrom(s, rbuf, sizeof rbuf, 0,
                          (struct sockaddr *)skt, (socklen_t *)sizeof(*skt))) < 0)
    {
        if (alrmflag > 0) {
            return R_SIGNAL;
        } else {
            perror("recvfrom");
            exit(1);
        }
    }
    r_dh = (struct dhcph *)rbuf;

    c = search_client(client_list, skt);
    if (c == NULL)
    {
        c = malloc(sizeof(struct client));
        memset(c, 0, sizeof(struct client));
        c->status = INIT;
        c->skt = *skt;
        c->id = skt->sin_addr;
        insert_client_top(client_list, c);
    }

    // パケット受信
    switch (c->status)
    {
    case INIT:
        // wait discover
        if (r_dh->type == 1)
        {
            //search ip.
            if (search_ip_addr(ip_addr_h) == 0)
            {
                // requestしているipaddressのセットを返す.
                return R_DISCOVER_IP;
            }
            else
            {
                return R_DISCOVER_NO_IP;
            }
        } else {
            return 0;
        }
        break;
    case WAIT_REQ:
        // wait request
        if (rbuf[0] == '0')
        {
            // if ok
            return R_REQUEST_ACK_OK;
        }
        else
        {
            return R_REQUEST_ACK_NG;
        }

    case IN_USE:
        if (rbuf[0] == '0')
        {
            return R_EXT_OK;
        }
        else
        {
            return R_EXT_NG;
        }
        break;
    case TERMINATE:
        return NO_EVENT;
        break;
    default:
        break;
    }

    // in_port_t port = skt.sin_port;
    // char ip_str[STR_MAX] = skt.sin_addr;
    // ipアドレスと, skt.sin_port を見る.
}

int main(int argc, char *argv[])
{
    // config file 読みとり
    // 1 : ttl
    // 2行めから ip_addr netmask
    // ...

    FILE *fp;
    char buf[STR_MAX];
    char tmp_ip[STR_MAX];
    char tmp_netmask[STR_MAX];
    int i;

    ip_addr_h = malloc(sizeof(struct ip_addr));
    ip_addr_h->fp = ip_addr_h;
    ip_addr_h->bp = ip_addr_h;

    client_list = (struct client *)malloc(sizeof(struct client));
    client_list->fp = client_list;
    client_list->bp = client_list;

    if (argc != 2)
    {
        perror("input error");
        exit(1);
    }

    if ((fp = fopen(argv[1], "r")) == NULL)
    {
        perror("cannot open file");
        exit(1);
    }

    // ip_addrの双方向リストにいれる.
    int line = 0;
    while (fgets(buf, STR_MAX, fp) != NULL)
    {
        memset(tmp_ip, 0, STR_MAX);
        memset(tmp_netmask, 0, STR_MAX);
        if (line == 0)
        {
            sscanf(buf, "%d", &ttl);
        }
        else
        {
            if (sscanf(buf, "%s %s\n", tmp_ip, tmp_netmask) != EOF)
            {
                struct ip_addr *p;
                char s_n[32], s_i[32];
                strncpy(inet_ntoa(p->netmask):q, s_n, 32);
                strncpy(inet_ntoa(p->ip), s_i, 32);
                printf("%s %s\n", tmp_ip, tmp_netmask);
                p = (struct ip_addr *)malloc(sizeof(struct ip_addr));
                if (inet_aton(tmp_ip, &p->ip) < 0)
                {
                    perror("config file");
                    exit(1);
                }
                if (inet_aton(tmp_netmask, &p->netmask) < 0)
                {
                    perror("config file");
                    exit(1);
                }
                insert_ip_addr_top(ip_addr_h, p);
            }
            else
            {
                perror("config file");
                exit(1);
            }
        }
        line++;
    }
    printf("%d\n", ttl);
    print_ip_addr(ip_addr_h);
    line--;
    // ソケットは一個作成
    // socket(int domain, int type, int protocol);
    int s, s2, count;
    struct sockaddr_in myskt;      // 自ソケット
    struct sockaddr_in skt;        // 相手のソケット
    in_port_t port = DEFAULT_PORT; // 相手のポート番号
    struct in_addr ipaddr;         // 相手のIPアドレス
    char sbuf[STR_MAX];
    struct itimerval clock;
    struct timeval clock_t;
    clock_t.tv_sec = 1;
    clock_t.tv_usec = 0;
    clock.it_interval = clock_t;
    clock.it_value = clock_t;
    inet_aton("127.0.0.1", &ipaddr);

    sigaction(SIGALRM, alrm_func, NULL);

    // 1秒おきにSIGALRMが起きる.
    setitimer(ITIMER_REAL, &clock, NULL);

        if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    // 自ソケットに設定
    memset(&myskt, 0, sizeof myskt);
    myskt.sin_family = AF_INET;
    myskt.sin_port = htons(DEFAULT_PORT);
    myskt.sin_addr.s_addr = htonl(INADDR_ANY);

    // dhcp serverのip addrをバインド
    if (bind(s, (struct sockaddr *)&myskt, sizeof myskt) < 0)
    {
        perror("bind");
        exit(1);
    }

    // skt.sin_family = AF_INET;
    // skt.sin_port = htons(port);
    // skt.sin_addr.s_addr = htonl(ipaddr.s_addr);
    // if ((count = sendto(s, sbuf, sizeof sbuf, 0,
    //                     (struct sockaddr *)&skt, sizeof skt)) < 0)
    // {
    //     perror("sendto");
    //     exit(1);
    // }

    struct proctable *pt;
    int event;

    for (;;)
    {
        event = wait_event(s, &skt);
        if (alrmflag){
            dec_ttl();
            check_ttl(s);
            alrmflag = 0;
        }
        for (pt = ptab; pt->status; pt++)
        {
            if (pt->status == status && pt->event == event)
            {
                (*pt->func)(s, &skt);
                break;
            }
        }
        if (pt->status == 0)
        {
            // エラー処理;
            fprintf(stderr, "server main loop error\n");
            exit(1);
        }
    }
}

void send_offer_ok(int s, struct sockaddr_in *skt)
{
    // create client, alloc IP, send offer ok
    int count;
    char sbuf[STR_MAX];
    struct ip_addr *addr = ip_addr_h->fp;
    struct dhcph *s_dh = (struct dhcph *)sbuf;
    s_dh->type = 2;
    s_dh->code = 0;
    s_dh->ttl = ttl;
    s_dh->address = addr->ip.s_addr;
    s_dh->netmask = addr->netmask.s_addr;
    remove_ip_addr(addr);
    if ((count = sendto(s, sbuf, sizeof sbuf, 0,
                        (struct sockaddr *)&skt, sizeof skt)) < 0)
    {
        perror("sendto");
        exit(1);
    }
    printf("status: \n");
    c->status = WAIT_REQ;
    c->addr = addr->ip;
    c->netmask = addr->netmask;
    c->ttl = ttl;
    c->ttlcounter = ttl;
}

void send_offer_ng(int s, struct sockaddr_in *skt)
{
    // send offer ng,
    status = TERMINATE;
}

void send_ack_ok(int s, struct sockaddr_in *skt)
{
    // send ack ok
    status = IN_USE;
}

void send_ack_ng(int s, struct sockaddr_in *skt)
{
    // send ack ng
    status = TERMINATE;
}

void resend_offer(int s, struct sockaddr_in *skt)
{
    // send offer ok
}

void extent(int s, struct sockaddr_in *skt)
{
    // reset ttl, send ack[ok]
}

void terminate(int s, struct sockaddr_in *skt)
{
    // recall IP, del client
    status = TERMINATE;
}

void terminated(int s, struct sockaddr_in *skt)
{
    // pass
}
