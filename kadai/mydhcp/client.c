#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>


int main()
{

    // ソケットを生成, bindしない
    // 送り先の情報は to とかで設定する.
    // send_toする.
    // discover送って,
    // wait_event入ってそのループ.

    int s, count;
    struct sockaddr_in myskt;
    struct sockaddr_in skt;
    char buf[512];
    socklen_t sktlen;

    // s = socket(); bind();
    sktlen = sizeof(skt);
    if ((count = recvfrom(s, buf, sizeof buf, 0,
            (struct sockaddr *)&skt, &sktlen)) < 0 ) {
        perror("recvfrom");
        exit(1);
    }
}