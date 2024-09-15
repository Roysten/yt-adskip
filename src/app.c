#include <cstdio>
#include <cstdlib>

#include <sockets.h>

#include <fakeinput.hpp>

#define ARRAY_COUNT(X) (sizeof(X) / sizeof(*X))

int main(int argc, char **argv)
{
	sock_init();
	socket_t sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		fprintf(stderr, "Could not create socket\n");
		return 1;
	}

	struct sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(13276);
	int rc = bind(sock, (struct sockaddr*) &addr, sizeof(addr));
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
		auto client_sock = accept(sock, NULL, NULL);
		if (client_sock == INVALID_SOCKET) {
			fprintf(stderr, "Accept failure\n");
			return 1;
		}

		int flag = 1;
		setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(flag));

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
			response =
				"HTTP/1.0 200 OK\r\n"
				"Access-Control-Allow-Origin: *\r\n"
				"\r\n"
				"<link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\">";
			FakeInput::Mouse::moveTo(x, y);
			FakeInput::Mouse::pressButton(FakeInput::Mouse_Left);
			FakeInput::Mouse::releaseButton(FakeInput::Mouse_Left);
		} else {
			response =
				"HTTP/1.0 400 Bad request\r\n"
				"Access-Control-Allow-Origin: *\r\n"
				"\r\n"
				"<link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\">";
		}

		send(client_sock, response, strlen(response), 0);
		close(client_sock);
	}

	sock_cleanup();
	return 0;
}
