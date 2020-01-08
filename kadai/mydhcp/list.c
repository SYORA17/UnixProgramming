#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include "mydhcp.h"

void insert_ip_addr(struct ip_addr *h, struct ip_addr *p) {
    while (h->fp != h) {
        h = h->fp;
    }
    p->bp = h->bp;
    p->fp = h;
    h->bp->fp = p;
    h->bp = p;
}

void remove_ip_addr(struct ip_addr *p) {
    p->bp->fp = p->fp;
    p->fp->bp = p->bp;
    free(p);
}
