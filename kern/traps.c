#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_ri(void);
extern void handle_reserved(void);

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
#endif
    // Lab3 Extra
    [10] = handle_ri,
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}

void do_ri(struct Trapframe *tf) {
	// 取出异常的指令
	u_long instruction = *((u_long *)tf->cp0_epc);

	u_long instruction_mask = 0xFC0007FF;	
	u_long rs_mask = 0x3E00000;
	u_long rt_mask = 0x1F0000;
	u_long rd_mask = 0xF800;

	u_long instruction_judge = instruction & instruction_mask;
	u_long rs = (instruction & rs_mask) >> 21;
	u_long rt = (instruction & rt_mask) >> 16;
	u_long rd = (instruction & rd_mask) >> 11;
	
	u_long pmaxub = 0x3F;
	u_long cas = 0x3E;

	if (instruction_judge == pmaxub) {
		tf->regs[rd] = (u_long) 0;
		for(int i = 0; i < 32; i += 8) {
			u_int rs_i_byte = tf->regs[rs] & (0xff << i);
			u_int rt_i_byte = tf->regs[rt] & (0xff << i);
			if (rs_i_byte < rt_i_byte) {
				tf->regs[rd] = tf->regs[rd] | rt_i_byte;
			} else {
				tf->regs[rd] = tf->regs[rd] | rs_i_byte;
			}
		}
	} else if (instruction_judge == cas) {
		u_long *rs_value = (u_long*)tf->regs[rs];
		// tmp = *rs;
		u_long rs_mem = *rs_value;
		u_long temp = rs_mem;
		// if (*rs == rt) {
		if (rs_mem == tf->regs[rt]) {
			// *rs = rd;
			*rs_value = tf->regs[rd];
		}
		// rd = tmp;
		tf->regs[rd] = temp;
	}

	// 无论触发异常的是 自定义指令 还是非法指令，你都需要在异常处理结束前让 EPC + 4 ，以免再次触发相同异常。
	tf->cp0_epc = tf->cp0_epc + 4;
}
