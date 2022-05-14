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
    int valid; // 0: invalid, 1: valid
    int tag;
    int lruCounter;
} CacheLineT, **CacheT;

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
    int E = cacheInfo->E;
    *cache = (CacheLineT **)malloc(S * sizeof(CacheLineT*));
    for (int i = 0; i < S; i++) {
        (*cache)[i] = (CacheLineT *)malloc(E * sizeof(CacheLineT));
        for (int j = 0; j < E; j++) {
            (*cache)[i][j].valid = 0;
            (*cache)[i][j].tag = 0;
            (*cache)[i][j].lruCounter = 0;
        }
    }
}

void destroyCacheLine(CacheInfoT *cacheInfo, CacheT *cache)
{
    int S = 1 << cacheInfo->s;
    for (int i = 0; i < S; i++) {
        free((*cache)[i]);
    }
    free(*cache);
}

void updateLruCounter(CacheT *cache, int index, int E)
{
    for (int i = 0; i < E; i++) {
        CacheLineT *cl = &(*cache)[index][i];
        cl->lruCounter++;
    }
}

// process load/modify/save
void processMain(int addr, const CacheInfoT *cacheInfo, CacheT *cache, ResultT *result)
{
    int tmp = addr >> cacheInfo->b;
    int tag = tmp >> cacheInfo->s;
    int index = tmp & ((1 << cacheInfo->s) - 1);
    int E = cacheInfo->E;

    // Step1: check if hit
    for (int i = 0; i < E; i++) {
        CacheLineT *cl = &(*cache)[index][i];
        if (cl->valid == 1 && cl->tag == tag) {
            // hit
            result->hits++;
            updateLruCounter(cache, index, E);
            cl->lruCounter = 0;
            if (cacheInfo->isVerbose) {
                printf(" hit");
            }
            return;
        }
    }

    // Step2: not hit, check if miss
    for (int i = 0; i < E; i++) {
        CacheLineT *cl = &(*cache)[index][i];
        if (cl->valid == 0) {
            // miss
            result->misses++;
            cl->valid = 1;
            cl->tag = tag;
            updateLruCounter(cache, index, E);
            cl->lruCounter = 0;
            if (cacheInfo->isVerbose) {
                printf(" miss");
            }
            return;
        }
    }

    // Step3: not hit and miss, evict
    result->evictions++;
    result->misses++;
    int maxCounter = -2;
    int evictIndex = 0;
    for (int i = 0; i < E; i++) {
        CacheLineT *cl = &(*cache)[index][i];
        if (cl->lruCounter > maxCounter) {
            maxCounter = cl->lruCounter;
            evictIndex = i;
        }
    }
    CacheLineT *evictCacheLine = &(*cache)[index][evictIndex];
    evictCacheLine->tag = tag;
    updateLruCounter(cache, index, E);
    evictCacheLine->lruCounter = 0;
    if (cacheInfo->isVerbose) {
        printf(" miss eviction");
    }
    return;
}

int main(int argc, char *argv[])
{
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
    ResultT result = {0};

    while (fscanf(traceFile, " %c %x,%d", &identifier, &addr, &size)>0)
    {
        if (cacheInfo.isVerbose) {
            printf("%c %x,%d", identifier, addr, size);
        }
        
        switch(identifier) {
            case 'L':
                processMain(addr, &cacheInfo, &cache, &result);
                break;
            case 'M':
                processMain(addr, &cacheInfo, &cache, &result);
            case 'S':
                processMain(addr, &cacheInfo, &cache, &result);
                break;
            default:
                break;
        }

        if (cacheInfo.isVerbose) {
            printf("\n");
        }
    }
    fclose(traceFile);
    destroyCacheLine(&cacheInfo, &cache);

    printSummary(result.hits, result.misses, result.evictions);
    return 0;
}
