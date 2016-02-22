# 操作系统概述
---
## v9-cpu相关题目
---

请分析em.c，并补充cpu.md中描述不够或错误的地方。

### 在v9-cpu中如何实现时钟中断的；

timer为计时器，timer>=timeout时产生超时事件，timer清零，若中断使能打开（iean=1），则trap置为FTIMER，iena置为0，进入中断处理；若中断使能关闭（iean=0），将FTIMER中断加入ipend。

### v9-cpu指令，关键变量描述有误或不全的情况；

ssp: 内核态的sp

usp: 用户态的sp

cycle: 

xcycle:

timer: 计时变量

timeout: 超时阈值

delta: 单位时长（4096）


### 在v9-cpu中的跳转相关操作是如何实现的；

JMP/JMPI: 根据operand0的值，修改xpc；

JSR/JSRA: 先将当前xpc保存在栈中，然后跳转；

conditional branch: 先判断相应的条件，然后决定是否跳转；

### 在v9-cpu中如何设计相应指令，可有效实现函数调用与返回；

函数的调用与返回可以使用JSR和JSRA指令，即调用时先在栈中保存当前pc，返回时再取出pc。参数和返回值可以通过寄存器传递，或者按顺序保存在栈中。

### emhello/os0/os1等程序被加载到内存的哪个位置,其堆栈是如何设置的；

```
  if ((f = open(file, O_RDONLY)) < 0) { dprintf(2,"%s : couldn't open %s\n", cmd, file); return -1; }
  if (fstat(f, &st)) { dprintf(2,"%s : couldn't stat file %s\n", cmd, file); return -1; }
	
  read(f, &hdr, sizeof(hdr));
  if (hdr.magic != 0xC0DEF00D) { dprintf(2,"%s : bad hdr.magic\n", cmd); return -1; }
  
  if (read(f, (void*)mem, st.st_size - sizeof(hdr)) != st.st_size - sizeof(hdr)) { dprintf(2,"%s : failed to read file %sn", cmd, file); return -1; }
  close(f); 
```
从以上代码可以看到，程序被加载到mem数组的开始，即内存的0位置开始。

栈底初始位置为128M-4M=124M处（内存总大小-RAM文件系统大小）。

### 在v9-cpu中如何完成一次内存地址的读写的；

```
case LX:   if (!(p = tr[(v = a + (ir>>8)) >> 12]) && !(p = rlook(v))) break; a = *(uint *)   ((v ^ p) & -4); continue;
```
以LX指令为例，根据虚地址v求出虚页号，先在当前TLB即tr中查看有没有对应的实页号，如果有则可得到物理地址，如果没有再用用rlook()函数查找，先根据v的高10位找到page directory entry (pde)，对应一个页表，再根据v的中间10位找到page table entry (pte)，就得到了对应的内存页。

写和读同理，区别是使用tw和wlook()。

### 在v9-cpu中如何实现分页机制；

初始化时在内存中分配了四个表trk、twk、tru、twu，分别用于内核态（k）、用户态（u）下的读地址（r）、写地址（w）翻译。tr、tw根据当前是什么态指向trk、twk或tru、twu。

访存时，先查看TLB（tr或tw）中有没有对应项，如果没有再用rlook/wlook查找页表中的对应项。使用的页表是二级页表，地址高10位是page directory entry，中间10位是page table entry。

如果分页机制没有打开，虚地址直接作为实地址返回。
