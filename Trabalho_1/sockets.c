#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdint.h>

#define _OPEN_SYS_ITOA_EXT
pid_t pid;



int gerador(){
	// --> gera o delta entre 1 e 100 que será adicionado ao ni
	srand(time(NULL));
	int r_numb = rand() % 100 + 1;
	return r_numb;
}

char *receptor(char* ni){
    int n = atoi(ni);
    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0) {
            return "Esse número não é primo.\n";
        }
    }
    return "Esse número é primo. WOW!\n";
}

int main( int argc, char *argv[] ) {
  	int falta_gerar;
	int enable = 1;

	falta_gerar = atoi(argv[1]);
	printf("Serão gerados %d números hoje\n", falta_gerar);
	pid = fork();

	
      //filho tem pid == 0 e do pai é > 0
      //Pai é o server
		if (pid > 0){
			int socket_desc, client_sock, client_size;
		    struct sockaddr_in server_addr, client_addr;
		    char server_message[2000], client_message[2000];
		    
		    // Limpa os buffers:
		    memset(server_message, '\0', sizeof(server_message));
		    memset(client_message, '\0', sizeof(client_message));
		    
		    // Create socket:
		    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
		    
		    if(socket_desc < 0){
            	printf("SERVIDOR: Erro ao criar o socket\n");
		        return -1;
		    }
        	printf("SERVIDOR: Socket criado com sucesso!\n");
		    
		    // Fixa porta e IP:
		    server_addr.sin_family = AF_INET;
		    server_addr.sin_port = htons(2000);
		    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		    
			setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
			if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
		        printf("SERVIDOR: Não foi possível se conectar a porta - server\n");
				return -1;
			}
		    printf("SERVIDOR: Conectado a porta.\n");
			
			if(listen(socket_desc, 1) < 0){
		        printf("SERVIDOR: Erro ao esperar conexões\n");
			    return -1;
			}
		    printf("\nSERVIDOR: Esperando conexão...\n");
			
			client_size = sizeof(client_addr);
			client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
			    
			if (client_sock < 0){
		        printf("SERVIDOR: Problema ao receber na conexão\n");
			   return -1;
			}
		    printf("SERVIDOR: Cliente conectado no IP: %s e porta: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			
			while(1){    
			    if (recv(client_sock, client_message, sizeof(client_message), 0) < 0){
		            printf("SERVIDOR: Não foi possível receber a mensagem\n");
			        return -1;
			    }

				if(atoi(client_message) == 0) {
		        	printf("SERVIDOR: Recebido 0, finalizando...\n");
					break;
				}
				
		        printf("SERVIDOR: Número recebido: %s\n", client_message);
			    
		        strcpy(server_message, receptor(client_message));
			    
			    if (send(client_sock, server_message, strlen(server_message), 0) < 0){
		            printf("SERVIDOR: Não foi possível enviar\n");
			        return -1;
			    }
			}
		    
		    close(client_sock);
		    close(socket_desc);
		}
        else{
			int socket_desc;
		    struct sockaddr_in server_addr;
		    char server_message[2000], client_message[2000];
		    
		    memset(server_message,'\0',sizeof(server_message));
		    memset(client_message,'\0',sizeof(client_message));
		    
		    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
		    
		    if(socket_desc < 0){
            	printf("Erro ao criar o socket\n");
            	return -1;
        	}
		    
        	printf("Socket criado com sucesso!\n");
		    
		    server_addr.sin_family = AF_INET;
		    server_addr.sin_port = htons(2000);
		    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		    
			setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
			if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
			            printf("CLIENTE: Não foi possível conectar\n");
				        return -1;
				    }
		    printf("CLIENTE: Conectado ao servidor!\n");
			
			for (int i = 0; i <= falta_gerar; i++){
				if(i == falta_gerar){
			        printf("CLIENTE: Máximo de iterações, finalizando...\n");
					sprintf(client_message, "%d", 0);
				}
			    else{
			        printf("CLIENTE: Número para enviar ao servidor: ");
				    gets(client_message);
				}
			    
			    if(send(socket_desc, client_message, strlen(client_message), 0) < 0){
		            printf("CLIENTE: Incapaz de enviar a mensagem\n");
			        return -1;
			    }
		        printf("CLIENTE: Mensagem enviada\n");
			    
			    if(recv(socket_desc, server_message, sizeof(server_message), 0) < 0){
		            printf("CLIENTE: Erro ao receber a mensagem\n");
			        return -1;
			    }
		        if(i != falta_gerar) printf("CLIENTE: Mensagem do servidor: %s\n",server_message);
			}
		    
		    close(socket_desc);
		}

  return 0;

}
