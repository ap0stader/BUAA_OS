#include <bitops.h>
#include <env.h>
#include <pmap.h>

/* Lab 2 Key Code "tlb_invalidate" */
/* Overview:
 *   Invalidate the TLB entry with specified 'asid' and virtual address 'va'.
 *
 * Hint:
 *   Construct a new Entry HI and call 'tlb_out' to flush TLB.
 *   'tlb_out' is defined in mm/tlb_asm.S
 */
void tlb_invalidate(u_int asid, u_long va) {
	tlb_out((va & ~GENMASK(PGSHIFT, 0)) | (asid & (NASID - 1)));
}
/* End of Key Code "tlb_invalidate" */

static void passive_alloc(u_int va, Pde *pgdir, u_int asid) {
	struct Page *p = NULL;

	if (va < UTEMP) {
		sigaction_kill(0, SIGSEGV);
		// panic("address too low");
	}

	if (va >= USTACKTOP && va < USTACKTOP + PAGE_SIZE) {
		panic("invalid memory");
	}

	if (va >= UENVS && va < UPAGES) {
		panic("envs zone");
	}

	if (va >= UPAGES && va < UVPT) {
		panic("pages zone");
	}

	if (va >= ULIM) {
		panic("kernel address");
	}

	panic_on(page_alloc(&p));
	panic_on(page_insert(pgdir, asid, p, PTE_ADDR(va), (va >= UVPT && va < ULIM) ? 0 : PTE_D));
}

/* Overview:
 *  Refill TLB.
 */
void _do_tlb_refill(u_long *pentrylo, u_int va, u_int asid) {
	tlb_invalidate(asid, va);
	Pte *ppte;
	/* Hints:
	 *  Invoke 'page_lookup' repeatedly in a loop to find the page table entry '*ppte'
	 * associated with the virtual address 'va' in the current address space 'cur_pgdir'.
	 *
	 *  **While** 'page_lookup' returns 'NULL', indicating that the '*ppte' could not be found,
	 *  allocate a new page using 'passive_alloc' until 'page_lookup' succeeds.
	 */

	/* Exercise 2.9: Your code here. */
	while (page_lookup(cur_pgdir, va, &ppte) == NULL) {
		passive_alloc(va, cur_pgdir, asid);
	}
	ppte = (Pte *)((u_long)ppte & ~0x7);
	pentrylo[0] = ppte[0] >> 6;
	pentrylo[1] = ppte[1] >> 6;
}

#if !defined(LAB) || LAB >= 4
/* Overview:
 *   This is the TLB Mod exception handler in kernel.
 *   Our kernel allows user programs to handle TLB Mod exception in user mode, so we copy its
 *   context 'tf' into UXSTACK and modify the EPC to the registered user exception entry.
 *
 * Hints:
 *   'env_user_tlb_mod_entry' is the user space entry registered using
 *   'sys_set_user_tlb_mod_entry'.
 *
 *   The user entry should handle this TLB Mod exception and restore the context.
 */
void do_tlb_mod(struct Trapframe *tf) {
	struct Trapframe tmp_tf = *tf;

	if (tf->regs[29] < USTACKTOP || tf->regs[29] >= UXSTACKTOP) {
		tf->regs[29] = UXSTACKTOP;
	}
	tf->regs[29] -= sizeof(struct Trapframe);
	*(struct Trapframe *)tf->regs[29] = tmp_tf;
	// Removed useless codes
	if (curenv->env_user_tlb_mod_entry) {
		tf->regs[4] = tf->regs[29];
		tf->regs[29] -= sizeof(tf->regs[4]);
		// Hint: Set 'cp0_epc' in the context 'tf' to 'curenv->env_user_tlb_mod_entry'.
		/* Exercise 4.11: Your code here. */
		tf->cp0_epc = curenv->env_user_tlb_mod_entry;
	} else {
		panic("TLB Mod but no user handler registered");
	}
}
#endif

// challenge-sigaction
void do_sigaction(struct Trapframe *tf) {
	if (curenv->env_start_sigkill) {
		return;
	}
	int process_signo = 0;
	// 获取当前未被屏蔽的等待处理的信号
	// 对应位为1表示阻塞，为0表示未被阻塞，所以需要取反
	uint32_t unproc_signo = curenv->env_sigpending.sig & ~curenv->env_sigprocmask.sig;
	// 获取当前需要处理的信号
	if (curenv->env_sigpending.sig & signo2mask(SIGKILL)) {
		// 最优先考虑SIGKILL
		curenv->env_start_sigkill = 1;
		process_signo = SIGKILL;
	} else if (unproc_signo) {
		for (int i = MINSIGNO; i <= MAXSIGNO; i++) {
			// 信号越小，优先级越高
			if (unproc_signo & signo2mask(i)) {
				process_signo = i;
				break;
			}
		}
	}

	if (process_signo != 0) {
		// 入栈
		curenv->env_sigprocmask_stack[++curenv->env_sigprocmask_stack_top] = curenv->env_sigprocmask;
		// 修改
		curenv->env_sigpending.sig &= ~signo2mask(process_signo);
		curenv->env_sigprocmask.sig |= (curenv->env_sigaction[process_signo - 1].sa_mask.sig | signo2mask(process_signo)) & ~signo2mask(SIGKILL);

		struct Trapframe tmp_tf = *tf;
		// 复制到用户的异常处理栈保存以允许异常重入
		if (tf->regs[29] < UXSTACKTOP || tf->regs[29] >= UXSTACKTOP) {
			tf->regs[29] = UXSTACKTOP;
		}
		tf->regs[29] -= sizeof(struct Trapframe);
		*(struct Trapframe *)tf->regs[29] = tmp_tf;

		if (curenv->env_user_sigaction_entry) {
			tf->regs[4] = tf->regs[29];
			tf->regs[5] = (u_int)process_signo;
			tf->regs[6] = (u_int)curenv->env_sigaction[process_signo - 1].sa_handler;
			tf->regs[29] -= sizeof(tf->regs[4]);
			tf->regs[29] -= sizeof(tf->regs[5]);
			tf->regs[29] -= sizeof(tf->regs[6]);

			tf->cp0_epc = curenv->env_user_sigaction_entry;
		} else {
			panic("Sigaction but no user handler registered");
		}
	}
}
