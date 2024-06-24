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

// --- 信号发送函数 ---

// --- 信号集处理函数 ---
static inline int is_legal_signo(int __signo) {
    return 1 <= __signo && __signo <= 32;
}

static inline uint32_t signo2mask(int __signo) {
	return (uint32_t)(1 << (__signo - 1));
}

int sigemptyset(sigset_t *__set) {
    if (__set != NULL) {
        __set->sig = (uint32_t)0x00000000;
        return 0;
    } else {
        return -1;
        // return -E_UNSPECIFIED;
    }
}

int sigfillset(sigset_t *__set) {
    if (__set != NULL) {
        __set->sig = (uint32_t)0xFFFFFFFF;
        return 0;
    } else {
        return -1;
        // return -E_UNSPECIFIED;
    }
}

int sigaddset(sigset_t *__set, int __signo) {
    if (__set != NULL && is_legal_signo(__signo)) {
        __set->sig |= signo2mask(__signo);
        return 0;
    } else {
        return -1;
        // return -E_UNSPECIFIED;
    }
}

int sigdelset(sigset_t *__set, int __signo) {
    if (__set != NULL && is_legal_signo(__signo)) {
        __set->sig &= ~signo2mask(__signo);
		return 0;
    } else {
		return -1;
		// return -E_UNSPECIFIED;
	}
}

int sigismember(const sigset_t *__set, int __signo) {
	if (__set != NULL && is_legal_signo(__signo)) {
		return __set->sig & signo2mask(__signo) ? 1 : 0;
	} else {
		return -1;
		// return -E_UNSPECIFIED;
	}
}

int sigisemptyset(const sigset_t *__set) {
	if (__set != NULL) {
		return __set->sig == (uint32_t)0x00000000 ? 1 : 0;
	} else {
		return -1;
		// return -E_UNSPECIFIED;
	}
}

int sigandset(sigset_t *__set, const sigset_t *__left, const sigset_t *__right) {
	if (__set != NULL && __left != NULL && __right != NULL) {
		__set->sig = __left->sig & __right->sig;
		return 0;
	} else {
		return -1;
		// return -E_UNSPECIFIED;
	}
}

int sigorset(sigset_t *__set, const sigset_t *__left, const sigset_t *__right) {
	if (__set != NULL && __left != NULL && __right != NULL) {
		__set->sig = __left->sig | __right->sig;
		return 0;
	} else {
		return -1;
		// return -E_UNSPECIFIED;
	}
}
