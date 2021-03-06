#include "mydhcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

struct proctable
{
    int status;
    int event;
    void (*func)();
} ptab[] = {
    {INIT, NO_EVENT, send_discover},
    {WAIT_OFF, R_OFF_OK, send_request},
    {WAIT_OFF, R_OFF_NG, release},
    {WAIT_OFF, R_TIMEOUT_1st, send_discover},
    {WAIT_OFF, R_TIMEOUT_2nd, release},
    {WAIT_ACK, R_ACK_OK, in_use},
    {WAIT_ACK, R_ACK_NG, release},
    {WAIT_ACK, R_TIMEOUT_1st, send_request},
    {WAIT_ACK, R_TIMEOUT_2nd, release},
    {IN_USE, TTL_2_PASSED, send_ext},
    {IN_USE, R_SIGHUP, release},
    {WAIT_EXT_ACK, R_ACK_OK, in_use},
    {EXIT, NO_EVENT, exited},
    {0, 0, NULL},
};

// global 変数
int status, event;
int alrmflag = 0;
int ttl_counter;
struct itimerval clock;

void alrm_func()
{
    alrmflag++;
}

void sighup_action()
{
    printf("catch sighup\n");
    exit(0);
}

int wait_event(int s, struct sockaddr_in skt)
{
    // アドレス取得前: メッセージ受信をまつ
    // アドレス取得後: SIGALRM or SIGHUP をまつ

    int count;
    char rbuf[STR_MAX];

    struct itimeval for_timer;
    for_timer.tv_sec = 10;
    for_timer.tv_usec = 0;

    fd_set rdfds;

    FD_ZERO(&rdfds);
    FD_SET(s, &rdfds);

    if (select(s + 1, &rdfds, NULL, NULL, &for_timer) < 0)
    {
        perror("select");
        exit(1);
    }
    if (FD_ISSET(s, &rdfds))
    {
        // パケット受信
        if ((count = recvfrom(s, rbuf, sizeof rbuf, 0,
                              (struct sockaddr *)&skt, (socklen_t *)sizeof(skt))) < 0)
            {
                perror("recvfrom");
                exit(1);
            }
        }
    }



    switch (status)
    {
    case WAIT_OFF:
        if (rbuf[0] == '0')
        {
            // Code : 0
            return R_OFF_OK;
        }
        else
        {
            return R_OFF_NG;
        }
        break;
    case WAIT_ACK:
        if ((count = recvfrom(s, rbuf, sizeof rbuf, 0,
                              (struct sockaddr *)&skt, (socklen_t *)sizeof(skt))) < 0)
        {
            perror("recvfrom");
            exit(1);
        }
        if (rbuf[0] == '0')
        {
            return R_ACK_OK;
        }
        else
        {
            return R_ACK_NG;
        }
        break;
    case IN_USE:
        // time spend
        break;

    case WAIT_EXT_ACK:
        break;

    case EXIT:
        break;
    default:
        break;
    }

    // sendto() でサーバのソケットアドレスを指定して送信
    // recvfrom() でサーバからの応答を受診
    // これらを必要なだけ繰り返す.
    return NO_EVENT;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        perror("usage: ./mydhcpc server-IP-address");
        exit(1);
    }
    char *ip = argv[1];

    // ソケットを生成, bindしない
    // 送り先の情報は to とかで設定する.
    // send_toする.
    // discover送って,
    // wait_event入ってそのループ.

    int s, count, datalen;
    struct sockaddr_in skt; //相手のソケット
    socklen_t sktlen;
    sktlen = sizeof(skt);
    char sbuf[STR_MAX];                //送信用バッファ
    in_port_t port = DEFAULT_PORT; // 相手のport
    struct in_addr ipaddr;         //相手のIPaddress

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    // 送り先の情報登録
    // inet_aton("127.0.0.1", &ipaddr);
    inet_aton(ip, &ipaddr);
    skt.sin_family = AF_INET;
    clock.it_interval = 0;
    clock.it_value = 10;


    // skt.sin_port = htons(port);
    // skt.sin_addr.s_addr = htonl(ipaddr.s_addr);
    // skt.sin_addr.s_addr = htonl(INADDR_ANY);

    // timer
    setitimer(ITIMER_REAL, clock, NULL);
    sigaction(SIGALRM, alrm_func);
    sigaction(SIGHUP, sighup_action);

    if ((count = sendto(s, sbuf, datalen, 0,
                        (struct sockaddr *)&skt, sizeof(skt))) < 0)
    {
        perror("sendto");
        exit(1);
    }

    struct proctable *pt;

    for (;;)
    {
        // pause();
        if (alrmflag > 0)
        {
            alrmflag = 0;
            // SIGALRM 受信時の処理

        }
        event = wait_event(s, skt);
        for (pt = ptab; pt->status; pt++)
        {
            if (pt->status == status && pt->event == event)
            {
                (*pt->func)(s, skt);
                break;
            }
        }
        if (pt->status == 0)
        {
            perror("event error");
            exit(1);
        }
    }

    if (close(s) < 0)
    {
        perror("close error");
        exit(1);
    }
    exit(0); // プロセスを終了する.
}

void send_discover(int s, struct sockaddr_in skt)
{
    int count;
    char sbuf[STR_MAX];
    // send discover
    if ((count = sendto(s, sbuf, sizeof(sbuf), 0,
                        (struct sockaddr *)&skt, *(socklen_t *)sizeof(skt))) < 0)
    {
        perror("sendto");
        exit(1);
    }
    printf("client : %d, STATUS: ", s);
    status = WAIT_OFF;
}

void send_request(int s, struct sockaddr_in skt)
{
    // send request
    int count;
    char sbuf[STR_MAX];
    if ((count = sendto(s, sbuf, sizeof(sbuf), 0,
                        (struct sockaddr *)&skt, *(socklen_t *)sizeof(skt))) < 0)
    {
        perror("sendto");
        exit(1);
    }
    status = WAIT_ACK;
}

void send_ext(int s, struct sockaddr_in skt)
{
    // send extent
    int count;
    char sbuf[STR_MAX];
    if ((count = sendto(s, sbuf, sizeof(sbuf), 0,
                        (struct sockaddr *)&skt, *(socklen_t *)sizeof(skt))) < 0)
    {
        perror("sendto");
        exit(1);
    }
    status = WAIT_EXT_ACK;
}

void in_use(int s, struct sockaddr_in skt)
{
    // pass
    return;
}
void release(int s, struct sockaddr_in skt)
{
    // send RELEASE
    int count;
    char sbuf[STR_MAX];
    if ((count = sendto(s, sbuf, sizeof(sbuf), 0,
                        (struct sockaddr *)&skt, *(socklen_t *)sizeof(skt))) < 0)
    {
        perror("sendto");
        exit(1);
    }
    status = EXIT;
}
void exited(int s, struct sockaddr_in skt)
{
    //pass
}
