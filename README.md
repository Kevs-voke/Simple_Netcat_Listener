Simple TCP Listener

This is basically an nc -l (netcat listener) clone written in C++ using the Asio
 library.
It listens on port 1234, accepts incoming TCP connections, and prints whatever clients send.

How to Run
g++ -std=c++17 main.cpp -o tcp_listener -pthread
./tcp_listener


You’ll see:

Server is running on port 1234...

Test with Netcat

Open another terminal and type:

nc localhost 1234


Anything you type here will be printed in the server console along with a timestamp.
