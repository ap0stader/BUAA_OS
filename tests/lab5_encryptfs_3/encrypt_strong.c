#include <lib.h>

static char *survive_msg = "The Communist Manifesto";

static char *msg1 = "A spectre is haunting Europe - the spectre of Communism.\nAll the Powers of "
		    "old Europe have entered into a holy alliance to exorcize this spectre:\nPope "
		    "and Czar, Metternich and Guizot, French Radicals and German police spies.";

static char *msg2 = "Workers of the world, unite!";

static char msg2_encrypted_key1[] = {0x91, 0xf9, 0x02, 0x1b, 0x11, 0xbf, 0xca, 0x8c, 0x8e, 0x64,
				     0xc6, 0xdb, 0x0c, 0xb8, 0x24, 0x21, 0xfb, 0x84, 0x20, 0xd2,
				     0xc4, 0x24, 0xd0, 0x60, 0x15, 0x22, 0x00, 0xd0, 0xe8};

static char msg2_encrypted_key2[] = {0xd4, 0x35, 0x00, 0x12, 0x60, 0x2a, 0xfe, 0xc5, 0x1c, 0x1d,
				     0xe2, 0x21, 0x51, 0x2d, 0x42, 0x93, 0x58, 0x72, 0xea, 0x58,
				     0xf1, 0x09, 0xce, 0xdf, 0xce, 0xee, 0x79, 0xa3, 0x0e};

static char msg2_encrypted_key3[] = {0x45, 0x25, 0x29, 0xdc, 0x7d, 0x29, 0x5d, 0x9d, 0x42, 0xdb,
				     0x32, 0xf0, 0xf1, 0x15, 0xc1, 0xf7, 0x1c, 0xca, 0x8d, 0x78,
				     0x90, 0x6c, 0xef, 0x2e, 0xd0, 0x26, 0x8e, 0xbb, 0xcd};

void set_key(char *key_path) {
	int r;
	int key_fd;

	// Open key file
	if ((r = open(key_path, O_RDWR)) < 0) {
		user_panic("[key=%s] cannot open %s: %d", key_path, key_path, r);
	}
	key_fd = r;
	// Set the key
	if ((r = fskey_set(key_fd)) != 0) {
		user_panic("[key=%s] fskey_set() failed: %d\n", key_path, r);
	}
	if ((r = fskey_isset()) != 1) {
		user_panic("[key=%s] fskey_isset() failed: %d\n", key_path, r);
	}
	if ((r = close(key_fd)) < 0) {
		user_panic("[key=%s] close() failed: %d\n", key_path, r);
	}
	debugf("\n[key=%s] fskey_set() passed\n", key_path);
}

void unset_key(char *key_path) {
	int r;

	// Unset the key
	if ((r = fskey_unset()) != 0) {
		user_panic("[key=%s] fskey_unset() failed: %d\n", key_path, r);
	}
	if ((r = fskey_isset()) != 0) {
		user_panic("[key=%s] fskey_isset() failed: %d\n", key_path, r);
	}
	debugf("\n[key=%s] fskey_unset() passed\n", key_path);
}

void compare_msg1(char *file_path, char *key_path) {
	int r;
	int msg_fd;
	char buf[512];

	memset(buf, 0, sizeof(buf));
	// Read
	if ((r = open(file_path, O_RDONLY | O_ENCRYPT)) < 0) {
		user_panic("[key=%s] cannot open %s: %d", key_path, file_path, r);
	}
	msg_fd = r;
	if ((r = read(msg_fd, buf, 511)) < 0) {
		user_panic("[key=%s] cannot read %s: %d", key_path, file_path, r);
	}
	for (int i = 0; i < strlen(msg1) + 1; i++) {
		if (buf[i] != msg1[i]) {
			user_panic("[key=%s] read %s returned wrong data at %d: %02x != %02x",
				   key_path, file_path, i, (unsigned char)buf[i],
				   (unsigned char)msg1[i]);
		} else {
			debugf("%c", buf[i]);
		}
	}
	if ((r = close(msg_fd)) < 0) {
		user_panic("[key=%s] cannot close %s: %d", key_path, file_path, r);
	}
	debugf("\n[key=%s] read is good\n", key_path);
}

void compare_msg2(char *file_path, char *encrypt_array, char *key_path) {
	int r;
	int msg_fd;
	char buf[512];
	memset(buf, 0, sizeof(buf));

	// Write
	if ((r = open(file_path, O_RDWR | O_ENCRYPT | O_CREAT)) < 0) {
		user_panic("[key=%s] cannot create and open %s: %d", key_path, file_path, r);
	}
	msg_fd = r;
	if ((r = write(msg_fd, msg2, strlen(msg2) + 1)) < 0) {
		user_panic("[key=%s] cannot write %s: %d", key_path, file_path, r);
	}
	if ((r = close(msg_fd)) < 0) {
		user_panic("[key=%s] cannot close %s: %d", key_path, file_path, r);
	}
	// Read
	if ((r = open(file_path, O_RDONLY)) < 0) {
		user_panic("[key=%s] cannot open %s: %d", key_path, file_path, r);
	}
	msg_fd = r;
	if ((r = read(msg_fd, buf, 511)) < 0) {
		user_panic("[key=%s] cannot read %s: %d", key_path, file_path, r);
	}
	for (int i = 0; i < strlen(msg2) + 1; i++) {
		if (buf[i] != encrypt_array[i]) {
			user_panic("[key=%s] read %s returned wrong data at %d: %02x != %02x",
				   key_path, file_path, i, (unsigned char)buf[i],
				   (unsigned char)encrypt_array[i]);
		}
	}
	for (int i = 0; i < strlen(msg2) + 1; i++) {
		debugf("%02x ", (unsigned char)buf[i]);
	}
	debugf("\n");
	if ((r = close(msg_fd)) < 0) {
		user_panic("[key=%s] cannot close %s: %d", key_path, file_path, r);
	}
	debugf("\n[key=%s] write is good\n", key_path);
}

void compare_survive_msg(char *buf) {
	for (int i = 0; i < strlen(survive_msg) + 1; i++) {
		if (buf[i] != survive_msg[i]) {
			user_panic(
			    "[SURVIVE] read /survive returned wrong data at %d: %02x != %02x", i,
			    (unsigned char)buf[i], (unsigned char)survive_msg[i]);
		} else {
			debugf("%c", buf[i]);
		}
	}
	debugf("\n[SURVIVE] read is good\n");
}

int main() {
	int r;
	int survive_fd;
	char buf[512];

	memset(buf, 0, sizeof(buf));
	// Key 1 ===>>>
	set_key("/key1.key");
	// Key 1 -- Survive
	if ((r = open("/survive", O_RDWR | O_ENCRYPT | O_CREAT)) < 0) {
		user_panic("[SURVIVE] cannot open /survive_1: %d", r);
	}
	survive_fd = r;
	if ((r = write(survive_fd, survive_msg, strlen(survive_msg) + 1)) < 0) {
		user_panic("[SURVIVE] cannot write /survive_1: %d", r);
	}
	// Key 1 -- Read
	compare_msg1("/msg1_key1", "/key1.key");
	// Key 1 -- Write
	compare_msg2("/msg2_key1", msg2_encrypted_key1, "/key1.key");
	// <<<=== key 1
	unset_key("/key1.key");

	// Key 2 ===>>>
	set_key("/key2.key");
	// Key 2 -- Survive
	if ((r = write(survive_fd, survive_msg, strlen(survive_msg) + 1)) < 0) {
		user_panic("[SURVIVE] cannot write /survive_2: %d", r);
	}
	// key 2 -- Read
	compare_msg1("/msg1_key2", "/key2.key");
	// Key 2 -- Write
	compare_msg2("/msg2_key2", msg2_encrypted_key2, "/key2.key");
	// <<<=== key 2
	unset_key("/key2.key");

	// Key 3 ===>>>
	set_key("/key3.key");
	// Key 3 -- Survive
	if ((r = write(survive_fd, survive_msg, strlen(survive_msg) + 1)) < 0) {
		user_panic("[SURVIVE] cannot write /survive_3: %d", r);
	}
	if ((r = close(survive_fd)) < 0) {
		user_panic("[SURVIVE] cannot close /survive_1: %d", r);
	}
	// Check the survive file
	if ((r = open("/survive", O_RDONLY | O_ENCRYPT)) < 0) {
		user_panic("[SURVIVE] cannot open /survive_2: %d", r);
	}
	survive_fd = r;
	if ((r = read(survive_fd, buf, 511)) < 0) {
		user_panic("[SURVIVE] cannot read /survive: %d", r);
	}
	compare_survive_msg(buf);
	compare_survive_msg(buf + strlen(survive_msg) + 1);
	compare_survive_msg(buf + 2 * (strlen(survive_msg) + 1));
	if ((r = close(survive_fd)) < 0) {
		user_panic("[SURVIVE] cannot close /survive_2: %d", r);
	}
	// Key3 -- Read
	compare_msg1("/msg1_key3", "/key3.key");
	// Key 3 -- Write
	compare_msg2("/msg2_key3", msg2_encrypted_key3, "/key3.key");
	// <<<=== key 3
	unset_key("/key3.key");

	return 0;
}
