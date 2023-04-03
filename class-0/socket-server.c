#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define MAX_CLIENTS 30

// Estrutura para armazenar informacoes dos clientes conectados
struct client {
    int socket;
    char name[20];
};

// enviar mensagem para todos os clientes conectados
void send_message(char* message, int sender_socket, struct client* clients, int num_clients) {
    for(int i = 0; i <num_clients; i++) {
        if (clients[i].socket != sender_socket) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }
}

int main(int argc, char const *argv[]) {

    int server_socket, new_socket, activity, valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    fd_set readfds;
    struct client clients[MAX_CLIENTS];
    int num_clients = 0;

    // Cria um Socket TCP
    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Define as propriedades do Socket
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Vincula o Socket a porta especificada
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Define o Socket como possível de receber conexões
    if(listen(server_socket, 3) < 0) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);

        for (int i = 0; i < num_clients; i++) {
            FD_SET(clients[i].socket, &readfds);
        }

        // Espera até que haja atividade em algum dos sockets
        activity = select(0, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("Select");
            exit(EXIT_FAILURE);
        }

        // Se houver atividade no socket do servidor
        // Existe uma nova conexão para ser aceita
        if(FD_ISSET(server_socket, &readfds)) {
            if ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // Adiciona o novo cliente a lista
            struct client new_client = { new_socket, "" };
            clients[num_clients++] = new_client;

            // Envia uma mensagem de boas-vindas para o novo cliente
            char welcome_message[100] = "Bem-vindo ao chat!\nDigite o seu nome:";
            send(new_socket, welcome_message, strlen(welcome_message), 0);
        }

        // Verifica se há mensagens de algum dos clientes
        
    }
    
    return EXIT_SUCCESS;
}

