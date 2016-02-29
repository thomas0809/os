#lec 3 SPOC Discussion
 
## 3.4 linux系统调用分析
 1. 通过分析[lab1_ex0](https://github.com/chyyuu/ucore_lab/blob/master/related_info/lab1/lab1-ex0.md)了解Linux应用的系统调用编写和含义。(w2l1)
 

 ```
  + 采分点：说明了objdump，nm，file的大致用途，说明了系统调用的具体含义
  - 答案没有涉及上述两个要点；（0分）
  - 答案对上述两个要点中的某一个要点进行了正确阐述（1分）
  - 答案对上述两个要点进行了正确阐述（2分）
  - 答案除了对上述两个要点都进行了正确阐述外，还进行了扩展和更丰富的说明（3分）
 
 ```
  - objdump命令用来将可执行文件进行反汇编
  - nm命令显示关于对象文件、可执行文件以及对象文件库里的符号信息
  - file命令用来检测给定文件的类型
  - 系统调用是操作系统提供给应用程序的服务接口，应用程序使用系统调用的时候，程序转入内核态执行，操作系统在系统调用表中根据系统调用编号查询需要执行的系统调用，并执行相应的代码。
   - 该程序使用了SYS\_write系统调用，调用前在`%eax`寄存器中存入系统调用编号（SYS\_write编号为4），在`%ebx`、`%ecx`、`%edx`中依次存入参数（文件句柄STDOUT、字符串地址、字符串长度），然后使用int指令执行系统调用。
 
 1. 通过调试[lab1_ex1](https://github.com/chyyuu/ucore_lab/blob/master/related_info/lab1/lab1-ex1.md)了解Linux应用的系统调用执行过程。(w2l1)
 

 ```
  + 采分点：说明了strace的大致用途，说明了系统调用的具体执行过程（包括应用，CPU硬件，操作系统的执行过程）
  - 答案没有涉及上述两个要点；（0分）
  - 答案对上述两个要点中的某一个要点进行了正确阐述（1分）
  - 答案对上述两个要点进行了正确阐述（2分）
  - 答案除了对上述两个要点都进行了正确阐述外，还进行了扩展和更丰富的说明（3分）
 ```
  - strace用于观察程序运行过程中的系统调用情况，其中包含有每个系统调用过程(syscall)的运行时间(seconds)、所占比例(%time)、调用次数(usecs/call)以及出现错误次数(errors)等。

  - 执行程序printf("hello world\n")，执行的系统调用包括execve、mprotect、fstat、brk、mmap、open、close、access、munmap、write、read、arch_prctl。
 
  - 程序首先调用execve，即执行文件，通过brk分配内存。接下来通过access、fstat等操作链接动态库，并利用open、close、read等进行文件操作。通过mmap来映射文件实现共享内存，mprotect修改内存的保护属性，最后调用write进行输出。程序执行完毕后，通过munmap解除内存映射，close关闭文件，exit_group退出程序。
 
## 3.5 ucore系统调用分析
 1. ucore的系统调用中参数传递代码分析。
  - 分析`kern/syscall.c`中的以下代码
   
	```
	void
	syscall(void) {
		struct trapframe *tf = current->tf;
		uint32_t arg[5];
		int num = tf->tf_regs.reg_eax;
		if (num >= 0 && num < NUM_SYSCALLS) {
			if (syscalls[num] != NULL) {
				arg[0] = tf->tf_regs.reg_edx;
				arg[1] = tf->tf_regs.reg_ecx;
				arg[2] = tf->tf_regs.reg_ebx;
				arg[3] = tf->tf_regs.reg_edi;
				arg[4] = tf->tf_regs.reg_esi;
				tf->tf_regs.reg_eax = syscalls[num](arg);
				return ;
			}
		}
		print_trapframe(tf);
		panic(“undefined syscall %d, pid = %d, name = %s.\n”,
		num, current->pid, current->name);
	}
	```
	可知，函数调用时`%eax`保存系统调用类型编号，函数参数依次通过`%edx`、`%ecx`、`%ebx`、`%edi`、`%esi`传入，结果通过`%eax`返回。
 1. 以getpid为例，分析ucore的系统调用中返回结果的传递代码。
  - `syscall`函数根据系统调用编号用函数指针的方式调用各个系统调用的具体实现，结果用`%eax`返回。（`tf->tf_regs.reg_eax = syscalls[num](arg);`）
  - getpid的实现如下，直接返回pid。
   
   ```
   static int
   sys_getpid(uint32_t arg[]) {
		return current->pid;
   }
   ```
 1. 以ucore lab8的answer为例，分析ucore 应用的系统调用编写和含义。
  - 在`usr/ulib.h`中声明了一些涉及系统调用的函数，比如`fork`、`getpid`、`fprintf`等，这些函数调用用户态的`sys_fork(void)`等函数，具体见`usr/syscall.h`；
  - 各个函数统一通过用户态的`syscall(int num, ...)`，使用`asm volatile`机制，将参数保存到寄存器中，然后用`int`指令进入内核态处理系统调用；
  - 内核态`syscall(void)`函数取出系统调用类型和参数，完成之后将返回值通过`%eax`返回，具体过程见第一题。
 1. 以ucore lab8的answer为例，尝试修改并运行ucore OS kernel代码，使其具有类似Linux应用工具`strace`的功能，即能够显示出应用程序发出的系统调用，从而可以分析ucore应用的系统调用执行过程。
  - 在内核态代码syscall.c中，加入cprintf输出系统调用的编号。
   ```
   cprintf("SYSCALL: %d\n", num);
   ```
  - 其中num为tf->tf_regs.reg_eax，在unistd.h中定义。
