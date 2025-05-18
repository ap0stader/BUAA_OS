#include <lib.h>

int main() {
	int r;
	int key_fd;

	if ((r = open("/key2.key", O_RDONLY)) < 0) {
		user_panic("[MASTER] open(key2.key) failed: %d\n", r);
	}
	key_fd = r;
	if ((r = fskey_set(key_fd)) != 0) {
		user_panic("[MASTER] fskey_set() failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 1) {
		user_panic("[MASTER] fskey_isset() failed: %d\n", r);
	}
	if ((r = close(key_fd)) < 0) {
		user_panic("[MASTER] close() failed: %d\n", r);
	}
	debugf("\n[MASTER] fskey_set() passed\n");

	return 0;
}
