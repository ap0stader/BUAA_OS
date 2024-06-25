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
		int r = syscall_sigaction_finish(tf);
		user_panic("syscall_sigaction_finish returned %d", r);
	} else if (signo == SIGINT || signo == SIGILL || signo == SIGSEGV) {
		// SIGINT/SIGILL/SIGSEGV默认处理是停止进程
		exit();
	} else {
		// 其他的默认处理是忽略，但是要跳过错误的指令
		tf->cp0_epc += 4;
		int r = syscall_sigaction_finish(tf);
		user_panic("syscall_sigaction_finish returned %d", r);
	}
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
// 清空参数中的__set掩码，全清0
int sigemptyset(sigset_t *__set) {
    if (__set != NULL) {
        __set->sig = (uint32_t)0x00000000;
        return 0;
    } else {
        return -1;
    }
}

// 将参数中的__set掩码填满，全置1
int sigfillset(sigset_t *__set) {
    if (__set != NULL) {
        __set->sig = (uint32_t)0xFFFFFFFF;
        return 0;
    } else {
        return -1;
    }
}

// 向__set信号集中添加一个信号__signo，如果操作成功，__set将包含该信号。置位为1
int sigaddset(sigset_t *__set, int __signo) {
    if (__set != NULL && is_legal_signo(__signo)) {
        __set->sig |= signo2mask(__signo);
        return 0;
    } else {
        return -1;
    }
}

// 从__set信号集中删除一个信号__signo。如果操作成功，__set将不再包含该信号。置位为0
int sigdelset(sigset_t *__set, int __signo) {
    if (__set != NULL && is_legal_signo(__signo)) {
        __set->sig &= ~signo2mask(__signo);
		return 0;
    } else {
		return -1;
	}
}

// 检查信号__signo是否是__set信号集的成员。如果是，返回1；如果不是，返回0
int sigismember(const sigset_t *__set, int __signo) {
	if (__set != NULL && is_legal_signo(__signo)) {
		return __set->sig & signo2mask(__signo) ? 1 : 0;
	} else {
		return -1;
	}
}

// 检查信号集__set是否为空。如果为空，返回1；如果不为空，返回0
int sigisemptyset(const sigset_t *__set) {
	if (__set != NULL) {
		return __set->sig == (uint32_t)0x00000000 ? 1 : 0;
	} else {
		return -1;
	}
}

// 计算两个信号集__left和__right的交集，并将结果存储在__set中
int sigandset(sigset_t *__set, const sigset_t *__left, const sigset_t *__right) {
	if (__set != NULL && __left != NULL && __right != NULL) {
		__set->sig = __left->sig & __right->sig;
		return 0;
	} else {
		return -1;
	}
}

// 计算两个信号集__left和__right的并集，并将结果存储在__set中
int sigorset(sigset_t *__set, const sigset_t *__left, const sigset_t *__right) {
	if (__set != NULL && __left != NULL && __right != NULL) {
		__set->sig = __left->sig | __right->sig;
		return 0;
	} else {
		return -1;
	}
}

int sigprocmask(int __how, const sigset_t *__set, sigset_t *__oset) {
	if (__set == NULL) {
		return -1;
	}
	return syscall_change_curenv_sigprocmask(__how, __set, __oset);
}

int sigpending(sigset_t *__set) {
	if (__set == NULL) {
		return -1;
	}
	return syscall_get_curenv_sigpending(__set);
}
