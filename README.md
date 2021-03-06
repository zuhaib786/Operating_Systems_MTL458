# Operating Systems Codes
This repository contains codes of the assignments submitted by me in the course MTL458-Operating Systems(Instructor: Ashutosh Rai), of Indian Institue of Technology, Delhi.
## Assignment 1: Shell.C
This is the basic implementation of Shell in C.
The shell gives the file directory path name as __MTL458__. If a path does not contain this directory path as its subpath then the whole absolute path is printed, else the __MTL458__ is appended to the rest of the path
The commands supported by the shell are __cd__ command, Commands which can be executed using __execvp__ like __cat__, __ls__ etc and __history__ command. The commands __cd__ and __history__ have the properties different from shell and follow the requirements as demanded by the assignment.
1. __cd__ Command: The shell gives the file directory path name as __MTL458__. If a path does not contain this directory path as its subpath then the whole absolute path is printed, else the __MTL458__ is appended to the rest of the path
So suppose our file directory is __./home/Competitive_Programming/Operating_Systems__, then running the shell for the first time will print on prompt as __MTL458~__.Now suppose we enter the command __cd Assignment1__, then the path printed on promt will be __MTL458/Assignment1__.
However if we enter the command __cd ..__, then since the absolute path is __./home/Competitive_Programming/__, which does not contain file directory as subpath, the absolute path will be printed. cd followed by empty space does not cause error and no action is taken. If the command __cd .__ is entered, then the current working directory is changed to home directory. Rest cd works exactly as normal cd in bash.
2. __History__: This command prints the past most recent 5 commands(not including the current command). If the number of commands is less than 5, then all the commands are printed.

## Assignment 2: my_alloc.c
This is the implementation of malloc in C. We map a block of 4KB using mmap command in C. The description of the various functions implemented are as follows.
1. __my_init()__: This is the initialization function for the memory allocator. It uses the mmap system call to request a page of 4KB from the memory and initializes a free list.The 4KB page is the space that the memory allocator will be working with. The user will first invoke my_init() before requesting any memory. It returns 0 on successful completion, otherwise the error code. 
2. my_alloc(int): This is our version of malloc(int). Any space asked for should be a multiple of 8 bytes, otherwise the function returns NULL, which it also does in case of any other error. As expected, just like malloc(int), it would return a pointer to the address assigned in case of successful completion.
3. my_free(void *): Takes a pointer to a previously allocated chunk and frees up that chunk, just like free(char *) does.
4. my_clean(): This is done at the end to return the memory back to the OS using munmap system call
5. my_heapinfo(): This gives some useful information about the current state of heap. This includes the maximum size allowed for the heap, the size of the current heap, total size of the free memory in the heap, number of blocks allocated, size of a smallest available chunk, and the size of a largest available chunk.

## Assignment 3: Page_Table_Simulation.c
In this assignment,we simulate a page table and given a sequence of memory accesses and whether each access was read or write, we output the number of page faults, the number of reads from the disk and the number of writes to the disk
The basic implementational details about this assignment are:
1. We implemented 5 strategies, OPT, FIFO, CLOCK, LRU, and RANDOM.
2. The program takes as input a trace file(e.g. trace.in), and integer denoting the number of frames available, and a string which would denote the strategy. For instance, if the executable is named foo, then an example command is ./foo trace.in 100 OPT, i.e. we simulate the memory accesses in file trace.in when number of pages is 100 using optimal page replacement policy.
3. The trace file is a list of virtual memory addresses followed by letter "R" or "W", which would indicate whether the memory access was a read or write. Each line of the trace file
would have one such memory access. A part of the file might look like the following.
0x4000a340 R
0x4000a345 R
0x4000a34b R
0x4000a353 W
0x4000a355 R
4. The string for the strategies are named as following: OPT, FIFO, CLOCK, LRU, and RANDOM.
5. The size of one frame is 8KB in a  32-bit virtual address space, and we assume that the physical memory can hold number of frames that is given as an integer input (maximum 1000).
6. All the memory references are generated by the same process, so we maintain only one page table. Also, since the process never ends, once all the frames in physical memory are full, they will not be freed up again, except to bring a page from the memory.
7. Number the frames from 0 to n ??? 1 where n was the input integer. Initially, we assume all the frames to be free(cold start) and start assigning from the first. Page replacements
are done after all the frames get full.
8. We keep both Page_Table as well as inverted page Table to make simulation easy. Also some additional data structures are created in cases of other strategies to make simulation fast(e.g. Heap in case of least recently used).
9. If the page to be replaced is not dirty, it does not need to get written, it is simply ???dropped??? from the physical memory.
10. For each of the strategies, output looks like the following.
Number of memory accesses:
Number of misses:
Number of writes:
Number of drops:
11. In addition to the above, if the -verbose option is used then after each page replacement, the program should print which page (page number in hex) was brought in, and which page was dropped/written on disk. The format would be like one of the lines below.
__Page 0x40345 was read from disk, page 0x40310 was written to the disk.__
__Page 0x40345 was read from disk, page 0x40310 was dropped (it was not dirty).__

## Assignment 4: assignment_code

In this assignment, we implement two versions of reader-writer locks. 
These will be used in situations where there is some shared data, and some users (called readers) want to read the data, while some other users (called writers) want to write into the data. The idea is to give access to other reader(s) when some reader(s) is(are) reading the data. On the other hand, the writer would need to have exclusive access to the data, where no other
writer(s) or reader(s) would be allowed to access the data when it is being modified. 
A reader-writer lock is a special kind of lock where the user thread can specify whether it is a reader or a writer by requesting the corresponding lock using the functions ReaderLock() and WriterLock(). 
There are two flavors of reader-writer locks. 
The first one is reader-preference lock, where a reader would be allowed to access the data even when a writer is waiting. Observe that while this is good for concurrency (many readers can access the data), but a writer may starve due to a steady stream of very small number of readers. 
The other version of the lock is writer-preference lock, where no more readers would be allowed to acquire the reader lock, once a writer is waiting for a lock to modify the data. We implement a reader-writer lock in this assignment which can be used for both reader and writer preference
locks.
The implementation details are as follows:
1. The function InitalizeReadWriteLock() initializes the lock.
2. The file rwlock-reader-pref.c contains the reader-preference version of the functions.
3. Similarly, the file rwlock-writer-pref.c contains the writer-preference version of the functions.
4. Both the versions share the same header file, which contains the definition of the lock structure. So the lock structure is going to be common for both versions, but contains elements which are used in only one version of the lock.
5. There are also two additional tester programs that can be used to check the correctness of the implementation.
6. There are a couple of scripts (run_reader_pref.sh and run_writer_pref.sh) that run these programs with with some arguments. Inside the .sh files, the executables are followed by two arguments which represents number of reader and writer threads created.
For example, ./rwlock-writer-pref 5 2 creates 5 reader threads followed by 2 writer threads and then lastly by additional 5 reader threads. These arguments can be modified for testing your code.
7. The commands in these .sh files can be used to compile and execute the code, and run with different arguments to check the correctness.
