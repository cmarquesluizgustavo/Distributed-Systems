#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>


int toRun = 3;
int mustRun = 0;

void signal_handler(int signal){ 
    //Essa função manipula os sinais passados ao processo para alterar a variável toRun, que determina se o programa deve ser encerrado ou qual rotina deve ser executada
    if (signal == SIGUSR1) toRun = 1;
    if (signal == SIGUSR2) toRun = 2;
    if (signal == SIGHUP)  toRun = 3;
    if (signal == SIGTERM) toRun = 0;
    mustRun = 1; //Essa variável garante que cada tarefa rode apenas uma vez a cada vez que receber um sinal
    return;
}


int main( int argc, char *argv[] ){
	
    printf("O PID do processo é %d.\n", getpid());
    
	if (strcmp(argv[1], "rating_receiver") == 0){

		int wait_type;
		wait_type = atoi(argv[2]);
		printf("Pode-se escolher o tipo de espera passando 0 (busy) ou 1 (blocking) para a função\n");
		printf("O tipo de espera escolhido foi o de: %d\n", wait_type);
		
		
        while(toRun) { // O loop se manterá enquanto o processo não detectar o sinal SIGTERM. As 3 primeiras lidam com problemas ao manipular 
            if (signal(SIGUSR1, signal_handler) == SIG_ERR) printf("Algum problema ocorreu ao manipular o sinal SIGUSR1.\n");
            if (signal(SIGUSR2, signal_handler) == SIG_ERR) printf("Algum problema ocorreu ao manipular o sinal SIGUSR2.\n" );
            if (signal(SIGHUP,  signal_handler) == SIG_ERR) printf("Algum problema ocorreu ao manipular o sinal SIGHUP.\n"  );
            if (signal(SIGTERM, signal_handler) == SIG_ERR) printf("Algum problema ocorreu ao manipular o sinal SIGTERM.\n");

            //Se for detectado o sinal SIGUSR1:
            if (toRun == 1 && mustRun){
                printf("Recebemos sua avaliação 1 e estamos encaminhando para o setor responsável.\nFicamos muito decepcionados que sua experiência tenha sido tão ruim a ponta de fazer você chorar.\n"); 
                mustRun = 0;
            }
            
            //Se for detectado o sinal SIGUSR2:
            else if (toRun == 2 && mustRun){
                printf("Recebemos sua avaliação 2.\nObrigado por usar nossos serviços, mas na próxima iremos prover meias de melhor qualidade.\n"); 
                mustRun = 0;
            }

            //Se for detectado o sinal SIGHUP:
            else if (toRun == 3 && mustRun){
                printf("Recebemos sua avaliação 3.\nFicamos muito felizes que gostou de nosso serviço! Estamos um pouco preocupados que amou nossos serviços mais que seu conjugê, mas agradecemos.\n"); 
                mustRun = 0;
            }

            //Se for detectado o sinal SIGTERM:
            else if (toRun == 0) {
                printf("Finalizando o disparador...\n");
                return 0;
            }

			//Se o tipo de wait for blocking, suspende a função até detectar um sinal
			if (wait_type){
				pause();
			}
        }
    }

	else if (strcmp(argv[1], "rating_signaler") == 0){
        printf("Olá, eu sou o processo que vai enviar as avaliações que você tiver da loja.\n");
        printf("Lembrando que as notas são: SIGUSR1 (ruim), SIGUSR2 (aceitável), SIGHUP (excepcional) da nossa loja. Também é possível enviar SIGKILL para terminar o outro processo.\n");
        printf("O PID do processo handler recebido foi o %d, e a nota enviada (o sinal) foi o %d.\n", atoi(argv[2]), atoi(argv[3]));
        
		if(kill(atoi(argv[2]), 0) == -1){
            printf("PID inválido.\n");
            return 0;
        }
		printf("Checamos que o PID é válido.\n");
        
		// Enviando o sinal para o processo handler
		if (strcmp(argv[3], "SIGUSR1") == 0){
        	kill(atoi(argv[2]), SIGUSR1); 
		}

		else if (strcmp(argv[3], "SIGUSR2") == 0){
        	kill(atoi(argv[2]), SIGUSR2); 
		}
		
		else if (strcmp(argv[3], "SIGHUP") == 0){
        	kill(atoi(argv[2]), SIGHUP); 
		}

		else if (strcmp(argv[3], "SIGKILL") == 0){
        	kill(atoi(argv[2]), SIGKILL); 
		}
			
		else{
			printf("Nota inválida.\n");
			return 0;
		}

		printf("Nota enviada.\n");
    }
    return 0;
}
