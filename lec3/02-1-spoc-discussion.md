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
   - 该程序使用了SYS\_write系统调用，调用前在`%eax`寄存器中存入系统调用编号（SYS\_write编号为4），在`%ebx`、`%ecx`、`%edx`中依次存入参数（文件句柄STDOUT、字符串地址、字符串长度）。
 
 1. 通过调试[lab1_ex1](https://github.com/chyyuu/ucore_lab/blob/master/related_info/lab1/lab1-ex1.md)了解Linux应用的系统调用执行过程。(w2l1)
 

 ```
  + 采分点：说明了strace的大致用途，说明了系统调用的具体执行过程（包括应用，CPU硬件，操作系统的执行过程）
  - 答案没有涉及上述两个要点；（0分）
  - 答案对上述两个要点中的某一个要点进行了正确阐述（1分）
  - 答案对上述两个要点进行了正确阐述（2分）
  - 答案除了对上述两个要点都进行了正确阐述外，还进行了扩展和更丰富的说明（3分）
 ```
  - strace用于观察程序运行过程中的系统调用情况，其中包含有每个系统调用过程(syscall)的运行时间(seconds)、所占比例(%time)、调用次数(usecs/call)以及出现错误次数(errors)等。

  - 执行程序printf("hello world\n")，包含的系统调用为:
     - mmap: 使得进程之间通过映射同一个普通文件实现共享内存
     - fstat: 由文件描述次取得文件状态
     - mprotect: 用来修改一段指定内存区域的保护属性
     - write: 向标准输出写
     - munmap: 接触内存映射，与mmap相对应
     - read: 读文件
     - open: 打开文件
     - access: 判断文件是否存在
     - evecve: 启动新的程序
 
## 3.5 ucore系统调用分析
 1. ucore的系统调用中参数传递代码分析。
 1. 以getpid为例，分析ucore的系统调用中返回结果的传递代码。
 1. 以ucore lab8的answer为例，分析ucore 应用的系统调用编写和含义。
 1. 以ucore lab8的answer为例，尝试修改并运行ucore OS kernel代码，使其具有类似Linux应用工具`strace`的功能，即能够显示出应用程序发出的系统调用，从而可以分析ucore应用的系统调用执行过程。
 