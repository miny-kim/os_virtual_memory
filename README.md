## Project #3: Virtual Memory Simulator

### *** Due on 24:00, December 15 (Sunday) ***


### Goal
Implement a mini virtual memory system simulator.


### Problem Specification
- The framework code provides an environment to simulate paging. Your task is to implement key features of the simulator.

- In the simulator, we can have multiple processes in the system. Likewise PA2, each process is simplified to `struct process`, and `struct list_head processes` is the ready queue of the system. We have `@current` as well which points to the currently running process. You may manage the list as of PA2.

- The framework accepts three main commands, which are `read`, `write`, and `switch`, and two supplementary commands, which are `show` and `exit`.

- `read` and `write` is to instruct the system to simulate the memory access to the following VPN. For example;
  ```
	>> read 10    /* Read VPN 10 */
	>> write 0x10 /* Write to VPN 0x10 */
	```

- Each read and write request will be processed by the framework, and call `translate()` function to translate the VPN to PFN. You need to complete the function to translate VPN to PFN by walking through the page table of the current process.
If it cannot translate the address, return false to call in the page fault handler of the system.
This function is to simulate address translation in MMU so *you should not alter/allocate/fixup the page table in this function*.

- When the translation is successful (i.e., return `true` in `translate()`, the framework will print out the translation result and continue accepting the command from the prompt.

- When the translation is unsuccessful, it is equivalent to issue a page fault in MMU. The framework will call `handle_page_fault()` to handle that page fault. In this function, you need to inspect the situation, resolve the page fault accordingly, and return true. Thus, you may modify/allocate/fixup the page table in this function. Note that the entire virtual address space is supposed to be writable.

- You may switch the currently running process with `switch` command. Enter the command followed by the process id to switch, and then, the framework will call `switch_process()` to handle the request. Find the target process from `processes`, and if exists, do the context switching by replacing `@current` with it.

- If the target process does not exist, you need to fork a child process from @current. This implies you should allocate `struct process` for the child process and initialize it accordingly. To duplicate the parent's address space, set up the PTE in the child's page table to map to the same pfn of the parent. You need to set up the PTEs to support copy-on-write.

- `show` prompt command shows the page table of the current process.


### Tips and Restriction
- Likewise previous PAs, printing out to stdout does not influence on the grading.
- Implement features in an incremental way; implement the basic translation function first and get used to the page table/PTE manipulation first. And then implement the fork by duplicating the page table contents. You need to manipulate the PTEs of parent and child while implementing the feature to handle copy-on-write properly.
- As explained above, the entire address space is supposed to be writable. So, be careful to handle `writable` bit in the page table.


### Submission / Grading
- Use [PAsubmit](https://sslab.ajou.ac.kr/pasubmit) for submission
	- 320 pts + 10 pts 
	- Some testcases are hidden and only show the final decision (i.e., pass/fail);

- Code: ***pa3.c*** (300 pts)
  - Address translation (50 pts)
	- Fork (100 pts)
	- Copy-on-Write (150 pts)

- Document: One PDF document (20 pts) including;
	- Description how you implement address translation, fork, and copy-on-write
	- Lesson learned (if you have any)
	- No more than three pages
	- Otherwise you will get 0 pts for the report

- Git repository (10 pts)
	- Register http URL and with a deploy token and password.
	- Start the repository by cloning this repository.
	- Make sure the token is valid through December 20 (due + 4 slip days + 1 day)

- WILL NOT ANSWER THE QUESTIONS ABOUT THOSE ALREADY SPECIFIED ON THE HANDOUT.
