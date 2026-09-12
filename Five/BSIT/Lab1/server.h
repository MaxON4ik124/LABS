#pragma once

// Runs an IPv4 information server until Ctrl+C. Returns zero on normal shutdown.
int run_tcp_server(unsigned short port);
