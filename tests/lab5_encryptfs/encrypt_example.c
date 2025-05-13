#include <lib.h>

static char *msg1 = "Let the ruling classes tremble at a Communistic "
		    "revolution.\nThe proletarians have nothing to lose but "
		    "their chains.\nThey have a world to win.\n";

static char *msg2 = "Workers of the world, unite!";

static char msg2_encrypted[] = {0xee, 0x83, 0x4c, 0xd0, 0x60, 0xa4, 0x6e, 0x00, 0xc2, 0xc8,
				0x64, 0x6a, 0x0e, 0x9a, 0x28, 0x22, 0x92, 0x62, 0x61, 0x29,
				0x8f, 0x2e, 0xa3, 0x19, 0x49, 0xbf, 0x82, 0x91, 0x54};

int main() {
	int r;
	int key_fd, msg_fd;
	char buf[512];

	if ((r = open("/key0.key", O_RDONLY)) < 0) {
		user_panic("cannot open /key0.key: %d", r);
	}
	key_fd = r;
	debugf("open key0.key is good\n");

	if ((r = fskey_set(key_fd)) < 0) {
		user_panic("fskey_set: %d", r);
	}
	debugf("fskey_set is good\n");

	close(key_fd);

	if (fskey_isset() != 1) {
		user_panic("fskey_isset: %d", r);
	}
	debugf("fskey_isset is good\n");

	// Read an encrypted file /msg
	if ((r = open("/msg", O_RDONLY | O_ENCRYPT)) < 0) {
		user_panic("cannot open /msg: %d", r);
	}
	msg_fd = r;
	if ((r = read(msg_fd, buf, 511)) < 0) {
		user_panic("cannot read /msg: %d", r);
	}
	for (int i = 0; i < strlen(msg1) + 1; i++) {
		if (buf[i] != msg1[i]) {
			user_panic("read /msg returned wrong data at %d: %02x != %02x", i,
				   (unsigned char)buf[i], (unsigned char)msg1[i]);
		} else {
			debugf("%c", buf[i]);
		}
	}
	if ((r = close(msg_fd)) < 0) {
		user_panic("cannot close /msg: %d", r);
	}
	debugf("read is good\n");

	// Write an encrypted file /newmsg
	if ((r = open("/newmsg", O_RDWR | O_ENCRYPT | O_CREAT)) < 0) {
		user_panic("cannot create and open /newmsg: %d", r);
	}
	msg_fd = r;
	if ((r = write(msg_fd, msg2, strlen(msg2) + 1)) < 0) {
		user_panic("cannot write /newmsg: %d", r);
	}
	if ((r = close(msg_fd)) < 0) {
		user_panic("cannot close /newmsg: %d", r);
	}
	// -----
	if ((r = open("/newmsg", O_RDONLY)) < 0) {
		user_panic("cannot open /newmsg: %d", r);
	}
	msg_fd = r;
	if ((r = read(msg_fd, buf, 511)) < 0) {
		user_panic("cannot read /newmsg: %d", r);
	}
	for (int i = 0; i < strlen(msg2) + 1; i++) {
		if (buf[i] != msg2_encrypted[i]) {
			user_panic("read /newmsg returned wrong data at %d: %02x != %02x", i,
				   (unsigned char)buf[i], (unsigned char)msg2_encrypted[i]);
		}
	}
	for (int i = 0; i < strlen(msg2) + 1; i++) {
		debugf(" %c ", msg2[i]);
	}
	debugf("\n");
	for (int i = 0; i < strlen(msg2) + 1; i++) {
		debugf("%02x ", (unsigned char)buf[i]);
	}
	debugf("\n");
	if ((r = close(msg_fd)) < 0) {
		user_panic("cannot close /newmsg: %d", r);
	}
	debugf("write is good\n");

	if ((r = fskey_unset()) < 0) {
		user_panic("fskey_unset: %d", r);
	}
	if (fskey_isset() != 0) {
		user_panic("fskey_isset: %d", r);
	}
	debugf("fskey_unset is good\n");

	return 0;
}
