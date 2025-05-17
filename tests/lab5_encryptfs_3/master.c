#include <lib.h>

int main() {
	int r;
	int key_fd;

	key_fd = open("/key2.key", O_RDONLY);
	if ((r = fskey_set(key_fd)) != 0) {
		user_panic("[MASTER] fskey_set() failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 1) {
		user_panic("[MASTER] fskey_isset() failed: %d\n", r);
	}
	if ((r = close(key_fd)) != 0) {
		user_panic("[MASTER] close() failed: %d\n", r);
	}
	debugf("\n[MASTER] fskey_set() passed\n");

	return 0;
}
