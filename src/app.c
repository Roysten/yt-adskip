#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include "wayland.h"

#define ARRAY_COUNT(X) (sizeof(X) / sizeof(*X))

int main(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	struct Wl_ctx wl_ctx = {0};
	wlc_setup_wl_ctx(&wl_ctx);
	printf("%d %d\n", wl_ctx.width, wl_ctx.height);

	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		fprintf(stderr, "Could not create socket\n");
		return 1;
	}

	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(13276);
	int flag = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &flag, sizeof(flag));

	int rc = bind(sock, (struct sockaddr *) &addr, sizeof(addr));
	if (rc != 0) {
		fprintf(stderr, "Failed to bind socket\n");
		return 1;
	}

	rc = listen(sock, 1);
	if (rc != 0) {
		fprintf(stderr, "Listen failure\n");
		return 1;
	}

	for (;;) {
		int client_sock = accept(sock, NULL, NULL);
		if (client_sock == -1) {
			fprintf(stderr, "Accept failure\n");
			return 1;
		}

		setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(flag));

		char buf[65535] = {};
		rc = recv(client_sock, buf, ARRAY_COUNT(buf), 0);
		if (rc <= 0) {
			fprintf(stderr, "recv returned %d\n", rc);
			close(client_sock);
			continue;
		}

		printf("\nRequest:\n%s\n", buf);

		int x = 0;
		int y = 0;
		rc = sscanf(buf, "GET /?x=%d&y=%d", &x, &y);
		const char *response = NULL;
		if (rc == 2) {
			response = "HTTP/1.0 200 OK\r\n"
				   "Access-Control-Allow-Origin: *\r\n"
				   "\r\n"
				   "<link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\">";
			printf("Moving cursor to %d, %d\n", x, y);
			wlc_move_pointer(&wl_ctx, x, y);
			wlc_click(&wl_ctx);
		} else {
			response = "HTTP/1.0 400 Bad request\r\n"
				   "Access-Control-Allow-Origin: *\r\n"
				   "\r\n"
				   "<link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\">";
		}

		send(client_sock, response, strlen(response), 0);
		close(client_sock);
	}
	return 0;
}
