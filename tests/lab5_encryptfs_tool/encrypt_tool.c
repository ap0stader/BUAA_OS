#include <lib.h>

// static char *msg = "Let the ruling classes tremble at a Communistic revolution.\nThe proletarians have nothing to lose but their chains.\nThey have a world to win.\n";

static char *msg = "Workers of the world, unite!   ";

// static char *msg = "A spectre is haunting Europe - the spectre of Communism.\nAll the Powers of old Europe have entered into a holy alliance to exorcize this spectre:\nPope and Czar, Metternich and Guizot, French Radicals and German police spies.";

// static char *msg = "In place of the old bourgeois society, with its classes and class antagonisms,\nwe shall have an association,\nin which the free development of each is the condition for the free development of all.";

// static char *msg = "The history of all hitherto existing society is the history of class struggles.";

// static char *msg = "In short, the Communists everywhere support every revolutionary movement against the existing social and political order of things.";

// static char *msg = "In all these movements they bring to the front, as the leading question in each, the property question, no matter what its degree of development at the time.";

// static char *msg = "Finally, the labour everywhere for the union and agreement of the democratic parties of all countries.";

int main() {
    int r;
    int key_fd, msg_fd;
    char buf[512];
    
    key_fd = open("/key0.key", O_RDONLY);
    r = fskey_set(key_fd);
    debugf("%d\n", r);
    close(key_fd);

    msg_fd = open("/newmsg", O_RDWR | O_ENCRYPT | O_CREAT);
    r = write(msg_fd, msg, strlen(msg) + 1);
    debugf("%d\n", r);
    r = close(msg_fd);
    debugf("%d\n", r);

    msg_fd = open("/newmsg", O_RDONLY);
    read(msg_fd, buf, 511);
    for (int i = 0; i < strlen(msg) + 1; i++) {
        debugf(" %c", msg[i]);
        if (msg[i] != '\n') {
            debugf(" ");
	}
    }
    debugf("\n");
    for (int i = 0; i < strlen(msg) + 1; i++) {
        debugf("%02x ", (unsigned char) buf[i]);
        if (msg[i] == '\n') {
            debugf("\n");
        }
    }
    debugf("\n");
    close(msg_fd);
    fskey_unset();

    return 0;
}
