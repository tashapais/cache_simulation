#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int log2n(unsigned int n)
{
    return (n > 1) ? 1 + log2n(n / 2) : 0;
}

int has_spot(long int **c, int set, int E) {
    int s = (set * E);
    int i;
    for (i = s; i < s + E; i++) {
        if (c[i][0] == 0) {
            return i;
        }
    }
    return -1;
}

void incr_age(long int **c, int set, int E) {
    int s = (set * E);
    int i;
    for (i = s; i < s + E; i++) {
        if (c[i][0] == 1) {
            c[i][2] += 1;
        }
    }
}

int get_index(long int **c, int set, int E) {
    int s = (set * E);
    int i, max = 0, index = -1;
    for (i = s; i < s + E; i++) {
        if (c[i][0] == 1 && c[i][2] > max) {
            max = c[i][2];
            index = i;
        }
    }
    return index;
}

const char* __asan_default_options() { return "detect_leaks=0"; }

void store(long int **c, int set, int E, long int tag, int method) {
    int empty = has_spot(c, set, E);
    incr_age(c, set, E);
    if (empty != -1) {
        c[empty][0] = 1;
        c[empty][1] = tag;
    }

    else if (method) {
        int i = get_index(c, set, E);
        c[i][0] = 1;
        c[i][1] = tag;
        c[i][2] = 0;
    }
}

int search(long int **c, int set, int E, long int tag) {
    int s = (set * E);
    int i;
    for (i = s; i < s + E; i++) {
        if (c[i][0] == 1 && c[i][1] == tag) {
            return 1;
        }
    }
    return 0;
}

void print_cache(long int **c, int S) {
    int i;
    for (i = 0; i < S; i++) {
        printf("[%lx, %lx, %lx]\n", c[i][0], c[i][1], c[i][2]);
    }
}



int main(int argc, char **argv) {


    int b = 0, s = 0;
    int pre = 0, re = 0;
    int reads = 0, writes = 0;
    int hits = 0, misses = 0;
    int C = atoi(argv[1]); 
    int B = atoi(argv[4]); 
    int E = 0;
    int S = 0;
    long int addr = 0;
    char op;
    long int **cache;

    if (strcmp(argv[2], "direct") == 0) {
        E = 1;
        S = C / (B * E);
        b = log2n(B);
        s = log2n(S);
    }
    else if (strstr(argv[2], ":") != NULL) {
        sscanf(argv[2], "assoc:%d", &E);
        S = C / (B * E);
        b = log2n(B);
        s = log2n(S);
    }
    else if (strcmp(argv[2], "assoc") == 0) {
        S = 1;
        E = C / (B * S);
        b = log2n(B);
        s = 0;
    }

   


    cache = malloc(S * E * sizeof(long int *));
    int i;
    for (i = 0; i < S * E; i++) {
        cache[i] = malloc(3 * sizeof(long int));
        cache[i][0] = 0;
        cache[i][1] = 0;
        cache[i][2] = 0;
    }


    re = strcmp("fifo", argv[3]) == 0; 

    FILE *file = fopen(argv[5], "r"); 
    if (!file) {
        printf("Invalid file\n");
        return 0;
    }


    while (1 < 2) {

        fscanf(file, "%*x");
        fscanf(file, " %c %c", &op, &op);
        fscanf(file, "%lx\n", &addr);
        if (op == 'e') {
            break;
        }

        long int id = addr >> b;
        long int set = id & ((1 << s) - 1);
        long int tag = id >> s;

        int res = search(cache, set, E, tag);
        if (res) {
            hits++;
        }
        else {
            misses++;
            reads++;
            store(cache, set, E, tag, re);
            if (pre) {
                long preId = (addr + B) >> b;
                set = preId & ((1 << s) - 1);
                tag = preId >> s;
                res = search(cache, set, E, tag);
                if (!res) {
                    reads++;
                    store(cache, set, E, tag, re);
                }
            }
        }
        if (op == 'W') {
            writes++;
        }
    }


    printf("Prefetch 0\n");
    printf("Memory reads: %d\n", reads);
    printf("Memory writes: %d\n", writes);
    printf("Cache hits: %d\n", hits);
    printf("Cache misses: %d\n", misses);
   
   

    E = 0;
    S = 0;
    b = 0;
    s = 0;
    re = 0;
    reads = 0;
    writes = 0;
    hits = 0;
    misses = 0;
    addr = 0;
    char op2;
    long int **cache2;

    if (strcmp(argv[2], "direct") == 0) {
        E = 1;
        S = C / (B * E);
        b = log2n(B);
        s = log2n(S);
    }
    else if (strstr(argv[2], ":") != NULL) {
        sscanf(argv[2], "assoc:%d", &E);
        S = C / (B * E);
        b = log2n(B);
        s = log2n(S);
    }
    else if (strcmp(argv[2], "assoc") == 0) {
        S = 1;
        E = C / (B * S);
        b = log2n(B);
        s = 0;
    }

   

    pre = 1;
    cache2 = malloc(S * E * sizeof(long int *));
   
    for (int i = 0; i < S * E; i++) {
        cache2[i] = malloc(3 * sizeof(long int));
        cache2[i][0] = 0;
        cache2[i][1] = 0;
        cache2[i][2] = 0;
    }

    re = strcmp("fifo", argv[3]) == 0; 

    FILE *file2 = fopen(argv[5], "r");
    if (!file2) {
        printf("Invalid file2\n");
        return 0;
    }


    while (1 < 2) {
        fscanf(file2, "%*x");
        fscanf(file2, " %c %c", &op2, &op2);
        fscanf(file2, "%lx\n", &addr);
        if (op2 == 'e') {
            break;
        }

        long int id = addr >> b;
        long int set = id & ((1 << s) - 1);
        long int tag = id >> s;

        int res = search(cache2, set, E, tag);
        if (res) {
            hits++;
        }
        else {
            misses++;
            reads++;
            store(cache2, set, E, tag, re);
            if (pre) {
                long preId = (addr + B) >> b;
                set = preId & ((1 << s) - 1);
                tag = preId >> s;
                res = search(cache2, set, E, tag);
                if (!res) {
                    reads++;
                    store(cache2, set, E, tag, re);
                }
            }
        }
        if (op2 == 'W') {
            writes++;
        }
    }




    printf("Prefetch 1\n");
    printf("Memory reads: %d\n", reads);
    printf("Memory writes: %d\n", writes);
    printf("Cache hits: %d\n", hits);
    printf("Cache misses: %d\n", misses);


   

    exit(0);
}
