#include "cachelab.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_STRING_LEN 128

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

typedef struct CacheInfo {
    int s;
    int E;
    int b;
    bool isVerbose;
    char traceFilePath[MAX_STRING_LEN];
} CacheInfoT;

typedef struct CacheLine {
    bool valid;
    int tag;
    int lruCounter;
} CacheLineT;

typedef struct Cache {
    CacheLineT **cl;
} CacheT;

typedef struct Result {
    int hits;
    int misses;
    int evictions;
} ResultT;

void processInput(CacheInfoT *cacheInfo, int argc, char *argv[])
{
    char optStr[] = "hvs:E:b:t:";
    int o;

    while ((o = getopt(argc, argv, optStr)) != -1)
    {
        switch (o)
        {
        case 's':
            cacheInfo->s = optarg[0] - '0';
            break;
        case 'E':
            cacheInfo->E = optarg[0] - '0';
            break;
        case 'b':
            cacheInfo->b = optarg[0] - '0';
            break;
        case 't':
            strcpy(cacheInfo->traceFilePath, optarg);
            break;
        case 'h':
            printf("%s\n", HELP);
            exit(0);
        case 'v':
            cacheInfo->isVerbose = true;
            break;
        default:
            break;
        }
    }
}

bool checkInputValid(CacheInfoT *cacheInfo)
{
    if (cacheInfo->b == 0 || cacheInfo->E == 0 || cacheInfo->s == 0 || strlen(cacheInfo->traceFilePath) == 0) {
        printf("%s\n", WARNING);
        printf("%s\n", HELP);
        return false;
    } else {
        return true;
    }
}

void createCacheLine(CacheInfoT *cacheInfo, CacheT *cache)
{
    int S = 1 << cacheInfo->s;
    cache->cl = malloc(S * cacheInfo->E * sizeof(CacheLineT));
    if (cache->cl == NULL) {
        printf("malloc failed when creating cacheline.\n");
        exit(0);
    }
}

// process load/modify/save
void processMain(int addr, int size, const CacheInfoT *cacheInfo, CacheT *cache, ResultT *result)
{
    
}

int main(int argc, char *argv[])
{
    printf("===========Main===========\n");
    CacheInfoT cacheInfo = {0};
    processInput(&cacheInfo, argc, argv);
    if (!checkInputValid(&cacheInfo)) {
        return 0;
    }

    CacheT cache = {0};
    createCacheLine(&cacheInfo, &cache);

    FILE *traceFile = fopen(cacheInfo.traceFilePath, "r");
    assert(traceFile);

    char identifier;
    unsigned addr;
    int size;
    ResultT result = {
        .hits = 0,
        .misses = 0,
        .evictions = 0
    };

    while (fscanf(traceFile, " %c %x,%d", &identifier, &addr, &size)>0)
    {
        switch(identifier) {
            case 'L':
                processMain(addr, size, &cacheInfo, &cache, &result);
                break;
            case 'M':
                processMain(addr, size, &cacheInfo, &cache, &result);
            case 'S':
                processMain(addr, size, &cacheInfo, &cache, &result);
                break;
            default:
                break;
        }
    }
    fclose(traceFile);
    free(cache.cl);

    printSummary(result.hits, result.misses, result.evictions);
    return 0;
}

