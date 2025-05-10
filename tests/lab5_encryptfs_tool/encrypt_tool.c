#include <lib.h>

static char *msg = "Let the ruling classes tremble at a Communistic revolution.\nThe proletarians have nothing to lose but their chains.\nThey have a world to win.\n";

int main() {
    int r;
    int key_fd, msg_fd;
    char buf[512];
    
    key_fd = open("/key0.key", O_RDONLY);
    fskey_set(key_fd);
    close(key_fd);

    msg_fd = open("/newmsg", O_RDWR | O_ENCRYPT | O_CREAT);
    write(msg_fd, msg, strlen(msg) + 1);
    close(msg_fd);

    msg_fd = open("/newmsg", O_RDONLY);
    read(msg_fd, buf, 511);
    for (int i = 0; i < strlen(msg) + 1; i++) {
        debugf(" %c ", msg[i]);
    }
    debugf("\n");
    for (int i = 0; i < strlen(msg) + 1; i++) {
        debugf("%02x ", (unsigned char) buf[i]);
    }
    debugf("\n");
    close(msg_fd);
    fskey_unset();

    return 0;
}
