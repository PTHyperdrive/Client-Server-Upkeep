#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP   "your_server_ip"
#define SERVER_PORT 1234  // Replace with your chosen port

int get_public_ip(char *ip_buffer) {
    // Implementation using Ipify API
    FILE *fp = popen("curl -s https://api.ipify.org", "r");
    if (fp == NULL) return -1;
    fgets(ip_buffer, 256, fp); 
    pclose(fp);
    return 0;
}

int get_hostname(char *hostname_buffer) {
    char cmd[256];
    sprintf(cmd, "powershell.exe -Command \"(Get-CimInstance -ClassName Win32_ComputerSystem).Name\" | Out-String");
    FILE *fp = popen(cmd, "r");
    if (fp == NULL) return -1;
    fgets(hostname_buffer, 256, fp); 
    pclose(fp);
    return 0;
}

// (Similar functions to get hostname and gateway using PowerShell)

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    // Socket setup
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // UDP
    // ... Error handling

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    while (1) {
        char ip[256], hostname[256];
        get_public_ip(ip); 
        get_hostname(hostname);

        // Construct packet payload
        char payload[512]; 
        sprintf(payload, "IP: %s, Hostname: %s", ip, hostname);

        // Send to server 
        sendto(sockfd, payload, strlen(payload), 0, 
               (struct sockaddr *)&server_addr, sizeof(server_addr));

        sleep(60); // Send every 60 seconds
    }

    close(sockfd);
    return 0;
}
