#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <unistd.h>
#include "mydhcp.h"

void insert_ip_addr_top(struct ip_addr *h, struct ip_addr *p)
{
    p->bp = h;
    p->fp = h->fp;
    h->fp->bp = p;
    h->fp = p;
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
    // free(p);
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

struct client *search_client(struct client *h, struct sockaddr_in *skt)
{
    struct client *s;
    for (s = h; s->fp != h; s = s->fp)
    {
        if (s->id.s_addr == skt->sin_addr.s_addr)
        {
            return s;
        }
    }
    return NULL;
}

void dec_ttl()
{
    struct client *c;
    for (c = client_list->fp; c->fp != client_list; c = c->fp) {
        if (c->status == IN_USE) {
            c->ttlcounter--;
        }
    }
}

void check_ttl(int s)
{
    struct client *c;
    int count;
    char sbuf[STR_MAX] = "0";
    for (c = client_list->fp; c->fp != client_list; c = c->fp) {
        if (c->ttl == 0 && c->status == IN_USE) {
            printf("%s, timeout\n", inet_ntoa(c->addr));
            if ((count = sendto(s, sbuf, sizeof(sbuf), 0,
                        (struct sockaddr *)&c->skt, sizeof(c->skt))) < 0)
            {
                perror("sendto");
                exit(1);
            }
            struct ip_addr *p;
            p = malloc(sizeof(struct ip_addr));
            p->ip = c->addr;
            p->netmask = c->netmask;
            insert_ip_addr_tail(ip_addr_h, p);
            remove_client(c);
            close(c);
        }
    }
}
