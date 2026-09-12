from __future__ import annotations

import os
import select
import socket
import socketserver

LISTEN_PORT = int(os.environ["LISTEN_PORT"])
TARGET_HOST = os.environ["TARGET_HOST"]
TARGET_PORT = int(os.environ["TARGET_PORT"])


class ProxyHandler(socketserver.BaseRequestHandler):
    def handle(self):
        with socket.create_connection((TARGET_HOST, TARGET_PORT), timeout=10) as upstream:
            sockets = [self.request, upstream]

            while True:
                readable, _, exceptional = select.select(sockets, [], sockets, 60)
                if exceptional or not readable:
                    return

                for source in readable:
                    data = source.recv(65536)
                    if not data:
                        return

                    destination = upstream if source is self.request else self.request
                    destination.sendall(data)


class ThreadingTCPServer(socketserver.ThreadingTCPServer):
    allow_reuse_address = True
    daemon_threads = True


if __name__ == "__main__":
    print(
        f"Proxy 0.0.0.0:{LISTEN_PORT} -> {TARGET_HOST}:{TARGET_PORT}",
        flush=True,
    )
    with ThreadingTCPServer(("0.0.0.0", LISTEN_PORT), ProxyHandler) as server:
        server.serve_forever()
