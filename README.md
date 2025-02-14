# 🌐 Internet Relay Chat

### **A lightweight IRC (Internet Relay Chat) server implementation in C++98, featuring fundamental IRC functionalities and client communication over a network**

---

## Features
- **Non-blocking I/O**: Handle multiple clients simultaneously using poll()
- **TCP/IP**: Network communication (IPv4/IPv6)
- **Authentication**: Secure password-protected connections
- **Real-time**: Instant message delivery and channel updates

---

## Video Demo
[Coming Soon]

---

## Quick Setup

1. **Build the Server**
   ```bash
   make
   ```

2. **Launch the Server**
   ```bash
   ./ircserv <port> <password>
   ```

3. **Connect with Any IRC Client**
   ```bash
   nc localhost <port>
   ```

---

## Technical Details

<details>
  <summary><strong>Available Commands</strong></summary>

### User Commands
- **NICK**: Set/change nickname
- **USER**: Set username and real name
- **JOIN**: Enter a channel
- **PRIVMSG**: Send private messages to users/channels
- **QUIT**: Disconnect from server

### Operator Commands
- **KICK**: Remove user from channel
- **INVITE**: Invite user to channel
- **TOPIC**: Set/view channel topic
- **MODE**: Modify channel settings
  - `i`: Toggle invite-only
  - `t`: Restrict topic changes to operators
  - `k`: Set/remove channel password
  - `o`: Grant/revoke operator status
  - `l`: Set/remove user limit

</details>

<details>
  <summary><strong>Technical Specifications</strong></summary>

- **Language**: C++98
- **Network**: TCP/IP (IPv4/IPv6) implementation
- **Architecture**: Single-process, non-blocking I/O
- **System Calls**: poll(), socket, fcntl
- **Client Support**: Compatible with standard IRC clients
- **Error Handling**: Comprehensive error management
- **Memory**: No memory leaks

</details>

---

#### 🖥️ Works on Linux, Mac and WSL(for Windows)
