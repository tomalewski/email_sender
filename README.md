# email_sender
## Written by Joseph Tomalewski
### For Professor Reynolds CS484 Computer Networks

I wrote this email sender in C and it is designed to send an email using the SMTP protocol. The SMTP or Simple Mail Transfer Protocol is to communicate between servers, specifically the SMTP server so you can send emails. 

The <sys/socket.h> and <arpa/inet.h> headers are very crucial for initializing the TCP connection and communicating with the SMTP server. 

I also included an email template that can be used, and a simple makefile for compiling the program using a terminal.

Here is a basic idea of how the program runs:
TCP Connection -> Parse Input -> Read File -> Speak SMTP

### How to run the program:

// USAGE: 

// ./email_sender 〈SMTP ADDR IPv4〉 〈DEST Email ADDR〉 〈Email Filename〉

### Can be found using DIG command:

// NMSU SMTP ADDR: mailrelay-p-vs.nmsu.edu.

// NMSU SMTP IPV4 A Records: 128.123.88.190

// SMTP Port: 25
