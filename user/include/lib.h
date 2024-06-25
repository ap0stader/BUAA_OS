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
// !!需要实现的函数的某些函数的具体功能可能需要由系统调用实现!!
// --- 信号注册函数 ---
// - signo:需要设置的信号编号
//          当收到编号大于32的信号时直接返回异常码-1(-E_UNSPECIFIED)
// - newact:signo
// - oldact:如果oldact不为NULL，将该信号之前的sigaction结构体复制到oldact中
int sigaction(int signo, const struct sigaction *newact, struct sigaction *oldact);

// --- 信号发送函数 ---
// - envid:向envid进程发送信号
//         当envid为0时，代表向自身发送信号
//         当envid对应进程不存在，返回异常码-1(-E_UNSPECIFIED)
// - sig:要发送的信号编号
//       当sig不符合定义范围时，返回异常码-1
// 注意：一些信号通常并不通过kill函数发出，而是由内核发出，不需要考虑通过kill函数发出这些信号的情况
int kill(u_int envid, int signo);

// --- 信号集处理函数 ---
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
// __set是要应用的新掩码（如果为NULL，则不做任何修改）
// __oset（如果非NULL）则保存旧的信号屏蔽字
// __how:SIG_BLOCK-添加__set到当前掩码
//       SIG_UNBLOCK-从当前掩码中移除__set
//       SIG_SETMASK-设置当前掩码为__set
int sigprocmask(int __how, const sigset_t * __set, sigset_t * __oset);

// 获取当前被阻塞且未处理的信号集，并将其存储在__set中
int sigpending(sigset_t *__set);

// --- syscalls ---
int syscall_get_env_sigaction(int signo, struct sigaction *oldact);
int syscall_set_env_sigaction(int signo, const struct sigaction *newact);
int syscall_sigaction_kill(u_int envid, int signo);
int syscall_sigaction_finish(struct Trapframe *tf);
int syscall_change_curenv_sigprocmask(int __how, const sigset_t *__set, sigset_t *__oset);
int syscall_get_curenv_sigpending(sigset_t *__set);
int syscall_set_curenv_sigaction_entry(void (*func)(struct Trapframe *, int, void (*)(int)));

#endif
