#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 1024

int main()
{
    int sock;
    struct sockaddr_in serv_addr;

    char buffer[BUFFER_SIZE];
    char input[BUFFER_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    printf("Client socket created.\n");

    // Server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IP address
    if (inet_pton(AF_INET, "127.0.0.1",
                  &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address");
        close(sock);
        exit(1);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr,
                sizeof(serv_addr)) < 0)
    {
        perror("Connection failed");
        close(sock);
        exit(1);
    }

    printf("Connected to server.\n");
    printf("Enter command like: ANALYZE | Hello World\n");
    printf("Type bye to exit.\n\n");

    while (1)
    {
        printf("Enter: ");

        fgets(input, BUFFER_SIZE, stdin);

    
        input[strcspn(input, "\n")] = '\0';

        
        send(sock, input, strlen(input), 0);

    
        if (strcmp(input, "bye") == 0)
        {
            break;
        }

    
        memset(buffer, 0, BUFFER_SIZE);

        int valread = read(sock, buffer,
                           BUFFER_SIZE - 1);

        if (valread <= 0)
        {
            printf("Server disconnected.\n");
            break;
        }

        buffer[valread] = '\0';

        printf("Server: %s\n", buffer);
    }

    close(sock);

    return 0;
}