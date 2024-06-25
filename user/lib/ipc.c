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
// --- 信号注册函数 ---
int sigaction(int signum, const struct sigaction *newact, struct sigaction *oldact) {
	if (!is_legal_signo(signum)) {
		return -1;
	}
	if (oldact != NULL) {
		try(syscall_get_env_sigaction(0, signum, oldact));
	}
	if (newact != NULL) {
		try(syscall_set_env_sigaction(0, signum, newact));
	}
	return 0;
}

// --- 信号发送函数 ---
int kill(u_int envid, int sig) {
	if (!is_legal_signo(sig)) {
		return -1;
	}
	return syscall_sigaction_kill(envid, sig);
}
