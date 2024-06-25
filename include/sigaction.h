#ifndef SIGACTION_H
#define SIGACTION_H
#include <types.h>

// challenge-sigaction
// 使用32位表示MOS所需要处理的[1,32]信号掩码
// 对应位为1表示阻塞，为0表示未被阻塞
// 注意第 i 位表示的信号为 i+1
typedef struct sigset_t {
    uint32_t sig;
} sigset_t;

// 用于设置所需要处理的信号集及其对应的处理函数
struct sigaction {
    // 信号的处理函数，当未屏蔽信号到达并且“处理时机”合适时，进程就会执行该函数
    // 注意：SIGKILL信号不能设置其他处理函数
    void     (*sa_handler)(int);
    // 表示该sigaction结构体对应的信号被处理时，需要被屏蔽的信号集
    sigset_t   sa_mask;
};

// 最小的合法信号
#define MINSIGNO 1
// 最大的合法信号
#define MAXSIGNO 32

// 中断信号	停止进程
#define SIGINT 2
// 非法指令	停止进程
#define SIGILL 4
// 停止进程信号	强制停止该进程，不可被阻塞
// 任何对其处理函数进行修改都是无效的，其处理动作只会是结束进程
#define SIGKILL 9
// 访问地址错误，当访问[0, 0x003f_e000)(即UTEMP以下）内地址时	停止进程
// 对于SIGSEGV信号，在原MOS中会进行panic
// 需要取消该设置，改为发送SIGSEGV信号
#define SIGSEGV 11
// 子进程终止信号	忽略
#define SIGCHLD 17
// 系统调用号未定义	忽略
#define SIGSYS 31
// 其余[1,32]内的signum编号的默认处理动作为忽略

/*
注意：
1. 普通信号编号更小的信号拥有更高的优先级
   当有两个不同的信号需要处理时，需要先处理编号较小的信号
2. 同一普通信号在进程中最多只存在一个
   有多个同样编号信号发送至某一进程，只取其中一个
3. 当一个信号被执行时，需要添加同类型信号的屏蔽
   在结束执行后再恢复成原来的屏蔽集状态    
4. 信号的执行是否能被打断只与此时信号的屏蔽集有关
   优先级只会影响当有多种信号时，所需要执行信号的选择
*/

// 是否是合法的信号
#define is_legal_signo(__signo) (MINSIGNO <= (__signo) && (__signo) <= MAXSIGNO)
// 生成信号对应的掩码
#define signo2mask(__signo) ((uint32_t)(1 << ((__signo) - 1)))

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

// 内核中发送信号到某个进程
int sigaction_kill(u_int envid, int sig);

#endif
