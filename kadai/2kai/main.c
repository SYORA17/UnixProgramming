#include <stdio.h>
#include "buf.h"

// struct buf_header hash_head[NHASH];
// struct buf_header free_head;

void init()
{
    add_hash(&hash_head[0], 0, STAT_VALID, "hi");
}

int main()
{
    // 61709736 Takanori Shirasaka
    init();
}
