# Chatroom-Application
The Chatroom Application has been developed for the Linux environment and uses TCP/IP Socket programming for seamless server-client communication.

### Introduction
This documentation explains the functionality and implementation details of a program designed to support between server and client processes. The program is written in C and utilizes low-level socket programming techniques.

#### Purpose
The primary purpose of this program is to demonstrate how to use sockets in C to perform bidirectional and concurrent message transfer between two hosts(server and client/ server and multiple clients).

#### Components
The program consists of the following components:
1. **Header Files**: Include necessary header files for socket programming, networking and threading.
2. **Constants Definition**: Define constants, such as destination Port, Protocol, and buffer sizes.
3. **Sending and Receiving Functions**: Functions for constructing and sending data packets, and filling data.
4. **Main Caller Function**: The main function orchestrating the entire process, including creation of sockets, binding of sockets, listening to incoming connections and thread management.

#### Implementation Details
- **Modes of operation**: 
  - Supports 3 modes of operation.
  - **Sequential non-concurrent chatting**: Server responds after client has sent the message and vice versa.
  - **Flexible and concurrent message transfer**: Users may send no or multiple messages before receiving a response from another party.
  - **Flexible, concurrent message transfer with multi-client support**: Multiple clients may send messages to each other at the same time. Server acts a **chat mediator** forwards the messages to the intended client and also censoring sensitive chats. Server may also be extended for chatting as admin.
- **Message Sending**: 
  - Receives the message as input from the user, from terminal and packs the message into a character buffer for transfer.
  - Uses TCP/IP sockets to send message.
  - Packet is sent to the specific IP address obtained during binding of sockets and required port.
  - Use of separate thread for sending for concurrent message transfer.
- **Message Receiving**:
  - Continuously polls incoming messages.
  - Displays the messages from various clients.
- **Threading**:
  - Utilizes POSIX threads (`pthread`) for concurrent execution of message sending and receiving.

#### Usage
1. Clone the repo or download individual files. 
2. Compile the server program using a C compiler (`gcc` recommended). Use `gcc server.c -o server.out -pthread` to compile the program. server.c, server1.c and server2.c refer to the different modes of implementation.
3. Execute the compiled binary. Use `./server.out` to execute.
4. Compile the client program using a C compiler (`gcc` recommended). Use `gcc client.c -o client.out -pthread` to compile the program. client.c, client1.c and client2.c refer to the different modes of implementation.
5. Execute the compiled binary. Use `./client.out` to execute.

#### Dependencies
- Standard C libraries (`stdio.h`, `string.h`, `stdlib.h`, etc.).
- POSIX socket libraries (`sys/socket.h`, `netinet/in.h`, `arpa/inet.h`, etc.).
- POSIX thread library (`pthread.h`).
- Error handling using `errno` and `perror`.

#### Notes
- Modify the constants and configuration parameters according to the network environment and requirements.
- Error handling and resource cleanup mechanisms are included for robustness.
- This program is designed for Unix-like systems (tested on Linux) and may require adjustments for other platforms.
- Multi-client communication
<img width="960" alt="socket(prob-3-different VMs)-ss" src="https://github.com/Bipradeepb/Chatroom-Application/assets/78837695/3985dfe5-9baa-45c2-ad60-a92176abcaa2">
<img width="920" alt="socket(prob-3)-ss" src="https://github.com/Bipradeepb/Chatroom-Application/assets/78837695/9eb400a5-12c1-4047-a151-8f205288e1c9">
-Single server-client concurrent communication
<img width="907" alt="socket(prob-2)-ss" src="https://github.com/Bipradeepb/Chatroom-Application/assets/78837695/e32c2935-fa11-4c0a-8934-5d08ec56ab33">
-Single server-client sequential communication
<img width="926" alt="Socket(prob-1 1)ss" src="https://github.com/Bipradeepb/Chatroom-Application/assets/78837695/85f29831-999c-4136-bbc4-953b3de33581">
<img width="923" alt="Socket(prob-1)ss" src="https://github.com/Bipradeepb/Chatroom-Application/assets/78837695/7bfc5d1b-e4ea-4439-b480-539c331eca54">


#### Conclusion
The provided program showcases the implementation of a TCP/IP Client-Server chatroom application. It serves as a learning resource for understanding socket programming, threading, and message transmission in C over a network.
