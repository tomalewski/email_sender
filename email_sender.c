#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <regex.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// USAGE: 
// ./email_sender 〈SMTP ADDR IPv4〉 〈DEST Email ADDR〉 〈Email Filename〉

// NMSU SMTP ADDR: mailrelay-p-vs.nmsu.edu.
// NMSU SMTP IPV4 A Records: 128.123.88.190
// SMTP Port: 25


// Send a command and get a response from the SMTP server:
void send_receive_command(int socket, const char* command, char* buffer, int buf_size) {
    send(socket, command, strlen(command), 0);
    recv(socket, buffer, buf_size, 0);
	// Print server response:
    printf("SMTP: %s", buffer);  
}

// Validate DEST email format:
int isValidEmail(const char* email) {
    regex_t regex;
    int reti;
    // regex for email format:
    const char* pattern = "^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}$";
    
    reti = regcomp(&regex, pattern, REG_EXTENDED);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        return 0;
    }

    reti = regexec(&regex, email, 0, NULL, 0);
    regfree(&regex);
    
    if (reti == REG_NOMATCH) {
        return 0;
    }
    return 1;
}

// Validate DEST IP format (Only IPv4):
int isValidIP(const char* ip) {
    regex_t regex;
    int reti;
	// regex for IP format:
    const char* pattern = "^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\."
                          "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$";
    
    reti = regcomp(&regex, pattern, REG_EXTENDED);
    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        return 0;
    }

    reti = regexec(&regex, ip, 0, NULL, 0);
    regfree(&regex);
    
    if (reti == REG_NOMATCH) {
        return 0;
    }
    return 1;
}

int main(int argc, char** argv){

	// Check for arg count:
	if (argc != 4){
		printf("Usage is not as follows: ./email_sender 〈SMTP ADDR IPv4〉 〈DEST Email ADDR〉 〈Email Filename〉\nYou need 3 parameters.\n");
		return 1;
	}

	// Socket Descriptor/Handle:
	int socket_desc;
	struct sockaddr_in server;

	// Initialize: 
	char server_reply[2000];
	char email_content[2000];

	// Command line:
    char* smtp_server_ip = argv[1];
    char* recipient = argv[2];
    char* email_filename = argv[3];
	// SMTP command buffer:
	char smtp_command[512];

	// (CHANGE) Sender:
	char sender[] = "tj04420@nmsu.edu";


	// Check SMTP ADDR:
	if (isValidIP(smtp_server_ip) == 0){
		printf("Invalid IP, doesn't match format.\n");
		return 1;
	}

	// Check DEST email:
	if (isValidEmail(recipient) == 0){
		printf("Invalid email, doesn't match format.\n");
		return 1;
	}

	// Attempt opening email file:
	FILE* file = fopen(email_filename, "r");
    if (!file) {
        printf("Failed to open email file: %s\n", email_filename);
        return 1;
    }
	// Read into email_content:
    fread(email_content, sizeof(char), sizeof(email_content)-1, file);
    fclose(file);
	
	// Initialize socket: AF_INET - IPV4, SOCK_STREAM - TCP connection, 0 - Protocol
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	// socket_desc will return -1 when encounters error:
	if (socket_desc == -1){
		printf("Could not create socket!\n");
		return 1;
	}
	
	// Configure SMTP server:
	server.sin_addr.s_addr = inet_addr(smtp_server_ip);
	server.sin_family = AF_INET;
	server.sin_port = htons(25);

	// Connect to server:
	if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) == -1){
		printf("Connection Error.");
		return 1;
	}

	// Clear reply buffer:
    memset(server_reply, 0, sizeof(server_reply));
    // Read server's greeting message:
    recv(socket_desc, server_reply, sizeof(server_reply), 0);
    printf("Greeting: %s", server_reply);


	// SMTP Server Commands:
    // HELO:
    send_receive_command(socket_desc, "HELO client\r\n", server_reply, sizeof(server_reply));

    // MAIL FROM:
    snprintf(smtp_command, sizeof(smtp_command), "MAIL FROM:<%s>\r\n", sender);
    send_receive_command(socket_desc, smtp_command, server_reply, sizeof(server_reply));

    // RCPT TO:
    snprintf(smtp_command, sizeof(smtp_command), "RCPT TO:<%s>\r\n", recipient);
    send_receive_command(socket_desc, smtp_command, server_reply, sizeof(server_reply));

    // DATA:
    send_receive_command(socket_desc, "DATA\r\n", server_reply, sizeof(server_reply));

    // Send email content:
    snprintf(smtp_command, sizeof(smtp_command), "%s\r\n.\r\n", email_content);
    send_receive_command(socket_desc, smtp_command, server_reply, sizeof(server_reply));

    // Quit and close the socket connection:
    send_receive_command(socket_desc, "QUIT\r\n", server_reply, sizeof(server_reply));
    close(socket_desc);  
}