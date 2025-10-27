#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define WLSOCK_PATH "/tmp/wallski.sock"

int main(int argc, char **argv) {
  printf("wallski");
  const char *wp = NULL;
  const char *transition = "wipe";

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--set") == 0 && i + 1 < argc) {
      wp = argv[++i];
    } else if (strcmp(argv[i], "--transition") == 0 && i + 1 < argc) {
      transition = argv[++i];
    }
  }

  if (!wp) {
    fprintf(stderr, "usg: %s --set <path> --transitoin <type>\n", argv[0]);
    return 1;
  }

  // connection to daemon
  int sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock == -1) {
    perror("socket");
    return 1;
  }

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX; // socket family to unix domain

  // copying the stuff to sunpath
  strncpy(addr.sun_path, WLSOCK_PATH, sizeof(addr.sun_path) - 1);

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("connect");
    return 1;
  }

  // send command
  char buf[512];
  snprintf(buf, sizeof(buf), "--set %s --transition %s", wp, transition);
  if (write(sock, buf, strlen(buf)) == -1) {
    perror("write");
    return 1;
  }
  close(sock);
  return 0;
}
