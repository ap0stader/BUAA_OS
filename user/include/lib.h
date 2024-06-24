#ifndef LIB_H
#define LIB_H
#include <args.h>
#include <env.h>
#include <fd.h>
#include <mmu.h>
#include <pmap.h>
#include <syscall.h>
#include <trap.h>

#define vpt ((const volatile Pte *)UVPT)
#define vpd ((const volatile Pde *)(UVPT + (PDX(UVPT) << PGSHIFT)))
#define envs ((const volatile struct Env *)UENVS)
#define pages ((const volatile struct Page *)UPAGES)

// libos
void exit(void) __attribute__((noreturn));

extern const volatile struct Env *env;

#define USED(x) (void)(x)

// debugf
void debugf(const char *fmt, ...);

void _user_panic(const char *, int, const char *, ...) __attribute__((noreturn));
void _user_halt(const char *, int, const char *, ...) __attribute__((noreturn));

#define user_panic(...) _user_panic(__FILE__, __LINE__, __VA_ARGS__)
#define user_halt(...) _user_halt(__FILE__, __LINE__, __VA_ARGS__)

#undef panic_on
#define panic_on(expr)                                                                             \
	do {                                                                                       \
		int r = (expr);                                                                    \
		if (r != 0) {                                                                      \
			user_panic("'" #expr "' returned %d", r);                                  \
		}                                                                                  \
	} while (0)

/// fork, spawn
int spawn(char *prog, char **argv);
int spawnl(char *prot, char *args, ...);
int fork(void);

/// syscalls
extern int msyscall(int, ...);

void syscall_putchar(int ch);
int syscall_print_cons(const void *str, u_int num);
u_int syscall_getenvid(void);
void syscall_yield(void);
int syscall_env_destroy(u_int envid);
int syscall_set_tlb_mod_entry(u_int envid, void (*func)(struct Trapframe *));
int syscall_mem_alloc(u_int envid, void *va, u_int perm);
int syscall_mem_map(u_int srcid, void *srcva, u_int dstid, void *dstva, u_int perm);
int syscall_mem_unmap(u_int envid, void *va);

__attribute__((always_inline)) inline static int syscall_exofork(void) {
	return msyscall(SYS_exofork, 0, 0, 0, 0, 0);
}

int syscall_set_env_status(u_int envid, u_int status);
int syscall_set_trapframe(u_int envid, struct Trapframe *tf);
void syscall_panic(const char *msg) __attribute__((noreturn));
int syscall_ipc_try_send(u_int envid, u_int value, const void *srcva, u_int perm);
int syscall_ipc_recv(void *dstva);
int syscall_cgetc(void);
int syscall_write_dev(void *va, u_int dev, u_int len);
int syscall_read_dev(void *va, u_int dev, u_int len);

// ipc.c
void ipc_send(u_int whom, u_int val, const void *srcva, u_int perm);
u_int ipc_recv(u_int *whom, void *dstva, u_int *perm);

// wait.c
void wait(u_int envid);

// console.c
int opencons(void);
int iscons(int fdnum);

// pipe.c
int pipe(int pfd[2]);
int pipe_is_closed(int fdnum);

// pageref.c
int pageref(void *);

// fprintf.c
int fprintf(int fd, const char *fmt, ...);
int printf(const char *fmt, ...);

// fsipc.c
int fsipc_open(const char *, u_int, struct Fd *);
int fsipc_map(u_int, u_int, void *);
int fsipc_set_size(u_int, u_int);
int fsipc_close(u_int);
int fsipc_dirty(u_int, u_int);
int fsipc_remove(const char *);
int fsipc_sync(void);
int fsipc_incref(u_int);

// fd.c
int close(int fd);
int read(int fd, void *buf, u_int nbytes);
int write(int fd, const void *buf, u_int nbytes);
int seek(int fd, u_int offset);
void close_all(void);
int readn(int fd, void *buf, u_int nbytes);
int dup(int oldfd, int newfd);
int fstat(int fdnum, struct Stat *stat);
int stat(const char *path, struct Stat *);

// file.c
int open(const char *path, int mode);
int read_map(int fd, u_int offset, void **blk);
int remove(const char *path);
int ftruncate(int fd, u_int size);
int sync(void);

#define user_assert(x)                                                                             \
	do {                                                                                       \
		if (!(x))                                                                          \
			user_panic("assertion failed: %s", #x);                                    \
	} while (0)

// File open modes
#define O_RDONLY 0x0000	 /* open for reading only */
#define O_WRONLY 0x0001	 /* open for writing only */
#define O_RDWR 0x0002	 /* open for reading and writing */
#define O_ACCMODE 0x0003 /* mask for above modes */
#define O_CREAT 0x0100	 /* create if nonexistent */
#define O_TRUNC 0x0200	 /* truncate to zero length */

// Unimplemented open modes
#define O_EXCL 0x0400  /* error if already exists */
#define O_MKDIR 0x0800 /* create directory, not regular file */

// challenge-sigaction
// 使用32位表示MOS所需要处理的[1,32]信号掩码
// 对应位为1表示阻塞，为0表示未被阻塞
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

// !!需要实现的函数的某些函数的具体功能可能需要由系统调用实现!!
// --- 信号注册函数 ---
// - signum:需要设置的信号编号
//          当收到编号大于32的信号时直接返回异常码-1(-E_UNSPECIFIED)
// - newact:如果newact不为NULL，为signum设置sigaction结构体
// - oldact:如果oldact不为NULL，将该信号之前的sigaction结构体复制到oldact中
int sigaction(int signum, const struct sigaction *newact, struct sigaction *oldact);


// --- 信号发送函数 ---
// - envid:向envid进程发送信号
//         当envid为0时，代表向自身发送信号
//         当envid对应进程不存在，返回异常码-1(-E_UNSPECIFIED)
// - sig:要发送的信号编号
//       当sig不符合定义范围时，返回异常码-1
// 注意：一些信号通常并不通过kill函数发出，而是由内核发出，不需要考虑通过kill函数发出这些信号的情况
int kill(u_int envid, int sig);

// --- 信号集处理函数 ---
// 清空参数中的__set掩码，全清0
int sigemptyset(sigset_t *__set);

// 将参数中的__set掩码填满，全置1
int sigfillset(sigset_t *__set);

// 向__set信号集中添加一个信号__signo，如果操作成功，__set将包含该信号。置位为1
int sigaddset(sigset_t *__set, int __signo);

// 从__set信号集中删除一个信号__signo。如果操作成功，__set将不再包含该信号。置位为0
int sigdelset(sigset_t *__set, int __signo);

// 检查信号__signo是否是__set信号集的成员。如果是，返回1；如果不是，返回0
int sigismember(const sigset_t *__set, int __signo);

// 检查信号集__set是否为空。如果为空，返回1；如果不为空，返回0
int sigisemptyset(const sigset_t *__set);

// 计算两个信号集__left和__right的交集，并将结果存储在__set中
int sigandset(sigset_t *__set, const sigset_t *__left, const sigset_t *__right);

// 计算两个信号集__left和__right的并集，并将结果存储在__set中
int sigorset(sigset_t *__set, const sigset_t *__left, const sigset_t *__right);

// 根据__how的值更改【当前进程】的信号屏蔽字
// __set是要应用的新掩码
// __oset（如果非NULL）则保存旧的信号屏蔽字
// __how:SIG_BLOCK-添加__set到当前掩码
//       SIG_UNBLOCK-从当前掩码中移除__set
//       SIG_SETMASK-设置当前掩码为__set
int sigprocmask(int __how, const sigset_t * __set, sigset_t * __oset);

// 获取当前被阻塞且未处理的信号集，并将其存储在__set中
int sigpending(sigset_t *__set);

#endif
