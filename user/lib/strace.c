#include <lib.h>

void strace_barrier(u_int env_id) {
	int straced_bak = straced;
	straced = 0;
	while (envs[ENVX(env_id)].env_status == ENV_RUNNABLE) {
		syscall_yield();
	}
	straced = straced_bak;
}

void strace_send(int sysno) {
	if (!((SYS_putchar <= sysno && sysno <= SYS_set_tlb_mod_entry) ||
	      (SYS_exofork <= sysno && sysno <= SYS_panic)) ||
	    sysno == SYS_set_trapframe) {
		return;
	}

	// Your code here. (1/2)
	// 并且当前进程的 straced 非 0 时
	if (straced != 0) {
		// 我们需要在进行这些操作前设置 straced = 0 来避免这部分系统调用重入到 strace_send 中
		int straced_bak = straced;
		straced = 0;
		// 通过 IPC 向父进程发送当前的 sysno
		ipc_send(env->env_parent_id, sysno, 0, 0);
		// 同时通过 syscall_set_env_status 设置自身状态为 ENV_NOT_RUNNABLE
		syscall_set_env_status(0, ENV_NOT_RUNNABLE);
		// 并在最后恢复 straced 的值
		straced = straced_bak;
	}
}

void strace_recv() {
	// Your code here. (2/2)
	int sysno;
	u_int env_child_id;
	while(1) {
		// 通过循环不断调用 ipc_recv 接收子进程发来的 sysno
		sysno = ipc_recv(&env_child_id, NULL, NULL);
		strace_barrier(env_child_id);
		// 调用 recv_sysno(child_env_id, sysno) 标记收到了来自 env_id 为 child_env_id 的子进程发来的 sysno
		recv_sysno(env_child_id, sysno);
		// 将接收到的子进程的状态重新设为 ENV_RUNNABLE 恢复其运行。
		syscall_set_env_status(env_child_id, ENV_RUNNABLE);
		// 接收到的 sysno 为 SYS_env_destroy 时，说明被追踪子进程即将结束，因此完成上述工作后即可退出循环，结束 strace_recv 的运行
		if (sysno == SYS_env_destroy) {
			break;
		}
	}
}
