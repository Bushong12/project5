# project5
Virtual Memory
Grace Bushong: gbushong
Erin Turley: eturley
Operating Systems: 04/07/2017

Project Description: In this project, you will build a simple but fully functional demand paged virtual memory. Although virtual memory is normally implemented in the operating system kernel, it can also be implemented at the user level. This is exactly the technique used by modern virtual machines, so you will be learning an advanced technique without having the headache of writing kernel-level code. We will provide you with code that implements a "virtual" page table and a "virtual" disk. The virtual page table will create a small virtual and physical memory, along with methods for updating the page table entries and protection bits. When an application uses the virtual memory, it will result in page faults that call a custom handler. Most of your job is to implement a page fault handler that will trap page faults and identify the correct course of action, which generally means updating the page table, and moving data back and forth between the disk and physical memory.

Once your system is working correctly, you will evaluate the performance of several page replacement algorithms on a selection of simple programs across a range of memory sizes. You will write a short lab report that explains the experiments, describes your results, and draws conclusions about the behavior of each algorithm. 
