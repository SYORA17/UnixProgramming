//61709736 Takanori Shirasaka
#include <stdio.h>
#include <stdbool.h>

//input
#define NNODE 6
#define INF 100 //infinity
int cost[NNODE][NNODE] = {
    {0, 2, 5, 1, INF, INF},
    {2, 0, 3, 2, INF, INF},
    {5, 3, 0, 3, 1, 5},
    {1, 2, 3, 0, 1, INF},
    {INF, INF, 1, 1, 0, 2},
    {INF, INF, 5, INF, 2, 0}};

// A-0, B=1, C=2, D=3, E=4, F=5
int NODES[NNODE] = { 0, 1, 2, 3, 4, 5 };
int N[NNODE] = { INF, INF, INF, INF, INF, INF }; // shortest path
bool N_FLAG[NNODE] = {0, 0, 0, 0, 0, 0};
int dist[NNODE]; // minimum cost at current step
int prev[NNODE]; // last node in current shortest path


//initialization
void init(int root)
{
    for (int i = 0; i < NNODE; i++) {
        N[i] = INF;
        N_FLAG[i] = 0;
        if (i == root) {
            N[0] = i;
            N_FLAG[i] = 1;
        }
    }

    for (int i = 0; i < NNODE; i++)
    {
        if (cost[N[0]][i] != INF) {
            dist[i] = cost[N[0]][i];
            prev[i] = NODES[root];
        }
        else
        {
            dist[i] = INF;
            prev[i] = -1;
        }
    }
}

// change number to character(0→A)
char num_2_char(int n) {
    char c = 'A' + n;
    return c;
}

// print function given root node
void print_func(int root) {
    char char_n = num_2_char(root);
    printf("root node: %c\n\t", char_n);
    for(int j = 0; j < NNODE; j++) {
        printf("[%c, %c, %d] ", num_2_char(j), num_2_char(prev[j]), dist[j]);
    }
    printf("\n");
}


// main algorithm
void djikstra(int root) {
    init(root);

    while(N[NNODE - 1] == INF) {
        int min_cost = INF;
        int min_idx = INF;
        int now_cost = INF;
        int tmp_cost = INF;
        int n_idx = 0;

        //find minimum cost node
        for (int i = 0; i < NNODE; i++) {
            if (N_FLAG[i] == 0 && dist[i] < min_cost){
                min_cost = dist[i];
                min_idx = i;
            }
        }

        for (int i = 0; i < NNODE; i++) {
            n_idx += N_FLAG[i];
        }
        N[n_idx] = min_idx;
        N_FLAG[min_idx] = 1;

        // caliculate new cost
        for (int i = 0; i < NNODE; i++) {
            if (N_FLAG[i] == 1){
                continue;
            }
            now_cost = dist[i];
            tmp_cost = dist[min_idx] + cost[min_idx][i];
            if (now_cost > tmp_cost) {
                dist[i] = tmp_cost;
                prev[i] = min_idx;
            }
        }

    }

    print_func(root);
}

// main method
int main() {
    for (int i = 0; i < NNODE; i++) {
        djikstra(i);
    }
}