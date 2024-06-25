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
// 其余[1,32]内的signo默认处理动作为忽略

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

// 修改进程的屏蔽的方法
enum {
    SIG_BLOCK, //添加__set到当前掩码
    SIG_UNBLOCK, //从当前掩码中移除__set
    SIG_SETMASK, //设置当前掩码为__set
};

// 内核中调整当前进程的procmask
int change_curenv_sigprocmask(int __how, const sigset_t *__set, sigset_t *__oset);

// 内核中发送信号到某个进程
int sigaction_kill(u_int envid, int signo);

// 用户态信号处理程序入口
void __attribute__((noreturn)) sigaction_entry(struct Trapframe *tf, int signo, void (*sa_handler)(int));

#endif
