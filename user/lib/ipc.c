// User-level IPC library routines

#include <env.h>
#include <lib.h>
#include <mmu.h>

// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void ipc_send(u_int whom, u_int val, const void *srcva, u_int perm) {
	int r;
	while ((r = syscall_ipc_try_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		syscall_yield();
	}
	user_assert(r == 0);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int ipc_recv(u_int *whom, void *dstva, u_int *perm) {
	int r = syscall_ipc_recv(dstva);
	if (r != 0) {
		user_panic("syscall_ipc_recv err: %d", r);
	}

	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}

// challenge-sigaction
void __attribute__((noreturn)) sigaction_entry(struct Trapframe *tf, int signo, void (*sa_handler)(int)) {
	// SIGKILL最优先处理
	if (signo == SIGKILL) {
		exit();
	} else if (sa_handler != NULL) {
		sa_handler(signo);
	} else if (signo == SIGINT || signo == SIGILL || signo == SIGSEGV) {
		// SIGINT/SIGILL/SIGSEGV默认处理是停止进程
		exit();
		// 其他的默认处理是忽略
	}
	int r = syscall_sigaction_finish(tf);
	user_panic("syscall_sigaction_finish returned %d", r);
}

// --- 信号注册函数 ---
int sigaction(int signo, const struct sigaction *newact, struct sigaction *oldact) {
	if (!is_legal_signo(signo)) {
		return -1;
	}
	if (oldact != NULL) {
		try(syscall_get_env_sigaction(0, signo, oldact));
	}
	if (newact != NULL) {
		try(syscall_set_env_sigaction(0, signo, newact));
	}
	return 0;
}

// --- 信号发送函数 ---
int kill(u_int envid, int signo) {
	if (!is_legal_signo(signo)) {
		return -1;
	}
	return syscall_sigaction_kill(envid, signo);
}

// --- 信号集处理函数 ---
int sigprocmask(int __how, const sigset_t *__set, sigset_t *__oset) {
	return syscall_change_curenv_sigprocmask(__how, __set, __oset);
}

int sigpending(sigset_t *__set) {
	if (__set == NULL) {
		return -1;
	}
	return syscall_get_curenv_sigpending(__set);
}
