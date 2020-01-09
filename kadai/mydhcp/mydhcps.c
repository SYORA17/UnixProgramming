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

int status;

int wait_event(int s, struct sockaddr_in skt)
{
    // メッセージ受信のタイムアウトをチェック
    // 使用期限タイムアウトをチェック
    // メッセージ受信をまつ

    char rbuf[STR_MAX]; // 受信用バッファ
    int count;
    if ((count = recvfrom(s, rbuf, sizeof rbuf, 0,
                          (struct sockaddr *)&skt, (socklen_t *)sizeof(skt))) < 0)
    {
        perror("recvfrom");
        exit(1);
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
    int ttl = 0;
    char tmp_ip[STR_MAX];
    char tmp_netmask[STR_MAX];
    int i;

    struct ip_addr *ip_addr_h = malloc(sizeof(struct ip_addr));

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
        if (line == 0)
        {
            ttl = fscanf(buf, "%d", &ttl);
        }
        else
        {
            if (fscanf(buf, "%s %s\n", tmp_ip, tmp_netmask) != EOF)
            {
                struct ip_addr *p;
                p = malloc(sizeof(struct ip_addr));
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
    print_ip_addr(ip_addr_h);
    line--;
    return 0;
    // ソケットは一個作成
    // socket(int domain, int type, int protocol);
    int s, s2, count;
    struct sockaddr_in myskt;      // 自ソケット
    struct sockaddr_in skt;        // 相手のソケット
    in_port_t port = DEFAULT_PORT; // 相手のポート番号
    struct in_addr ipaddr;         // 相手のIPアドレス
    char sbuf[STR_MAX];
    fd_set rdfds;

    inet_aton("127.0.0.1", &ipaddr);

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

    FD_ZERO(&rdfds);
    FD_SET(0, &rdfds);
    FD_SET(s, &rdfds);

    if (select(s + 1, &rdfds, NULL, NULL, NULL) < 0)
    {
        perror("select");
        exit(1);
    }

    if (FD_ISSET(0, &rdfds))
    {
        // 標準入力から入力
    }

    if (FD_ISSET(s, &rdfds))
    {
        // パケット受信
    }

    skt.sin_family = AF_INET;
    skt.sin_port = htons(port);
    skt.sin_addr.s_addr = htonl(ipaddr.s_addr);
    if ((count = sendto(s, sbuf, sizeof sbuf, 0,
                        (struct sockaddr *)&skt, sizeof skt)) < 0)
    {
        perror("sendto");
        exit(1);
    }

    struct proctable *pt;
    int event;

    for (;;)
    {
        event = wait_event(s, skt);
        for (pt = ptab; pt->status; pt++)
        {
            if (pt->status == status && pt->event == event)
            {
                (*pt->func)(/*...*/);
                break;
            }
        }
        if (pt->status == 0)
        {
            // エラー処理;
        }
    }
}

void send_offer_ok(int s, struct sockaddr_in skt)
{
    // create client, alloc IP, send offer ok
    int count;
    char sbuf[STR_MAX];
    if ((count = sendto(s, sbuf, sizeof sbuf, 0,
                        (struct sockaddr *)&skt, sizeof skt)) < 0)
    {
        perror("sendto");
        exit(1);
    }
    status = WAIT_REQ;
}

void send_offer_ng(int s, struct sockaddr_in skt)
{
    // send offer ng,
    status = TERMINATE;
}

void send_ack_ok(int s, struct sockaddr_in skt)
{
    // send ack ok
    status = IN_USE;
}

void send_ack_ng(int s, struct sockaddr_in skt)
{
    // send ack ng
    status = TERMINATE;
}

void resend_offer(int s, struct sockaddr_in skt)
{
    // send offer ok
}

void extent(int s, struct sockaddr_in skt)
{
    // reset ttl, send ack[ok]
}

void terminate(int s, struct sockaddr_in skt)
{
    // recall IP, del client
    status = TERMINATE;
}

void terminated(int s, struct sockaddr_in skt)
{
    // pass
}
