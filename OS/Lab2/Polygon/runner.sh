#!/bin/bash

set -u

IP1="127.0.0.1"
IP2="192.168.67.130"

cleanup_port() {
  local proto="$1"
  local port="$2"


  if [ "$proto" = "tcp" ]; then
    fuser -k "${port}/tcp" 2>/dev/null || true
  elif [ "$proto" = "udp" ]; then
    fuser -k "${port}/udp" 2>/dev/null || true
  fi
}

cleanup_range() {
  local proto="$1"
  local start="$2"
  local end="$3"

  for ((port=start; port<=end; port++)); do
    cleanup_port "$proto" "$port"
  done
}

cleanup_all() {
  echo "=== START CLEANUP ==="
  echo "IPs used in tests: $IP1, $IP2"
  cleanup_port tcp 9011
  cleanup_port udp 9050
  cleanup_range udp 9960 9969
  cleanup_range udp 9970 9979
  cleanup_range udp 9980 9989
  pkill -f tcpserveremul.rb 2>/dev/null || true
  pkill -f udpserveremul.rb 2>/dev/null || true
  pkill -f tcpsrvemul 2>/dev/null || true
  pkill -f udpsrvemul 2>/dev/null || true

  echo "=== CLEANUP DONE ==="
  echo
}

run_test() {
  local testfile="$1"

  echo "========================================"
  echo "Running $testfile"
  echo "========================================"

  cleanup_all
  ruby "$testfile"
  cleanup_all
}

trap cleanup_all EXIT
python CLgen.py
run_test run_1.rb
run_test run_2.rb
run_test run_3.rb
run_test run_4.rb
run_test run_5.rb
# run_test run_6.rb