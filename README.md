# RTS-A2
This repo is the assignment two of Real Time System. The program will run on the Tiva TM4C1294XL.

In this assignment we are to design, implement, and test a light-weight kernel that allows different processes to run “simultaneously” by swapping them in a round-robin fashion.
Each process is to be assigned one of five priority levels. Processes with the higher priority are to run before processes at a lower priority. Priority levels can be changed at run-time by the process. Processes can change priority using the nice() kernel call.
A program developer should be allowed to initiate any number of processes (limited only by the amount of available process control block memory and stack memory). Once all processes have been initiated, the processes should be allowed to run (determined by their priority) and, when required, terminate.
Processes can operate as clients, servers, or both. The kernel is to support an Inter-Process Communication (IPC) system, allowing processes to send and receive messages. Messages are sent to queues rather than processes, requiring processes to bind to queues. Processes waiting for messages are to block until a message arrives.