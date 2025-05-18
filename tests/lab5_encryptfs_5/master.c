#include <lib.h>

int main() {
	int r;
	int key_fd;

	// Invalid key
	if ((r = open("/invalid_magic.key", O_RDONLY)) < 0) {
		user_panic("[MASTER] open(invalid_magic.key) failed: %d\n", r);
	}
	key_fd = r;
	if ((r = fskey_set(key_fd)) != -E_INVALID_KEY_FILE) {
		user_panic("[MASTER] fskey_set(invalid_magic.key) failed: %d\n", r);
	}
	if ((r = close(key_fd)) < 0) {
		user_panic("[MASTER] close(invalid_magic.key) failed: %d\n", r);
	}
	debugf("[MASTER] fskey_set(invalid_magic.key) passed\n");

	// Set key as key1.key (Valid key)
	if ((r = open("/key1.key", O_RDONLY)) < 0) {
		user_panic("[MASTER] open(key1.key) failed: %d\n", r);
	}
	key_fd = r;
	if ((r = fskey_set(key_fd)) != 0) {
		user_panic("[MASTER] fskey_set(key1.key) failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 1) {
		user_panic("[MASTER] fskey_isset(key1.key) failed: %d\n", r);
	}
	if ((r = close(key_fd)) < 0) {
		user_panic("[MASTER] close(key1.key) failed: %d\n", r);
	}
	debugf("\n[MASTER] fskey_set(key1.key) passed\n");

	return 0;
}
