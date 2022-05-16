# Lab4 - cachelab

## 基础知识

### 局部性

- 一个编写良好的程序常常具有良好的局部性。
- 时间局部性：被引用过一次的内存位置在不久被多次引用。
- 空间局部性：一个内存位置被引用了一次，在不久其附近的内存位置被引用。
- 顺序访问一个向量每个元素的函数，具有步长为1的引用模式，称为顺序引用模式。步长为 k 的访问就称为步长为 k 的引用模式。

### 缓存

- 层次结构中的每一层都缓存来自较低一层的数据对象。
- 第 k 层的缓存包含 k+1 层块的一个子集的副本。
- 数据总是以块大小为传送单元在第 k 层和第 k+1 层之间来回复制的。

> 缓存命中：
>
> 当程序需要第 k+1 层的某个数据对象 d 时，它首先在当前存储在第 k 层的一个块中查找 d。如果 d 刚好缓存在第 k 层中，那么就是缓存命中。

- 当发生缓存不命中时，第 k 层的缓存从第 k+1 层缓存中取出包含 d 的那个块，如果第 k 层的缓存已经满了，可能就会覆盖现存的一个块。

| 缓存不命中   |                                                              |
| ------------ | ------------------------------------------------------------ |
| 强制性不命中 | 当第 k 层缓存为空，则访问第 k+1 层的数据对象均不命中         |
| 冲突不命中   | 根据某种放置策略，将 k+1 层的多个数据对象映射至 k 层的某个块中，当循环访问这些区块时，缓存不停替换且不命中 |
| 容量不命中   | 工作集太小无法放在缓存中                                     |

- 大多数时候，缓存都是自动运行的，不需要程序采取特殊的或显示的行动。

### 高速缓存

**读**

<img src=".\figures\lab4-cache.png" alt="lab4-cache" style="zoom:50%;" />

- 高速缓存由 (S, E, B, m) 描述。

  | 参数          | 描述                 |
  | ------------- | -------------------- |
  | $S = 2^s$     | 组数                 |
  | $E$           | 每个组的行数         |
  | $B=2^b$       | 块大小（字节）       |
  | $m=\log_2(M)$ | （主存）物理地址位数 |

  

- 高速缓存地址将m个位分为t(标记)，s(组索引)，b(块偏移)。$m = t + s + b$.

- 当CPU发出一个地址A时，可通过s找到数据所在的组，通过t找到数据所在的行，通过b找到数据在高速缓存块中的偏移。由

$$
S = 2^s,\ B = 2^b,\ t = m - (s+b)
$$

可以知道该查找是充分的。

+ 高速缓存的大小为
  $$
  C = S \times E \times B.
  $$

- E = 1 时被称为直接映射高速缓存。

- 高速缓存确定一个请求是否命中，然后抽取出被请求的字的过程，分为三步：1. 组选择 2. 行匹配 3. 字抽取。

  Step1：直接映射高速缓存中的组选择。从地址中读取组索引，并定位到所在的组。

  Step2：直接映射高速缓存中的行匹配。上一步选择了某个组i，现在确定是否有字w的一个副本存储在组i包含的一个高速缓存行当中。如果查找到某行的标记位与地址中的标记位匹配，则缓存命中；否则不命中。

  Step3：直接映射高速缓存中的字选择。命中后我们就知道字w就在这个块中的某个地方。通过偏移来定位字的位置。

  Step4：直接映射高速缓存中不命中时的行替换。如果缓存不命中，那么它需要从存储器层次结构中的下一层取出被请求的块，然后将新的块存储在组索引位指示的组中的一个高速缓存行中。如果组中都有高速缓存行了，则要驱逐一块。

+ 组相联高速缓存满足
  $$
  1<E<C/B.
  $$

+ 组相联高速缓存中每组有多行。相联存储器是一个键值对，可以把每组看成一个小的相联存储器，键是标记和有效位，值就是块的内容。

+ 组相联高速缓存中不命中时，需要替换存在的行。若有空行则替换之；否则将使用一定的策略来替换行。策略有LFU、LRU等。

+ 全相联高速缓存满足
  $$
  E = C/B,\ \text{i.e. } S = 1.
  $$

+ 全相联高速缓存只有一个组，故其地址不包含组索引，由t位标记和b位块偏移构成。

**写**

+ 写命中：

  假设要写一个已经缓存了的字w，在高速缓存更新了他的w的副本后，如何更新w在层次结构中紧接着低一层中的副本？

| 方法                  | 描述                                                         | 缺点                   |
| --------------------- | ------------------------------------------------------------ | ---------------------- |
| 直写（write-through） | 立即将高速缓存块写回到紧接着的第一层中                       | 每次写都会引起总线流量 |
| 写回（write-back）    | 尽可能的推迟更新，只有当替换算法要驱逐这个更新过的块时，才把它写到紧接着的低一层。 | 增加了复杂性           |

+ 写不命中：

| 方法                           | 描述                                                       | 缺点                                           |
| ------------------------------ | ---------------------------------------------------------- | ---------------------------------------------- |
| 写分配（write-allocate）       | 加载相应的低一层中的块到高速缓存中，然后更新这个高速缓存块 | 每次不命中都会导致一个块从低一层传送到高速缓存 |
| 非写分配（not-write-allocate） | 避开高速缓存，直接把这个字写到第一层中                     |                                                |

+ 直写高速缓存通常是非写分配的。写回高速缓存通常是写分配的。建议在心中建立一个使用写回和写分配的高速缓存的模型。

**参数性能影响**

| 参数         | 影响                                                         |
| ------------ | ------------------------------------------------------------ |
| 高速缓存大小 | 较大的高速缓存可能会提高命中率，但会增加命中时间             |
| 块大小       | 较大的块能利用程序中可能存在的空间局部性，提高命中率；但块越大就意味着高速缓存行数越少，这会损害时间局部性比空间局部性更好的程序的命中率。 |
| 相联度       | 参数E的影响。较大的E可降低高速缓存由于冲突不命中出现抖动的可能性，但成本较高 |
| 写策略       | 高速缓存越往下层，越可能使用写回而不是直写                   |

## 实验简介

本实验将帮助你了解高速缓冲存储器对C语言程序性能的影响。本实验由以下两部分组成：

**Part A**：你将编写一个小的C语言程序（大约200-300行），模拟高速缓冲存储器的行为。

**Part B**：你将优化一个小的矩阵转置函数，目标是最小化高速缓存的失误次数。

### Part A

在 Part A 中，你将在csim.c中编写一个缓存模拟器，它以valgrind内存跟踪为输入，模拟缓存内存的 hits/misses 行为，并输出 hits, misses, 和 evictions 的总次数。 我们为你提供了一个名为csim-ref的参考缓存模拟器的二进制执行文件，它模拟了一个具有任意大小和关联性的缓存在valgrind跟踪文件中的行为。在选择驱逐哪条缓存线时，它使用LRU（最近使用最少的）替换策略。引用模拟器需要以下命令行参数。

```shell
Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>
```

> + -h: Optional help flag that prints usage info 
>
> + -v: Optional verbose flag that displays trace info 
> + -s \<s\>: Number of set index bits (S = 2^s is the number of sets) 
> + -E \<E\>: Associativity (number of lines per set) 
> + -b \<b\>: Number of block bits (B = 2^b is the block size)
> + -t \<tracefile\>: Name of the valgrind trace to replay

在 tracefile 中，每一条对内存访问的记录格式是 `[空格]操作符 地址,大小`，以 `I` 开头的是载入指令的记录，不算在内存访问中。

- M 表示数据修改，需要一次载入 + 一次存储，也就是相当于两次访问
- S 表示数据存储
- L 表示数据载入
- 地址指的是一个 64 位的 16 进制内存地址
- 大小表示该操作内存访问的字节数

你在 Part A 的工作是填写csim.c文件，使其接受相同的命令行参数，并产生与参考模拟器相同的输出。注意，这个文件几乎完全是空的。你需要从头开始写。模拟器 csim-ref 的输出如下：

```c
root# ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace
hits:4 misses:5 evictions:3
root# ./csim-ref -v -s 4 -E 1 -b 4 -t traces/yi.trace
L 10,1 miss
M 20,1 miss hit
L 22,1 hit
S 18,1 hit
L 110,1 miss eviction
L 210,1 miss eviction
M 12,1 miss eviction hit
hits:4 misses:5 evictions:3
```

**规则**

+ 你的模拟器必须对任意的 s、E 和 b 正常工作。这意味着你需要使用 malloc 函数为模拟器的数据结构分配存储空间。

+ 对于这个实验室，我们只对数据缓存的性能感兴趣，所以你的模拟器应该忽略所有指令缓存的访问（以 "I "开头的行）。记得 valgrind总是把 "I "放在第一列（前面没有空格），而把 "M"、"L"和 "S"放在第二列（前面有空格）。这可能有助于你解析追踪的内容。
+ 你必须在主函数的末尾调用函数 printSummary，并给出 hits, misses, 和 evictions 的总数：printSummary(hit_count, miss_count, eviction_count)。
+ 在这个实验中，你应该假设内存访问是正确对齐的，这样一个内存访问就不会跨越块的边界。通过这个假设，你可以忽略 valgrind 追踪中的请求大小。

### Part B

在B部分中，你将在 trans.c 中编写一个转置函数，尽可能少地造成缓存缺失。为了帮助你入门，我们在 trans.c 中给出了一个转置函数的例子，该函数计算 N×M 矩阵A的转置，并将结果存储在M×N矩阵B中： 

```c
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
```

这个转置函数例子很正确，但它的效率很低，因为访问模式会导致相对多的缓存丢失。你在B部分的工作是写一个类似的函数，叫做``transpose_submit``，在不同大小的矩阵中最小化缓存缺失次数：

```c
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]);
```

不要改变``transpose_submit``函数的描述字符串（"transpose submission"）。

**性能测试参数**

+ (s,E,b) = (5,1,5)
+ 三组测试
  + (M,N) = (32,32)
  + (M,N) = (64,64)
  + (M,N) = (61,67)

**评分标准**

在上述三组测试中，统计出的 miss 数 m 应满足

+ (M,N) = (32,32), m < 300
+ (M,N) = (64,64), m < 1300
+ (M,N) = (61,67), m < 2000

即可获得满分。

**规则**

+ 每个转置函数最多允许定义12个int类型的局部变量。
+ 你不允许通过使用任何long类型的变量或通过使用任何 **bit tricks** 将一个以上的值存储到一个单一的变量中来绕过前面的规则。
+ 你的转置函数不可以使用递归。
+ 如果你选择使用辅助函数，在你的辅助函数和你的顶层转置函数之间，你的堆栈上一次不能有超过12个局部变量。例如，如果你的转置函数声明了8个变量，然后你调用了一个使用4个变量的函数，这个函数又调用了另一个使用2个变量的函数，那么你的堆栈上就会有14个变量，你就违反了这个规则。
+ 你的转置函数不能修改数组A，但是，你可以对数组B的内容做任何你想做的事。
+ 你不允许在你的代码中定义任何数组或使用任何malloc的变体。

## Part A

Part A任务基本分为以下三个步骤进行：

1. 处理命令行输入：获取从bash终端输入命令的参数。
2. 文件读取并解析操作：获取tarce文件中对内存的操作参数。
3. 使用LRU替换算法模拟高速缓存：使用自定的数据结构，采用LRU策略模拟高速缓存。

### 命令行输入

使用``getopt``函数可以解析命令行的输入参数。其中``argc``为输入参数个数，``argv``存储了输入参数的字符串。

由于本人未设置全局变量，故解析的结果存储在数据结构``CacheInfo``中。

```c
typedef struct CacheInfo {
    int s;
    int E;
    int b;
    bool isVerbose;
    char traceFilePath[MAX_STRING_LEN];
} CacheInfoT;

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
```

### 文件读取

首先打开trace文件。我们可以采用文件中固有的格式``" %c %x,%d"``来读取文件每行的三个参数。

```c
FILE *traceFile = fopen(cacheInfo.traceFilePath, "r");
assert(traceFile);

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
```

### LRU策略

我们使用 LRUCounter 方法来实现高速缓存的LRU替换策略。

> PS:为追求时间复杂度O(1)的 put(hit/miss) get(evict)操作，可以采用“双向链表+哈希表”来实现LRU.

LRUCounter方法为每一个CacheLine添加一个计数器。当操作 hit/miss 某一 CacheSet 的 CacheLine 时：

1. 将该 CacheSet 的所有CacheLine计数器+1。
2. 将该 CacheLine 的计数器置0。

上述操作可以保证，若CacheLine计数器值越大，则它被访问的时间越早。

当操作 evict 时，就可以淘汰掉该组计数器值最大的 CacheLine。

上述 LRUCounter 方法对应的代码为：

```c
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
```

### 结果

运行脚本 ``test-csim``，可获取每个测试的得分。

```shell
# ./test-csim
                        Your simulator     Reference simulator
Points (s,E,b)    Hits  Misses  Evicts    Hits  Misses  Evicts
     3 (1,1,1)       9       8       6       9       8       6  traces/yi2.trace
     3 (4,2,4)       4       5       2       4       5       2  traces/yi.trace
     3 (2,1,4)       2       3       1       2       3       1  traces/dave.trace
     3 (2,1,3)     167      71      67     167      71      67  traces/trans.trace
     3 (2,2,3)     201      37      29     201      37      29  traces/trans.trace
     3 (2,4,3)     212      26      10     212      26      10  traces/trans.trace
     3 (5,1,5)     231       7       0     231       7       0  traces/trans.trace
     6 (5,1,5)  265189   21775   21743  265189   21775   21743  traces/long.trace
    27

TEST_CSIM_RESULTS=27
```

## Part B

性能测试参数 (s,E,b) = (5,1,5)，为直接命中缓存，每个 CacheLine 缓存32字节的数据。而矩阵中每个位置存储的是4字节的数据，那么每个 CacheLine 存储矩阵的8个位置。

> 如果计算机性能过于羸弱(例如我的)，可以将超时退出机制注释掉，或将超时时间变大：
>
> ```c
>  // if (signal(SIGALRM, sigalrm_handler) == SIG_ERR) {
>  //     fprintf(stderr, "Unable to install SIGALRM handler\n");
>  //     exit(1);
>  // }
> 
>  /* Time out and give up after a while */
>  alarm(120); // 或修改该数字
> ```

### 分块技术

在计算矩阵乘法时，使用分块技术可减少cache miss：

![lab4-block1](.\figures\lab4-block.png)



矩阵转置的情况类似。我们先来看两个实验。

### 实验：(M,N) = (8,8)

这里我们可以先来做一个实验。令 (M,N) = (8,8)，运行 ``Simple row-wise scan transpose`` 可得到如下 trace 文件(已经过Part A csim.c 处理)：

```shell
// op addr size index result
S 110001100000010001100,1 [4]  miss
L 110001100000010100000,8 [5]  miss
L 110001100000010000100,4 [4]  hit
L 110001100000010000000,4 [4]  hit
L 100001100000010000000,4 [4]  miss eviction // 从该行开始，执行矩阵 A,B 的转置操作
S 101001100000010000000,4 [4]  miss eviction // 不难看出是 L,S 交替执行
L 100001100000010000100,4 [4]  miss eviction // 10000 开头为对A矩阵操作(L)，10100开头为对B矩阵操作(S)
S 101001100000010100000,4 [5]  miss eviction // L操作中，每步的间隔为4字节，说明A按行扫描，且每8次index加1
L 100001100000010001000,4 [4]  hit           // L操作对应index规律为 44444444 -> 555... -> 11 11 11 ...
S 101001100000011000000,4 [6]  miss          // S操作中，每步的间隔为32字节，说明B按列扫描，且每次index加1
L 100001100000010001100,4 [4]  hit           // S操作对应index规律为 4 5 6 7 8 9 10 11 -> 4 5 6 7 8 9 10 11 -> ...
S 101001100000011100000,4 [7]  miss
... // 此处省略
L 100001100000101111100,4 [11]  hit
S 101001100000101111100,4 [11]  miss eviction
S 110001100000010001101,1 [4]  miss eviction
hits:93 misses:40 evictions:32
```

那么，A的 miss 矩阵为(1为访问至此地址发生 cache miss，其余为hit)

| A    | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| 0    | 1    | 1    |      |      |      |      |      |      |
| 1    | 1    |      | 1    |      |      |      |      |      |
| 2    | 1    |      |      | 1    |      |      |      |      |
| 3    | 1    |      |      |      | 1    |      |      |      |
| 4    | 1    |      |      |      |      | 1    |      |      |
| 5    | 1    |      |      |      |      |      | 1    |      |
| 6    | 1    |      |      |      |      |      |      | 1    |
| 7    | 1    |      |      |      |      |      |      |      |

B的 miss 矩阵为

| B    | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| 0    | 1    | 1    |      |      |      |      |      |      |
| 1    | 1    | 1    | 1    |      |      |      |      |      |
| 2    | 1    |      | 1    | 1    |      |      |      |      |
| 3    | 1    |      |      | 1    | 1    |      |      |      |
| 4    | 1    |      |      |      | 1    | 1    |      |      |
| 5    | 1    |      |      |      |      | 1    | 1    |      |
| 6    | 1    |      |      |      |      |      | 1    | 1    |
| 7    | 1    |      |      |      |      |      |      | 1    |

其中A miss 15次，B miss 22次，共计37次。再加上3次额外miss(对应第3,4及最后一行)，共计40次。

由上述实验可知，若想减少cache miss，**一定要减少访问数据的冲突**。

### 实验：(M,N) = (16,16)

高速缓存的容量为
$$
C = S * E * B = 2^5*1*2^5 \text{ Bytes}=2^{10}\text{ Bytes}.
$$
而 16*16 矩阵的空间恰好为 C。与(8,8)类似，这里我们借用网络上的一张图来表明cache miss的块：

<img src=".\figures\lab4-matrix.png" alt="lab4-matrix" style="zoom: 33%;" />



### (M,N) = (32,32)

现在我们针对 (M,N) = (32,32)进行优化。矩阵的每一行可由4个 CacheLine 载入。

**分块技术**

当采用分块技术，每次计算 8*8 的块时：

```c
// block
char transpose_block_32_32_desc[] = "Transpose block";
void transpose_block_32_32(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;
    int row = 8;
    int col = 8;
    for (i = 0; i < N; i += row) {
        for (j = 0; j < M; j += col) {
            for (int i1 = 0; i1 < row; i1++) {
                for (int j1 = 0; j1 < col; j1++) {
                    tmp = A[i + i1][j + j1];
                    B[j + j1][i + i1] = tmp;
                }
            }
        }
    }
}

// result
Function 0 (1 total)
Step 1: Validating and generating memory traces
Step 2: Evaluating performance (s=5, E=1, b=5)
func 0 (Transpose submission): hits:1710, misses:343, evictions:311
```

不难验证，将 32\*32 矩阵分为 8\*8 的小块，其中：

+ 对角线的块是上述 (8,8) 模型，cache miss 为37次。
+ 其它块不存在两个矩阵的数据冲突，而仅有第一次存储至缓存的cache miss，每个块8次，共计16次。
+ 函数相关 cache miss 3次。

共计
$$
4 * 37 + 12 * (2 * 8) + 3 = 343.
$$
与实验结果一致，但仍未达标。

下面考虑在分块计算过程中，优化对角线元素的cache miss，我们的目标是低于26次：

当每次访问A的其中一行时，我们将该CacheLine中的所有元素取出，随后再放回对应的B处。这样减少了冲突块“抖动(thrashing)”的次数。

上述策略可得A的 miss 矩阵为

| A    | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| 0    | 1    |      |      |      |      |      |      |      |
| 1    | 1    |      |      |      |      |      |      |      |
| 2    | 1    |      |      |      |      |      |      |      |
| 3    | 1    |      |      |      |      |      |      |      |
| 4    | 1    |      |      |      |      |      |      |      |
| 5    | 1    |      |      |      |      |      |      |      |
| 6    | 1    |      |      |      |      |      |      |      |
| 7    | 1    |      |      |      |      |      |      |      |

B的 miss 矩阵为

| B    | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| 0    | 1    |      |      |      |      |      |      |      |
| 1    | 1    | 1    |      |      |      |      |      |      |
| 2    | 1    |      | 1    |      |      |      |      |      |
| 3    | 1    |      |      | 1    |      |      |      |      |
| 4    | 1    |      |      |      | 1    |      |      |      |
| 5    | 1    |      |      |      |      | 1    |      |      |
| 6    | 1    |      |      |      |      |      | 1    |      |
| 7    | 1    |      |      |      |      |      |      | 1    |

cache miss共计23次。

那么我们可以计算出总的 cache miss 为
$$
4 * 23 + 12 * (2 * 8) + 3 = 287.
$$
进行实验，与预期一致！

```c
// block optimize
char transpose_block_opt_32_32_desc[] = "Transpose block optimize";
void transpose_block_opt_32_32(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, c0, c1, c2, c3, c4, c5, c6, c7;
    int row = 8;
    int col = 8;
    for (i = 0; i < N; i += row) {
        for (j = 0; j < M; j += col) {
            for (int i1 = 0; i1 < row; i1++) {
                c0 = A[i + i1][j + 0];
                c1 = A[i + i1][j + 1];
                c2 = A[i + i1][j + 2];
                c3 = A[i + i1][j + 3];
                c4 = A[i + i1][j + 4];
                c5 = A[i + i1][j + 5];
                c6 = A[i + i1][j + 6];
                c7 = A[i + i1][j + 7];
                B[j + 0][i + i1] = c0;
                B[j + 1][i + i1] = c1;
                B[j + 2][i + i1] = c2;
                B[j + 3][i + i1] = c3;
                B[j + 4][i + i1] = c4;
                B[j + 5][i + i1] = c5;
                B[j + 6][i + i1] = c6;
                B[j + 7][i + i1] = c7;
            }
        }
    }
}

// result
Function 0 (1 total)
Step 1: Validating and generating memory traces
Step 2: Evaluating performance (s=5, E=1, b=5)
func 0 (Transpose submission): hits:1766, misses:287, evictions:255
```

> (M,N) = (32,32), result = 287 (未达到理论最优情况)

### (M,N) = (64,64)

若我们仍考虑上述分块技术，块大小为 8\*8，理论上cache miss为
$$
8 * 23 + 56 * (2*8) + 3 = 1083
$$


```c
Function 0 (1 total)
Step 1: Validating and generating memory traces
Step 2: Evaluating performance (s=5, E=1, b=5)
func 0 (Transpose submission): hits:3586, misses:4611, evictions:4579
```

实验结果却与预期大相径庭！说明上述的公式失效了。这是为什么？

我们考虑同一矩阵上任意两个元素的地址 $p_1,p_2$, 并假设 $p_1<p_2$。那么，由 $s = 5, b=5$ 可知，

若
$$
(p_2-p_1)\ \&\ \mathtt{0xFFFFFFFFFF}\ = 0,
$$
即
$$
p_2-p_1 \equiv 0 \mod{2^{10}},
$$
则这两个元素命中同一CacheLine的同一block。

当(M,N)=(32,32)时，对矩阵上的任意元素，每向下8行即为对应冲突的位置。

然而，当(M,N)=(32,32)时，对矩阵上的任意元素，**每向下4行即为对应冲突的位置。如果仍按(8*8)分块，那么块内即有冲突。**

> 若想验证上述公式，只需令s = 6，那么仍为间隔8行冲突。可以得到结果为
>
> Function 0 (1 total)
> Step 1: Validating and generating memory traces
> Step 2: Evaluating performance (s=6, E=1, b=5)
> func 0 (Transpose block optimize 32,32): hits:7114, misses:1083, evictions:1019
>
> 与预期相符。

自然地，我们想到按 4*4 分块。

```c
Function 0 (1 total)
Step 1: Validating and generating memory traces
Step 2: Evaluating performance (s=5, E=1, b=5)
func 0 (Transpose submission): hits:6498, misses:1699, evictions:1667
```

这样虽然没有满分，但已极大减少cache miss。

> 更极限的做法是用 (8\*8) 分块，随后在分为4个 (4\*4) 的小块，这样确保每个小块内不会有冲突。
>
> 可参考：https://www.cnblogs.com/liqiuhao/p/8026100.html?utm_source=debugrun&utm_medium=referral

### (M,N)=(61,67)

注意到61和67均为素数，那么冲突的位置是不规律的。因此我们尽量按较大的块分，例如 (12*12) 分块：

```c
// TD;DR

// result
Function 0 (1 total)
Step 1: Validating and generating memory traces
Step 2: Evaluating performance (s=5, E=1, b=5)
func 0 (Transpose submission): hits:6279, misses:1900, evictions:1868
```

满足要求。
