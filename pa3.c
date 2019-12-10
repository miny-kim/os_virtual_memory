/**********************************************************************
 * Copyright (c) 2019
 *  Sang-Hoon Kim <sanghoonkim@ajou.ac.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "types.h"
#include "list_head.h"
#include "vm.h"

/**
 * Ready queue of the system
 */
extern struct list_head processes;

/**
 * The current process
 */
extern struct process *current;

/**
 * alloc_page()
 *
 * DESCRIPTION
 *   Allocate a page from the system. This function is implemented in vm.c
 *   and use to get a page frame from the system.
 *
 * RETURN
 *   PFN of the newly allocated page frame.
 */
extern unsigned int alloc_page(void);//handle_page_fault

int type;
//1 : page_directory is not exists 
//2 : pte is not vaild
//3 : pte is not writable


/**
 * TODO translate()
 *
 * DESCRIPTION
 *   Translate @vpn of the @current to @pfn. To this end, walk through the
 *   page table of @current and find the corresponding PTE of @vpn.
 *   If such an entry exists and OK to access the pfn in the PTE, fill @pfn
 *   with the pfn of the PTE and return true.
 *   Otherwise, return false.
 *   Note that you should not modify any part of the page table in this function.
 *
 * RETURN
 *   @true on successful translation
 *   @false on unable to translate. This includes the case when @rw is for write
 *   and the @writable of the pte is false.
 */
bool translate(enum memory_access_type rw, unsigned int vpn, unsigned int *pfn)
{
	/*** DO NOT MODIFY THE PAGE TABLE IN THIS FUNCTION ***/
   // printf("vpn:%d\n", vpn);
    int second_idx = vpn & 15;
    int first_idx =( vpn>> 4)&15 ;
//   printf("second, first : %d, %d\n", second_idx, first_idx);
    struct pte_directory *  addr = current->pagetable.outer_ptes[first_idx];
    
    if(addr)
    {
        if(addr->ptes[second_idx].valid)
        {
            if(addr->ptes[second_idx].writable == false)
            {
                type = 3;
                return false;
            }

            *pfn = addr->ptes[second_idx].pfn;
            return true;
        }
        else
        {
            type = 2;
            return false;
        }
      // *pfn = pfnn;
        //return true;
    }    //    printf("rw : %d\n", rw);
//    int ppfn = current->pagetable.outer_ptes[vpn];
    type = 1;
	return false;
}


/**
 * TODO handle_page_fault()
 *
 * DESCRIPTION
 *   Handle the page fault for accessing @vpn for @rw. This function is called
 *   by the framework when the translate() for @vpn fails. This implies;
 *   1. Corresponding pte_directory is not exist
 *   2. pte is not valid
 *   3. pte is not writable but @rw is for write
 *   You can assume that all pages are writable; this means, when a page fault
 *   happens with valid PTE without writable permission, it was set for the
 *   copy-on-write.
 *
 * RETURN
 *   @true on successful fault handling
 *   @false otherwise */

bool handle_page_fault(enum memory_access_type rw, unsigned int vpn)
{
    printf("handle_page_fault_vpn : %d\n", vpn);
    int second_idx =vpn & 15 ;
    int first_idx = (vpn >> 4)&15;
    
    
    if(type !=3)
    {
        printf("!!!!type 1!!!!\n");
        if(!(current->pagetable.outer_ptes[first_idx]))//type1
            current->pagetable.outer_ptes[first_idx] = (struct pte_directory *)malloc(sizeof(struct pte_directory));

        //type2
        current->pagetable.outer_ptes[first_idx]->ptes[second_idx].pfn = alloc_page();
        current->pagetable.outer_ptes[first_idx]->ptes[second_idx].valid = true;
  //  printf("a:%d\n", a);
        current->pagetable.outer_ptes[first_idx]->ptes[second_idx].writable = true;
    }
    else
    {
        printf("!!!!!type 3!!!!!\n");
         struct pte_directory * new_directory  = (struct pte_directory *)malloc(sizeof(struct pte_directory));       
       // current->pagetable.outer_ptes[first_idx] = new_directory;
        
            struct pte_directory *pd = current->pagetable.outer_ptes[first_idx];
                
            for(int j = 0; j<NR_PTES_PER_PAGE; j++){
                struct pte *pte = &pd->ptes[j];
                printf("j:%d\n", j);
                if(!pte->valid)continue;
                
                new_directory->ptes[j] = *pte;
                
                if(j == second_idx)
                {
                    new_directory->ptes[j].pfn = alloc_page();
                    new_directory->ptes[j].valid = true;
                    new_directory->ptes[j].writable = true;
                }

    
            }

            current->pagetable.outer_ptes[first_idx] = new_directory;
    }
        


    return true;
}


/**
 * TODO switch_process()
 *
 * DESCRIPTION
 *   If there is a process with @pid in @processes, switch to the process.
 *   The @current process at the moment should be put to the **TAIL** of the
 *   @processes list, and @current should be replaced to the requested process.
 *   Make sure that the next process is unlinked from the @processes.
 *   If there is no process with @pid in the @processes list, fork a process
 *   from the @current. This implies the forked child process should have
 *   the identical page table entry 'values' to its parent's (i.e., @current)
 *   page table. Also, should update the writable bit properly to implement
 *   the copy-on-write feature.
 */
void switch_process(unsigned int pid)
{
    struct process * next=NULL;
    struct process * pnext = NULL;
    struct process * end=NULL;
    int f = false;


    if(!list_empty(&processes)){
        next = list_first_entry(&processes, struct process, list);
        end = list_last_entry(&processes, struct process, list);
        pnext = next;

        while(pnext){

            if(pid == pnext->pid)
            {
                next = pnext;
                f = true;
                list_del_init(&next->list);
                break;
            }

            if(pnext->pid == end->pid)
                break;

            pnext = list_next_entry(pnext, list);
        } 
            
       // list_del_init(&next->list);
       // current = next;

/*    list_for_each_entry(tmp, &processes, list)
    {
        printf("tmp->pid : %d\n", tmp->pid);
        if(pid == tmp->pid)
        {
            list_add_tail(&current->list, &processes);
            list_del_init(&tmp->list);
            f = true;
            break;
        }
        printf("f:%d\n", f);
    }
*/  }
//    printf("flag : %d\n", f);
    if(!f)
    {
        struct pagetable * new_table  = (struct pagetable *)malloc(sizeof(struct pagetable));       
        next = (struct process *)malloc(sizeof(struct process));
  //      printf("next address : %p\n", next);
        next->pid = pid;
        next->list = current->list;
        next->pagetable = *new_table;

        for(int i = 0; i<NR_PTES_PER_PAGE; i++){
            struct pte_directory *pd = current->pagetable.outer_ptes[i];
    //        printf("current_pid : %d\n", current->pid);
            if(!pd) continue;
                
            next->pagetable.outer_ptes[i] = pd;

            for(int j = 0; j<NR_PTES_PER_PAGE; j++){
                struct pte *pte = &pd->ptes[j];

                if(!pte->valid)continue;
                
                pte->writable = false;
      //          printf("j : %d\n", j);
    
            }
        }


    }
        list_add_tail(&current->list, &processes);
        current = next;
}

