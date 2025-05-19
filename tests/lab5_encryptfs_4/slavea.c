#include <lib.h>

static char *msg1 = "A spectre is haunting Europe - the spectre of Communism.\nAll the Powers of "
		    "old Europe have entered into a holy alliance to exorcize this spectre:\nPope "
		    "and Czar, Metternich and Guizot, French Radicals and German police spies.";

static char *msg2 = "Workers of the world, unite!";

static char msg2_encrypted[] = {0xd4, 0x35, 0x00, 0x12, 0x60, 0x2a, 0xfe, 0xc5, 0x1c, 0x1d,
				0xe2, 0x21, 0x51, 0x2d, 0x42, 0x93, 0x58, 0x72, 0xea, 0x58,
				0xf1, 0x09, 0xce, 0xdf, 0xce, 0xee, 0x79, 0xa3, 0x0e};

int main() {
	int r;
	int msg_fd;
	char buf[512];

	memset(buf, 0, sizeof(buf));
	while (!fsipc_key_isset()) {
		;
	}
	debugf("\n[SLAVEA] key is set\n");

	// Read (/msga)
	if ((r = open("/msga", O_RDONLY | O_ENCRYPT)) < 0) {
		user_panic("[SLAVEA] cannot open /msga: %d", r);
	}
	msg_fd = r;
	if ((r = read(msg_fd, buf, 511)) < 0) {
		user_panic("[SLAVEA] cannot read /msga: %d", r);
	}
	for (int i = 0; i < strlen(msg1) + 1; i++) {
		if (buf[i] != msg1[i]) {
			user_panic("[SLAVEA] read /msga returned wrong data at %d: %02x != %02x", i,
				   (unsigned char)buf[i], (unsigned char)msg1[i]);
		} else {
			debugf("%c", buf[i]);
		}
	}
	if ((r = close(msg_fd)) < 0) {
		user_panic("[SLAVEA] cannot close /msga: %d", r);
	}
	debugf("\n[SLAVEA] read is good\n");

	// Write (/newmsga)
	if ((r = open("/newmsga", O_RDWR | O_ENCRYPT | O_CREAT)) < 0) {
		user_panic("[SLAVEA] cannot create and open /newmsga: %d", r);
	}
	msg_fd = r;
	if ((r = write(msg_fd, msg2, strlen(msg2) + 1)) < 0) {
		user_panic("[SLAVEA] cannot write /newmsga: %d", r);
	}
	if ((r = close(msg_fd)) < 0) {
		user_panic("[SLAVEA] cannot close /newmsga: %d", r);
	}
	// Read (/newmsga)
	if ((r = open("/newmsga", O_RDONLY)) < 0) {
		user_panic("[SLAVEA] cannot open /newmsga: %d", r);
	}
	msg_fd = r;
	if ((r = read(msg_fd, buf, 511)) < 0) {
		user_panic("[SLAVEA] cannot read /newmsga: %d", r);
	}
	for (int i = 0; i < strlen(msg2) + 1; i++) {
		if (buf[i] != msg2_encrypted[i]) {
			user_panic("[SLAVEA] read /newmsga returned wrong data at %d: %02x != %02x",
				   i, (unsigned char)buf[i], (unsigned char)msg2_encrypted[i]);
		}
	}
	for (int i = 0; i < strlen(msg2) + 1; i++) {
		debugf("%02x ", (unsigned char)buf[i]);
	}
	debugf("\n");
	if ((r = close(msg_fd)) < 0) {
		user_panic("[SLAVEA] cannot close /newmsga: %d", r);
	}
	debugf("\n[SLAVEA] write is good\n");

	return 0;
}
