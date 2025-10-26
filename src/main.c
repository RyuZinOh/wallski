#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define WLSOCK_PATH "/tmp/wallski.sock"

int main(int argc, char **argv) {
  printf("wallski");

  if (argc < 3 || strcmp(argv[1], "--set") != 0) {
    fprintf(stderr, "usg: %s\n", argv[0]);
    return 1;
  }
  const char *wp = argv[2];

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
  snprintf(buf, sizeof(buf), "--set %s", wp);
  if (write(sock, buf, strlen(buf)) == -1) {
    perror("write");
    return 1;
  }
  close(sock);
  return 0;
}
