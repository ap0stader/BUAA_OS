#include <lib.h>

static char *msg1 = "Let the ruling classes tremble at a Communistic "
		    "revolution.\nThe proletarians have nothing to lose but "
		    "their chains.\nThey have a world to win.\n";

void compare_msg1() {
	int msg_fd;
	char buf[512];

	msg_fd = open("/msg", O_RDONLY | O_ENCRYPT);
	read(msg_fd, buf, 511);
	for (int i = 0; i < strlen(msg1) + 1; i++) {
		if (buf[i] != msg1[i]) {
			user_panic("read /msg returned wrong data at %d: %02x != %02x", i,
				   (unsigned char)buf[i], (unsigned char)msg1[i]);
		} else {
			debugf("%c", buf[i]);
		}
	}
	close(msg_fd);
}

int main() {
	int r;
	int key_fd, msg_fd;

	// Invalid fd
	if ((r = fskey_set(-1)) != -E_INVAL) {
		user_panic("[-1] fskey_set() failed: %d\n", r);
		return 1;
	}
	debugf("[-1] fskey_set() passed\n");

	// Invalid open mode - O_WRONLY
	key_fd = open("/key2.key", O_WRONLY);
	if ((r = fskey_set(key_fd)) != -E_INVAL) {
		user_panic("[O_WRONLY] fskey_set() failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 0) {
		user_panic("[O_WRONLY] fskey_isset() failed: %d\n", r);
	}
	close(key_fd);
	debugf("[O_WRONLY] fskey_set() passed\n");

	// Invalid open mode - O_ENCRYPT
	key_fd = open("/key1.key", O_RDONLY);
	fskey_set(key_fd);
	if ((r = fskey_isset()) != 1) {
		user_panic("[O_ENCRYPT] fskey_isset(key1.key) failed: %d\n", r);
	}
	close(key_fd);
	compare_msg1();
	if ((r = open("/empty_key.key", O_RDONLY | O_ENCRYPT)) < 0) {
		user_panic("[O_ENCRYPT] open(empty_key.key) failed: %d\n", r);
	}
	key_fd = r;
	if ((r = fskey_set(key_fd)) != -E_INVAL) {
		user_panic("[O_ENCRYPT] fskey_set() failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 1) {
		user_panic("[O_ENCRYPT] fskey_isset(empty_key.key) failed: %d\n", r);
	}
	close(key_fd);
	compare_msg1();
	debugf("[O_ENCRYPT] fskey_set() passed\n");

	// Reset key without unset key
	key_fd = open("/key2.key", O_RDONLY);
	if ((r = fskey_set(key_fd)) != -E_BAD_KEY) {
		user_panic("[RESET] fskey_set() failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 1) {
		user_panic("[RESET] fskey_isset() failed: %d\n", r);
	}
	close(key_fd);
	compare_msg1();
	debugf("[RESET] fskey_set() passed\n");

	// Unset key with key unset
	if ((r = fskey_unset()) != 0) {
		user_panic("[REUNSET] fskey_unset()_1 failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 0) {
		user_panic("[REUNSET] fskey_isset()_1 failed: %d\n", r);
	}
	if ((r = fskey_unset()) != -E_BAD_KEY) {
		user_panic("[REUNSET] fskey_unset()_2 failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 0) {
		user_panic("[REUNSET] fskey_isset()_2 failed: %d\n", r);
	}
	debugf("[REUNSET] fskey_unset() passed\n");

	// Open file with key unset
	if ((r = open("/msg", O_RDONLY | O_ENCRYPT)) != -E_BAD_KEY) {
		user_panic("[NOKEY_OPEN] open()_1 failed: %d\n", r);
	}
	if ((r = open("/empty_key.key", O_RDONLY)) < 0) {
		user_panic("[NOKEY_OPEN] open()_2 failed: %d\n", r);
	}
	key_fd = r;
	if ((r = close(key_fd)) != 0) {
		user_panic("[NOKEY_OPEN] close() failed: %d\n", r);
	}
	debugf("[NOKEY_OPEN] open() passed\n");

	// Close file with key unset
	key_fd = open("/key1.key", O_RDONLY);
	if ((r = fskey_set(key_fd)) != 0) {
		user_panic("[NOKEY_CLOSE] fskey_set() failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 1) {
		user_panic("[NOKEY_CLOSE] fskey_isset()_1 failed: %d\n", r);
	}
	msg_fd = open("/msg", O_RDONLY | O_ENCRYPT);
	if ((r = fskey_unset()) != 0) {
		user_panic("[NOKEY_CLOSE] fskey_unset() failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 0) {
		user_panic("[NOKEY_CLOSE] fskey_isset()_2 failed: %d\n", r);
	}
	if ((r = close(key_fd)) != 0) {
		user_panic("[NOKEY_CLOSE] close()_1 failed: %d\n", r);
	}
	if ((r = close(msg_fd)) != -E_BAD_KEY) {
		user_panic("[NOKEY_CLOSE] close()_2 failed: %d\n", r);
	}
	debugf("[NOKEY_CLOSE] close() passed\n");

	// Invalid key - empty key
	key_fd = open("/empty_key.key", O_RDONLY);
	if ((r = fskey_set(key_fd)) != -E_INVALID_KEY_FILE) {
		user_panic("[KEY_EMPTY] fskey_set() failed: %d\n", r);
	}
	close(key_fd);
	debugf("[KEY_EMPTY] fskey_set() passed\n");

	// Invalid key - too_short key
	key_fd = open("/too_short.key", O_RDONLY);
	if ((r = fskey_set(key_fd)) != -E_INVALID_KEY_FILE) {
		user_panic("[KEY_SHORT] fskey_set() failed: %d\n", r);
	}
	close(key_fd);
	debugf("[KEY_SHORT] fskey_set() passed\n");

	// Invalid key - invalid_magic key
	key_fd = open("/invalid_magic.key", O_RDONLY);
	if ((r = fskey_set(key_fd)) != -E_INVALID_KEY_FILE) {
		user_panic("[KEY_MAGIC] fskey_set() failed: %d\n", r);
	}
	close(key_fd);

	return 0;
}
