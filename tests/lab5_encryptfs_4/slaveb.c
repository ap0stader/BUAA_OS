#include <lib.h>

static char *msg1 = "In place of the old bourgeois society, with its classes and class "
		    "antagonisms,\nwe shall have an association,\nin which the free development of "
		    "each is the condition for the free development of all.";

static char *msg2 =
    "The history of all hitherto existing society is the history of class struggles.";

static char msg2_encrypted[] = {
    0xd7, 0x32, 0x17, 0x59, 0x6d, 0x31, 0xfe, 0x91, 0x1c, 0x09, 0xbb, 0x75, 0x56, 0x2e, 0x42, 0x85,
    0x5b, 0x6c, 0xa6, 0x54, 0xb4, 0x5d, 0xd3, 0xd4, 0xd5, 0xee, 0x73, 0xa2, 0x6b, 0x26, 0x5f, 0xce,
    0x1d, 0x92, 0x55, 0x62, 0x77, 0xb2, 0x6d, 0x9a, 0x64, 0x7e, 0xaf, 0x1f, 0x1d, 0xf4, 0xf1, 0xae,
    0x4b, 0x74, 0x29, 0xf3, 0xed, 0x7e, 0x05, 0x8e, 0x09, 0x60, 0x3f, 0xc7, 0xdc, 0xce, 0x8f, 0x0b,
    0xa9, 0xed, 0x63, 0x07, 0xc2, 0xb9, 0x5e, 0xcb, 0x29, 0x9e, 0x3b, 0xb3, 0x3f, 0x2c, 0xfa, 0xbe};

int main() {
	int r;
	int msg_fd;
	char buf[512];

	memset(buf, 0, sizeof(buf));
	while (!fsipc_key_isset()) {
		;
	}
	debugf("\n[SLAVEB] key is set\n");

	// Write (/newmsgb)
	if ((r = open("/newmsgb", O_RDWR | O_ENCRYPT | O_CREAT)) < 0) {
		user_panic("[SLAVEB] cannot create and open /newmsgb: %d", r);
	}
	msg_fd = r;
	if ((r = write(msg_fd, msg2, strlen(msg2) + 1)) < 0) {
		user_panic("[SLAVEB] cannot write /newmsgb: %d", r);
	}
	if ((r = close(msg_fd)) < 0) {
		user_panic("[SLAVEB] cannot close /newmsgb: %d", r);
	}
	// Read (/newmsgb)
	if ((r = open("/newmsgb", O_RDONLY)) < 0) {
		user_panic("[SLAVEB] cannot open /newmsgb: %d", r);
	}
	msg_fd = r;
	if ((r = read(msg_fd, buf, 511)) < 0) {
		user_panic("[SLAVEB] cannot read /newmsgb: %d", r);
	}
	for (int i = 0; i < strlen(msg2) + 1; i++) {
		if (buf[i] != msg2_encrypted[i]) {
			user_panic("[SLAVEB] read /newmsgb returned wrong data at %d: %02x != %02x",
				   i, (unsigned char)buf[i], (unsigned char)msg2_encrypted[i]);
		}
	}
	for (int i = 0; i < strlen(msg2) + 1; i++) {
		debugf("%02x ", (unsigned char)buf[i]);
	}
	debugf("\n");
	if ((r = close(msg_fd)) < 0) {
		user_panic("[SLAVEB] cannot close /newmsgb: %d", r);
	}
	debugf("\n[SLAVEB] write is good\n");

	// Read (/msgb)
	if ((r = open("/msgb", O_RDONLY | O_ENCRYPT)) < 0) {
		user_panic("[SLAVEB] cannot open /msgb: %d", r);
	}
	msg_fd = r;
	if ((r = read(msg_fd, buf, 511)) < 0) {
		user_panic("[SLAVEB] cannot read /msgb: %d", r);
	}
	for (int i = 0; i < strlen(msg1) + 1; i++) {
		if (buf[i] != msg1[i]) {
			user_panic("[SLAVEB] read /msgb returned wrong data at %d: %02x != %02x", i,
				   (unsigned char)buf[i], (unsigned char)msg1[i]);
		} else {
			debugf("%c", buf[i]);
		}
	}
	if ((r = close(msg_fd)) < 0) {
		user_panic("[SLAVEB] cannot close /msgb: %d", r);
	}
	debugf("\n[SLAVEB] read is good\n");

	return 0;
}
