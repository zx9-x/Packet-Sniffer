🕵️‍♂️Network Communication Suite: Winsock Server, Client & Packet Sniffer

A complete educational project demonstrating **TCP networking** and **real-time packet analysis** on Windows. This suite includes:
-  A **TCP server** (Winsock)
-  A **TCP client** (Winsock)
-  A **packet sniffer** (Npcap)

Perfect for learning how data flows over networks — even on `localhost`.
🛑how to RUN:
1- Start the Sniffer (let it print "waiting for traffic")
2- Start the Server
3- Start the Client
4- Send a message
DEMO(project test)
![Image Alt](https://github.com/zx9-x/Packet-Sniffer/blob/1e186e8ec10514967469f0cf60b019159bc5e4ee/Packet%20Sniffer/test.png)

| **TCP Server** | Listens on `127.0.0.1:5555`, accepts client connections, and echoes messages. |
| **TCP Client** | Connects to the server and sends user messages. |
| **Packet Sniffer** | Captures loopback traffic, parses IP/TCP headers, filters by port `5555`, and displays payloads. |

