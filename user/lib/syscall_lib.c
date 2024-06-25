#include <env.h>
#include <lib.h>
#include <mmu.h>
#include <syscall.h>
#include <trap.h>

void syscall_putchar(int ch) {
	msyscall(SYS_putchar, ch);
}

int syscall_print_cons(const void *str, u_int num) {
	return msyscall(SYS_print_cons, str, num);
}

u_int syscall_getenvid(void) {
	return msyscall(SYS_getenvid);
}

void syscall_yield(void) {
	msyscall(SYS_yield);
}

int syscall_env_destroy(u_int envid) {
	return msyscall(SYS_env_destroy, envid);
}

int syscall_set_tlb_mod_entry(u_int envid, void (*func)(struct Trapframe *)) {
	return msyscall(SYS_set_tlb_mod_entry, envid, func);
}

int syscall_mem_alloc(u_int envid, void *va, u_int perm) {
	return msyscall(SYS_mem_alloc, envid, va, perm);
}

int syscall_mem_map(u_int srcid, void *srcva, u_int dstid, void *dstva, u_int perm) {
	return msyscall(SYS_mem_map, srcid, srcva, dstid, dstva, perm);
}

int syscall_mem_unmap(u_int envid, void *va) {
	return msyscall(SYS_mem_unmap, envid, va);
}

int syscall_set_env_status(u_int envid, u_int status) {
	return msyscall(SYS_set_env_status, envid, status);
}

int syscall_set_trapframe(u_int envid, struct Trapframe *tf) {
	return msyscall(SYS_set_trapframe, envid, tf);
}

void syscall_panic(const char *msg) {
	int r = msyscall(SYS_panic, msg);
	user_panic("SYS_panic returned %d", r);
}

int syscall_ipc_try_send(u_int envid, u_int value, const void *srcva, u_int perm) {
	return msyscall(SYS_ipc_try_send, envid, value, srcva, perm);
}

int syscall_ipc_recv(void *dstva) {
	return msyscall(SYS_ipc_recv, dstva);
}

int syscall_cgetc() {
	return msyscall(SYS_cgetc);
}

int syscall_write_dev(void *va, u_int dev, u_int size) {
	/* Exercise 5.2: Your code here. (1/2) */
	return msyscall(SYS_write_dev, va, dev, size);
}

int syscall_read_dev(void *va, u_int dev, u_int size) {
	/* Exercise 5.2: Your code here. (2/2) */
	return msyscall(SYS_read_dev, va, dev, size);
}

// challenge-sigaction
int syscall_get_env_sigaction(int signo, struct sigaction *oldact) {
	return msyscall(SYS_get_env_sigaction, signo, oldact);
}

int syscall_set_env_sigaction(int signo, const struct sigaction *newact) {
	return msyscall(SYS_set_env_sigaction, signo, newact);
}

int syscall_sigaction_kill(u_int envid, int signo) {
	return msyscall(SYS_sigaction_kill, envid, signo);
}

int syscall_sigaction_finish(struct Trapframe *tf) {
	return msyscall(SYS_sigaction_finish, tf);
}

int syscall_change_curenv_sigprocmask(int __how, const sigset_t *__set, sigset_t *__oset) {
	return msyscall(SYS_change_curenv_sigprocmask, __how, __set, __oset);
}

int syscall_get_curenv_sigpending(sigset_t *__set) {
	return msyscall(SYS_get_curenv_sigpending, __set);
}

int syscall_set_curenv_sigaction_entry(void (*func)(struct Trapframe *, int, void (*)(int))) {
	return msyscall(SYS_set_curenv_sigaction_entry, func);
}
