#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUFFER_SIZE 1024

// Count words
int count_words(char *str)
{
    int count = 0;
    int in_word = 0;

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isspace(str[i]) && in_word == 0)
        {
            count++;
            in_word = 1;
        }
        else if (isspace(str[i]))
        {
            in_word = 0;
        }
    }

    return count;
}

// Count vowels
int count_vowels(char *str)
{
    int count = 0;

    for (int i = 0; str[i] != '\0'; i++)
    {
        char ch = tolower(str[i]);

        if (ch == 'a' || ch == 'e' || ch == 'i' ||
            ch == 'o' || ch == 'u')
        {
            count++;
        }
    }

    return count;
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];


    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    printf("Server socket created.\n");

    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(1);
    }

    printf("Bind successful.\n");


    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    
    socklen_t addrlen = sizeof(address);

    new_socket = accept(server_fd,
                        (struct sockaddr *)&address,
                        &addrlen);

    if (new_socket < 0)
    {
        perror("Accept failed");
        close(server_fd);
        exit(1);
    }

    printf("Client connected.\n");

    
    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);

        int valread = read(new_socket, buffer, BUFFER_SIZE - 1);

        if (valread <= 0)
            break;

        buffer[valread] = '\0';

    
        buffer[strcspn(buffer, "\n")] = '\0';

        
        if (strcmp(buffer, "bye") == 0)
        {
            printf("Client disconnected.\n");
            break;
        }

        char command[20];
        char text[BUFFER_SIZE];

        memset(command, 0, sizeof(command));
        memset(text, 0, sizeof(text));

        
        if (sscanf(buffer, " %19[^|]|%1023[^\n]",
                   command, text) == 2)
        {
            
            char *ptr = text;

            while (*ptr == ' ')
                ptr++;

            strcpy(text, ptr);

            
            for (int i = strlen(command) - 1;
                 i >= 0 && command[i] == ' '; i--)
            {
                command[i] = '\0';
            }

            if (strcmp(command, "ANALYZE") == 0)
            {
                int chars = strlen(text);
                int words = count_words(text);
                int vowels = count_vowels(text);

                char response[BUFFER_SIZE];

                sprintf(response,
                        "Chars = %d, Words = %d, Vowels = %d",
                        chars, words, vowels);

                send(new_socket, response,
                     strlen(response), 0);
            }
            else
            {
                char *msg = "Invalid Command";

                send(new_socket, msg,
                     strlen(msg), 0);
            }
        }
        else
        {
            char *msg = "Invalid Format. Use: ANALYZE | TEXT";

            send(new_socket, msg,
                 strlen(msg), 0);
        }
    }

    close(new_socket);
    close(server_fd);

    return 0;
}