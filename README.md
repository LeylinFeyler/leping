# leping (C)

A minimal `ping`-like ICMP client written in C.

* Sends ICMP echo requests to a host.
* Shows round-trip time (RTT) for each reply.
* Handles Ctrl+C gracefully and prints statistics.

---

## Build

Compile the project:

```bash
make
```

---

## Run

You need root privileges to use raw sockets. Either run with sudo:

```bash
sudo ./leping <host>
```

or give the binary permission to use raw sockets without sudo:

```bash
sudo setcap cap_net_raw+ep ./leping
./leping <host>
```

Example:

```bash
./leping google.com
```
