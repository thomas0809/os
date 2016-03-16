#lec8 虚拟内存spoc练习


## 小组思考题目
----

### 虚拟页式存储的地址转换

（3）(spoc) 有一台假想的计算机，页大小（page size）为32 Bytes，支持8KB的虚拟地址空间（virtual address space）,有4KB的物理内存空间（physical memory），采用二级页表，一个页目录项（page directory entry ，PDE）大小为1 Byte,一个页表项（page-table entries
PTEs）大小为1 Byte，1个页目录表大小为32 Bytes，1个页表大小为32 Bytes。页目录基址寄存器（page directory base register，PDBR）保存了页目录表的物理地址（按页对齐）。

PTE格式（8 bit） :

```
  VALID | PFN6 ... PFN0
```
PDE格式（8 bit） :

```
  VALID | PT6 ... PT0
```
其

```
VALID==1表示，表示映射存在；VALID==0表示，表示内存映射不存在（有两种情况：a.对应的物理页帧swap out在硬盘上；b.既没有在内存中，页没有在硬盘上，这时页帧号为0x7F）。
PFN6..0:页帧号或外存中的后备页号
PT6..0:页表的物理基址>>5
```

已经建立好了1个页目录表和8个页表，且页目录表的index为0~7的页目录项分别对应了这8个页表。

在[物理内存模拟数据文件](./04-1-spoc-memdiskdata.md)中，给出了4KB物理内存空间和4KBdisk空间的值，PDBR的值。

请手工计算后回答下列虚地址是否有合法对应的物理内存，请给出对应的pde index, pde contents, pte index, pte contents，the value of addr in phy page OR disk sector。

```
1) Virtual Address 6653:
2) Virtual Address 1c13:
3) Virtual Address 6890:
4) Virtual Address 0af6:
5) Virtual Address 1e6f:
```

请写出一个translation程序（可基于python、ruby、C、C++、LISP、JavaScript等），输入是一个虚拟地址和一个物理地址，依据[物理内存模拟数据文件](./04-1-spoc-memdiskdata.md)自动计算出对应的pde index, pde contents, pte index, pte contents，the value of addr in phy page OR disk sector。

#### 程序

[代码 convert.py](convert.py)

```python
PDE_BASE = 0xd80

def getmem(filename):
	f = file(filename)
	mem = []
	for line in f.readlines():
		a = line.strip().split(' ')
		for x in a[2:]:
			mem.append(int(x, 16))
	f.close()
	return mem

def getcontent(base, index, mem):
	return mem[base + index]

def convert(va, mem, disk):
	print 'Virtual Address 0x%x:' %va
	pde_index = (va >> 10) & 0x1f
	pte_index = (va >> 5) & 0x1f
	offset = va & 0x1f
	pde_content = getcontent(PDE_BASE, pde_index, mem)
	pde_valid = pde_content >> 7
	pt = pde_content & 0x7f

	print '\t--> pde index: 0x%x pde contents: (valid 0x%x, pt 0x%x)' % (pde_index, pde_valid, pt)

	if pde_valid == 0:
		print '\t\t--> Fault (page directory entry not valid)'
		return
	
	pte_base = pt << 5
	pte_content = getcontent(pte_base, pte_index, mem)
	pte_valid = pte_content >> 7
	pfn = pte_content & 0x7f

	print '\t\t--> pte index: 0x%x pte contents: (valid 0x%x, pfn 0x%x)' % (pte_index, pte_valid, pfn)

	if pte_valid == 0:
		if pfn == 0x7f:
			print '\t\t\t--> Fault (page table entry not valid)'
			return
		disk_base = pfn << 5
		disk_content = getcontent(disk_base, offset, disk)
		print '\t\t\t--> To Disk Sector Address 0x%x --> Value: 0x%x' % (disk_base + offset, disk_content)
		return

	mem_base = pfn << 5
	mem_content = getcontent(mem_base, offset, mem)
	print '\t\t\t--> To Physical Address 0x%x --> Value: 0x%x' % (mem_base + offset, mem_content)


if __name__ == '__main__':
	mem = getmem('mem.txt')
	disk = getmem('disk.txt')
	f = file('input.txt')
	for line in f.readlines():
		va = int(line, 16)
		convert(va, mem, disk)

```

#### 运行结果

```
Virtual Address 0x6653:
	--> pde index: 0x19 pde contents: (valid 0x0, pt 0x7f)
		--> Fault (page directory entry not valid)
Virtual Address 0x1c13:
	--> pde index: 0x7 pde contents: (valid 0x1, pt 0x3d)
		--> pte index: 0x0 pte contents: (valid 0x1, pfn 0x76)
			--> To Physical Address 0xed3 --> Value: 0x12
Virtual Address 0x6890:
	--> pde index: 0x1a pde contents: (valid 0x0, pt 0x7f)
		--> Fault (page directory entry not valid)
Virtual Address 0xaf6:
	--> pde index: 0x2 pde contents: (valid 0x1, pt 0x21)
		--> pte index: 0x17 pte contents: (valid 0x0, pfn 0x7f)
			--> Fault (page table entry not valid)
Virtual Address 0x1e6f:
	--> pde index: 0x7 pde contents: (valid 0x1, pt 0x3d)
		--> pte index: 0x13 pte contents: (valid 0x0, pfn 0x16)
			--> To Disk Sector Address 0x2cf --> Value: 0x1c
```

## v9-cpu相关

[challenge]在v9-cpu上，设定物理内存为64MB。在os.c,os2.c,os4.c,os5的基础上实现os6.c，可体现基本虚拟内存管理机制，内核空间的映射关系： kernel_virt_addr=0xc00000000+phy_addr，内核空间大小为64MB，虚拟空间范围为0xc0000000--x0xc4000000, 物理空间范围为0x00000000--x0x04000000；用户空间的映射关系：user_virt_addr=0x40000000+usr_phy_addr，用户空间可用大小为2MB，虚拟空间范围为0x40000000--0x40200000，物理空间范围为0x02000000--x0x02200000，但只建立低地址的1MB的用户空间页表映射。可参考v9-cpu git repo的testing分支中的os.c和mem.h。修改代码为os5.c

- (1)在建立页表后，进入用户态，能够在用户态访问基于用户空间的映射关系
- (2)在用户态和内核态产生各种也访问的错误，并能够通过中断服务例程进行信息提示
- (3)内核通过中断服务例程在感知到用户态访问高地址的空间，且没有超过0x40200000时，内核动态建立页表，确保用户态程序可以正确运行

