# Lab8 Report

## 练习1：完成读文件操作的实现

在`kern/fs/sfs/sfs_inode.c`中完成`sys_io_nolock`函数，完成读取写入操作。代码如下：

```c
static int
sfs_io_nolock(struct sfs_fs *sfs, struct sfs_inode *sin, void *buf, off_t offset, size_t *alenp, bool write) {
    struct sfs_disk_inode *din = sin->din;
    assert(din->type != SFS_TYPE_DIR);
    off_t endpos = offset + *alenp, blkoff;
    *alenp = 0;
	// calculate the Rd/Wr end position
    if (offset < 0 || offset >= SFS_MAX_FILE_SIZE || offset > endpos) {
        return -E_INVAL;
    }
    if (offset == endpos) {
        return 0;
    }
    if (endpos > SFS_MAX_FILE_SIZE) {
        endpos = SFS_MAX_FILE_SIZE;
    }
    if (!write) {
        if (offset >= din->size) {
            return 0;
        }
        if (endpos > din->size) {
            endpos = din->size;
        }
    }

    int (*sfs_buf_op)(struct sfs_fs *sfs, void *buf, size_t len, uint32_t blkno, off_t offset);
    int (*sfs_block_op)(struct sfs_fs *sfs, void *buf, uint32_t blkno, uint32_t nblks);
    if (write) {
        sfs_buf_op = sfs_wbuf, sfs_block_op = sfs_wblock;
    }
    else {
        sfs_buf_op = sfs_rbuf, sfs_block_op = sfs_rblock;
    }

    int ret = 0;
    size_t size, alen = 0;
    uint32_t ino;
    uint32_t blkno = offset / SFS_BLKSIZE;          // The NO. of Rd/Wr begin block
    uint32_t nblks = endpos / SFS_BLKSIZE - blkno;  // The size of Rd/Wr blocks

  //LAB8:EXERCISE1 YOUR CODE HINT: call sfs_bmap_load_nolock, sfs_rbuf, sfs_rblock,etc. read different kind of blocks in file
	/*
	 * (1) If offset isn't aligned with the first block, Rd/Wr some content from offset to the end of the first block
	 *       NOTICE: useful function: sfs_bmap_load_nolock, sfs_buf_op
	 *               Rd/Wr size = (nblks != 0) ? (SFS_BLKSIZE - blkoff) : (endpos - offset)
	 * (2) Rd/Wr aligned blocks 
	 *       NOTICE: useful function: sfs_bmap_load_nolock, sfs_block_op
     * (3) If end position isn't aligned with the last block, Rd/Wr some content from begin to the (endpos % SFS_BLKSIZE) of the last block
	 *       NOTICE: useful function: sfs_bmap_load_nolock, sfs_buf_op	
	*/
    blkoff = offset % SFS_BLKSIZE;
    if (blkoff != 0) {
        size = (nblks != 0) ? (SFS_BLKSIZE - blkoff) : (endpos - offset);

        if ((ret = sfs_bmap_load_nolock(sfs, sin, blkno, &ino)) != 0) {
            goto out;
        }
        if ((ret = sfs_buf_op(sfs, buf, size, ino, blkoff)) != 0) {
            goto out;
        }
        alen += size;
        if (nblks == 0) {
            goto out;
        }
        buf += size;
        blkno++;
        nblks--;
    }

    size = SFS_BLKSIZE;
    while (nblks > 0) {
        if ((ret = sfs_bmap_load_nolock(sfs, sin, blkno, &ino)) != 0) {
            goto out;
        }
        if ((ret = sfs_block_op(sfs, buf, ino, 1)) != 0) {
            goto out;
        }
        alen += size;
        buf += size;
        blkno++;
        nblks--;
    }

    blkoff = 0;
    size = endpos % SFS_BLKSIZE;
    if (size > 0) {
        if ((ret = sfs_bmap_load_nolock(sfs, sin, blkno, &ino)) != 0) {
            goto out;
        }
        if ((ret = sfs_buf_op(sfs, buf, size, ino, blkoff)) != 0) {
            goto out;
        }
        alen += size;
    }
out:
    *alenp = alen;
    if (offset + alen > sin->din->size) {
        sin->din->size = offset + alen;
        sin->dirty = 1;
    }
    return ret;
}
```

代码的执行步骤为：

1. 处理起始地址所在的块。判断起始地址的`offset`是否对齐，如果不对齐，计算块内偏移`blkoff`和大小`size`。调用`sfs_bmap_load_nolock`获取`ino`，再调用`sfs_buf_op`完成实际的读写操作。完成后，需要增加完成的大小`alen`，缓冲区指针`buf`前移，并对`blkno`和`nblks`更新。
2. 处理中间完整的块。对于每一块，调用`sfs_bmap_load_nolock`获取`ino`，再调用`sfs_buf_op`完成实际的读写操作。对其他变量的更新与步骤一类似。
3. 处理最后的没有对齐的块。这时，`blkoff`为0，计算`size`。调用`sfs_bmap_load_nolock`获取`ino`，再调用`sfs_buf_op`完成实际的读写操作。


### 问题1.1：给出设计实现“UNIX的PIPE机制”的概要设方案，鼓励给出详细设计方案

设置一个链表维护缓冲区的数据。当链表为空时，表示缓冲区内没有数据。缓冲区没有满时，进程可以写数据，写的数据插入链表尾部，否则将进程阻塞。当缓冲区不是空时，进程可以读数据，读出的数据从链表头部删除，否则也将进程阻塞。

与“读者-写者”问题类似，需要考虑到互斥访问的情况，这时可以使用信号量或者管程来实现互斥操作。









    


