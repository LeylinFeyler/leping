# leping (C)

A small `ping`-like ICMP client written in C using raw sockets.

Features:

* ICMP echo requests and replies
* RTT measurement
* Packet statistics
* Configurable packet count, interval, timeout and payload size
* Graceful Ctrl+C handling
* IPv4 + IPv6-ready networking

---

## Build

```bash
make
```

---

## Run

Raw sockets require elevated privileges.

Run with sudo:

```bash
sudo ./leping <host>
```

or allow the binary to use raw sockets without sudo:

```bash
sudo setcap cap_net_raw+ep ./leping
./leping <host>
```

Example:

```bash
./leping google.com
```

---

## Usage

```bash
./leping [options] <host>
```

Options:

```text
-c <count>      number of packets
-i <interval>   delay between packets (seconds)
-s <size>       packet payload size
-t <timeout>    receive timeout (seconds)
-h              show help
```
