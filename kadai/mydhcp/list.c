#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include "mydhcp.h"

void insert_ip_addr_top(struct ip_addr *h, struct ip_addr *p)
{
    p->bp = h->bp;
    p->fp = h;
    h->bp->fp = p;
    h->bp = p;
}

void insert_ip_addr_tail(struct ip_addr *h, struct ip_addr *p)
{
    h = h->bp;
    p->bp = h;
    p->fp = h->fp;
    h->fp->bp = p;
    h->fp = p;
}

void remove_ip_addr(struct ip_addr *p)
{
    p->bp->fp = p->fp;
    p->fp->bp = p->bp;
    free(p);
}

int search_ip_addr(struct ip_addr *h)
{
    if (h->fp != h)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

void print_ip_addr(struct ip_addr *h)
{
    struct ip_addr *p;
    for (p = h; p->fp != h; p = p->fp)
    {
        printf("%s %s\n", inet_ntoa(p->ip), inet_ntoa(p->netmask));
    }
}

void insert_client_top(struct client *h, struct client *p)
{
    p->bp = h->bp;
    p->fp = h;
    h->bp->fp = p;
    h->bp = p;
}

void remove_client(struct client *p)
{
    p->bp->fp = p->fp;
    p->fp->bp = p->bp;
    free(p);
}
