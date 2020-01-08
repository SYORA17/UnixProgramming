#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include <netinet/in.h>
#include "mydhcp.h"

// ソケットの生成
// int socket(int domain, int type, int protocol);

struct proctable
{
    int status;
    int event;
    // void (*func)(...);
} ptab[] = {
    {stat1, event1, f_act1},
};

void wait_event(struct sockaddr skt, ) {
    char rbuf[STR_MAX]; // 受信用バッファ
    if ((count = recvfrom(skt, rbuf, sizeof rbuf, 0,
            (struct sockaddr *)&skt, &sktlen)) < 0) {
                perror("recvfrom");
                exit(1);
    }
    in_port_t port = skt->sin_port;
    char ip_str[STR_MAX] = skt->sin_addr;
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


    if (argc != 2) {
        perror("input error");
        exit(1);
    }

    if ((file = fopen(argv[1], "r")) == NULL) {
        perror("cannot open file");
        exit(1);
    }

    // ip_addrの双方向リストにいれる.
    int line = 0;
    while (fgets(buf, STR_MAX, fp) != NULL) {
        if (line == 0) {
            ttl = fscanf(buf, "%d", &ttl);
        } else {
            if (fscanf(buf, "%s %s\n", tmp_ip, tmp_netmask) != EOF) {
                struct ip_addr *p;
                p = malloc(sizeof(struct ip_addr));
                p->ip = tmp_ip;
                p->netmask = tmp_netmask;
                insert_ip_addr(ip_addr_h, p);
            } else {
                perror("config file");
                exit(1);
            }
        }
        line++;
    }
    line--;
    // ソケットは一個作成
    // socket(int domain, int type, int protocol);
    int s, s2, count;
    struct sockaddr_in myskt; // 自ソケット
    struct sockaddr_in skt; // 相手のソケット
    in_port_t port; // 相手のポート番号
    struct in_addr ipaddr; // 相手のIPアドレス
    socklen_t sktlen;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&myskt, 0, sizeof myskt);
    myskt.sin_family = AF_INET;
    myskt.sin_port = htons(51230);
    myskt.sin_addr.s_addr = htonl(INADDRANY);

    // dhcp serverのip addrをバインド
    if (bind(s, (struct sockaddr *)&myskt, sizeof myskt) < 0) {
        perror("bind");
        exit(1);
    }

    sktlen = sizeof skt;

    skt.sin_family = AF_INET;
    skt.sin_port = htons(port);
    skt.sin_addr.s_addr = htonl(ipaddr.s_addr);
    if ((count = sendto(s, sbuf, datalen, 0,
                    (struct sockaddr *)&skt, sizeof skt)) < 0) {
        perror("sendto");
        exit(1);
    }


    struct proctable *pt;
    int event;

    for (;;)
    {
        event = wait_event(/*...*/);
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
            // いらないはず...
            // エラー処理;
        }
    }
}

void f_act1()
{
}
void f_act2()
{
}
