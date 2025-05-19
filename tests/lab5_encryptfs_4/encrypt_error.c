#include <lib.h>

static char *msg1 = "Let the ruling classes tremble at a Communistic "
		    "revolution.\nThe proletarians have nothing to lose but "
		    "their chains.\nThey have a world to win.\n";

void compare_msg1(char *test_name) {
	int r;
	int msg_fd;
	char buf[512];

	memset(buf, 0, sizeof(buf));
	// Read (/msg)
	if ((r = open("/msg", O_RDONLY | O_ENCRYPT)) < 0) {
		user_panic("[%s] cannot open /msg: %d", test_name, r);
	}
	msg_fd = r;
	if ((r = read(msg_fd, buf, 511)) < 0) {
		user_panic("[%s] cannot read /msg: %d", test_name, r);
	}
	for (int i = 0; i < strlen(msg1) + 1; i++) {
		if (buf[i] != msg1[i]) {
			user_panic("[%s] read /msg returned wrong data at %d: %02x != %02x",
				   test_name, i, (unsigned char)buf[i], (unsigned char)msg1[i]);
		} else {
			debugf("%c", buf[i]);
		}
	}
	if ((r = close(msg_fd)) < 0) {
		user_panic("[%s] cannot close /msg: %d", test_name, r);
	}
}

int main() {
	int r;
	int key_fd, msg_fd;

	// Invalid fd
	if ((r = fskey_set(-1)) != -E_INVAL) {
		user_panic("[INVALID_FD] fskey_set() failed: %d\n", r);
		return 1;
	}
	debugf("[INVALID_FD] fskey_set() passed\n");

	// Invalid open mode - O_WRONLY
	if ((r = open("/key2.key", O_WRONLY)) < 0) {
		user_panic("[O_WRONLY] open() failed: %d\n", r);
	}
	key_fd = r;
	if ((r = fskey_set(key_fd)) != -E_INVAL) {
		user_panic("[O_WRONLY] fskey_set() failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 0) {
		user_panic("[O_WRONLY] fskey_isset() failed: %d\n", r);
	}
	if ((r = close(key_fd)) < 0) {
		user_panic("[O_WRONLY] close() failed: %d\n", r);
	}
	debugf("[O_WRONLY] fskey_set() passed\n");

	// Invalid open mode - O_ENCRYPT
	if ((r = open("/key1.key", O_RDONLY)) < 0) {
		user_panic("[O_ENCRYPT] open(key1.key) failed: %d\n", r);
	}
	key_fd = r;
	if ((r = fskey_set(key_fd)) != 0) {
		user_panic("[O_ENCRYPT] fskey_set(key1.key) failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 1) {
		user_panic("[O_ENCRYPT] fskey_isset(key1.key) failed: %d\n", r);
	}
	if ((r = close(key_fd)) < 0) {
		user_panic("[O_ENCRYPT] close(key1.key) failed: %d\n", r);
	}
	compare_msg1("O_ENCRYPT");
	if ((r = open("/empty.key", O_RDONLY | O_ENCRYPT)) < 0) {
		user_panic("[O_ENCRYPT] open(empty.key) failed: %d\n", r);
	}
	key_fd = r;
	if ((r = fskey_set(key_fd)) != -E_INVAL) {
		user_panic("[O_ENCRYPT] fskey_set(empty.key) failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 1) {
		user_panic("[O_ENCRYPT] fskey_isset(empty.key) failed: %d\n", r);
	}
	if ((r = close(key_fd)) != 0) {
		user_panic("[O_ENCRYPT] close(empty.key) failed: %d\n", r);
	}
	compare_msg1("O_ENCRYPT");
	debugf("[O_ENCRYPT] fskey_set() passed\n");

	// Set key twice without unset key
	// Use invalid_magic key to check if the -E_BAD_KEY is prior to -E_INVALID_KEY_FILE
	if ((r = open("/invalid_magic.key", O_RDONLY)) < 0) {
		user_panic("[SET_TWICE] open() failed: %d\n", r);
	}
	key_fd = r;
	if ((r = fskey_set(key_fd)) != -E_BAD_KEY) {
		user_panic("[SET_TWICE] fskey_set() failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 1) {
		user_panic("[SET_TWICE] fskey_isset() failed: %d\n", r);
	}
	if ((r = close(key_fd)) < 0) {
		user_panic("[SET_TWICE] close() failed: %d\n", r);
	}
	compare_msg1("SET_TWICE");
	debugf("[SET_TWICE] fskey_set() passed\n");

	// Unset key twice without set key
	if ((r = fskey_unset()) != 0) {
		user_panic("[UNSET_TWICE] fskey_unset()_1 failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 0) {
		user_panic("[UNSET_TWICE] fskey_isset()_1 failed: %d\n", r);
	}
	if ((r = fskey_unset()) != -E_BAD_KEY) {
		user_panic("[UNSET_TWICE] fskey_unset()_2 failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 0) {
		user_panic("[UNSET_TWICE] fskey_isset()_2 failed: %d\n", r);
	}
	debugf("[UNSET_TWICE] fskey_unset() passed\n");

	// Open file with key unset
	if ((r = open("/msg", O_RDONLY | O_ENCRYPT)) != -E_BAD_KEY) {
		user_panic("[NOKEY_OPEN] open()_1 failed: %d\n", r);
	}
	if ((r = open("/empty.key", O_RDONLY)) < 0) {
		user_panic("[NOKEY_OPEN] open()_2 failed: %d\n", r);
	}
	key_fd = r;
	if ((r = close(key_fd)) < 0) {
		user_panic("[NOKEY_OPEN] close() failed: %d\n", r);
	}
	debugf("[NOKEY_OPEN] open() passed\n");

	// Close file with key unset
	if ((r = open("/key1.key", O_RDONLY)) < 0) {
		user_panic("[NOKEY_CLOSE] open(key1.key) failed: %d\n", r);
	}
	key_fd = r;
	if ((r = fskey_set(key_fd)) != 0) {
		user_panic("[NOKEY_CLOSE] fskey_set() failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 1) {
		user_panic("[NOKEY_CLOSE] fskey_isset()_1 failed: %d\n", r);
	}
	if ((r = open("/msg", O_RDONLY | O_ENCRYPT)) < 0) {
		user_panic("[NOKEY_CLOSE] open(msg) failed: %d\n", r);
	}
	msg_fd = r;
	if ((r = fskey_unset()) != 0) {
		user_panic("[NOKEY_CLOSE] fskey_unset() failed: %d\n", r);
	}
	if ((r = fskey_isset()) != 0) {
		user_panic("[NOKEY_CLOSE] fskey_isset()_2 failed: %d\n", r);
	}
	if ((r = close(key_fd)) < 0) {
		user_panic("[NOKEY_CLOSE] close(key1.key) failed: %d\n", r);
	}
	if ((r = close(msg_fd)) != -E_BAD_KEY) {
		user_panic("[NOKEY_CLOSE] close(msg) failed: %d\n", r);
	}
	debugf("[NOKEY_CLOSE] close() passed\n");

	// Invalid key - empty key
	if ((r = open("/empty.key", O_RDONLY)) < 0) {
		user_panic("[KEY_EMPTY] open() failed: %d\n", r);
	}
	key_fd = r;
	if ((r = fskey_set(key_fd)) != -E_INVALID_KEY_FILE) {
		user_panic("[KEY_EMPTY] fskey_set() failed: %d\n", r);
	}
	if ((r = close(key_fd)) < 0) {
		user_panic("[KEY_EMPTY] close() failed: %d\n", r);
	}
	debugf("[KEY_EMPTY] fskey_set() passed\n");

	// Invalid key - too_short key
	if ((r = open("/too_short.key", O_RDONLY)) < 0) {
		user_panic("[KEY_SHORT] open() failed: %d\n", r);
	}
	key_fd = r;
	if ((r = fskey_set(key_fd)) != -E_INVALID_KEY_FILE) {
		user_panic("[KEY_SHORT] fskey_set() failed: %d\n", r);
	}
	if ((r = close(key_fd)) < 0) {
		user_panic("[KEY_SHORT] close() failed: %d\n", r);
	}
	debugf("[KEY_SHORT] fskey_set() passed\n");

	// Invalid key - invalid_magic key
	if ((r = open("/invalid_magic.key", O_RDONLY)) < 0) {
		user_panic("[KEY_MAGIC] open() failed: %d\n", r);
	}
	key_fd = r;
	if ((r = fskey_set(key_fd)) != -E_INVALID_KEY_FILE) {
		user_panic("[KEY_MAGIC] fskey_set() failed: %d\n", r);
	}
	if ((r = close(key_fd)) < 0) {
		user_panic("[KEY_MAGIC] close() failed: %d\n", r);
	}
	debugf("[KEY_MAGIC] fskey_set() passed\n");

	return 0;
}
