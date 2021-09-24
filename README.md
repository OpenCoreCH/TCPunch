# C++ TCP NAT Hole Punching Library

C++ server and client library to perform NAT hole punching. 

# Usage

## Server
The server listens per default on port 10,000. Optionally, a different listening port can be specified as the first argument.

## Client Library

You only have to specify a pairing name (used to identify the clients, therefore needs to be identical for two clients that want to connect), the server address, and optionally a differnt port to `pair` to get back a socket descriptor that can be used for the communication with the peer, the rest is handled by the library:
```cpp
int sock_fd = pair("my_pairing_name", "1.1.1.1");
int n = recv(sock_fd, recv_buffer, size, 0);
```

```cpp
int sock_fd = pair("my_pairing_name", "1.1.1.1");
int bytes = send(sock_fd, pBuf, size, 0);
```
