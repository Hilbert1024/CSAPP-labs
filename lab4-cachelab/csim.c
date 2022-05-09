#include "cachelab.h"
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Cache {
    int s;
    int E;
    int b;
} CacheT;

#define WARNING "./csim: Missing required command line argument"

#define HELP "Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n\
Options:\n\
  -h         Print this help message.\n\
  -v         Optional verbose flag.\n\
  -s <num>   Number of set index bits.\n\
  -E <num>   Number of lines per set.\n\
  -b <num>   Number of block offset bits.\n\
  -t <file>  Trace file.\n\
\
Examples:\n\
  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n\
  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace"

// #define NORMAL_PARAM_COUNT 8
// #define EXTRA_PARAM_COUNT 9

void ProcessInput(CacheT *cache, bool *isVerbose, char *traceFilePath, int argc, char *argv[])
{
    char optStr[] = "hvs:E:b:t:";
    int o;

    while ((o = getopt(argc, argv, optStr)) != -1)
    {
        switch (o)
        {
        case 's':
            cache->s = optarg[0] - '0';
            break;
        case 'E':
            cache->E = optarg[0] - '0';
            break;
        case 'b':
            cache->b = optarg[0] - '0';
            break;
        case 't':
            strcpy(traceFilePath, optarg);
            break;
        case 'h':
            printf("%s\n", HELP);
            exit(0);
        case 'v':
            *isVerbose = true;
            break;
        default:
            break;
        }
    }
}

bool CheckInputValid(CacheT *cache, char *traceFilePath)
{
    if (cache->b == 0 || cache->E == 0 || cache->s == 0 || strlen(traceFilePath) == 0) {
        printf("%s\n", WARNING);
        printf("%s\n", HELP);
        return false;
    } else {
        return true;
    }
}

int main(int argc, char *argv[])
{
    printf("===========Main===========\n");
    CacheT cache = {0};
    bool isVerbose = false;
    char traceFilePath[128];
    ProcessInput(&cache, &isVerbose, traceFilePath, argc, argv);
    printf("traceFilePath = %s, isVerbose = %d, Cache(s,E,b) = (%d,%d,%d)\n",
        traceFilePath, isVerbose, cache.s, cache.E, cache.b);
    if (!CheckInputValid(&cache, traceFilePath)) {
        return 0;
    }
    printSummary(0, 0, 0);
    return 0;
}

