#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int create_socket_and_connect(const char *hostname) {
    int sockfd;
    struct sockaddr_in server_addr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80); // HTTP port
    inet_pton(AF_INET, hostname, &server_addr.sin_addr);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void send_http_get_request(int sockfd, const char *hostname, const char *path) {
    char request[1024];
    snprintf(request, sizeof(request), "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", path, hostname);
    send(sockfd, request, strlen(request), 0);
    printf("HTTP GET request sent to %s\n", hostname);
}

void receive_http_response(int sockfd) {
    char response[4096];
    int bytes_received;

    while ((bytes_received = recv(sockfd, response, sizeof(response) - 1, 0)) > 0) {
        response[bytes_received] = '\0'; // Null-terminate the response
        printf("%s", response);
    }
}

void interact_with_server(const char *hostname, const char *path) {
    // Call function to create a socket and connect to the server
    int sockfd = create_socket_and_connect(hostname);

    // Call function to send an HTTP GET request
    send_http_get_request(sockfd, hostname, path);

    // Call function to receive and print the HTTP response
    receive_http_response(sockfd);

    // Call function to close the socket
    close(sockfd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <hostname> <path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *hostname = argv[1];
    const char *path = argv[2];

    printf("Interacting with server %s...\n", hostname);
    interact_with_server(hostname, path);

    return EXIT_SUCCESS;
}
