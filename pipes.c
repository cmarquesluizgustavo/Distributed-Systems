#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <math.h>

#define MAXBUFFER 20


pid_t pid;
int industriaPIPE[2];


int gerador(){
  // --> gera o delta entre 1 e 100 que será adicionado ao ni
  srand(time(NULL));
  int r_numb = rand() % 100 + 1;
  return r_numb;
}

void receptor(int ni){
    for (int i = 2; i <= sqrt(ni); i++) {
        if (ni % i == 0) {
            printf("O %d não é primo.\n", ni);
            return;
        }
    }
    if (ni <= 1) printf("O %d é primo. WOW!\n", ni);
    else printf("O %d é primo. WOW!\n", ni);
}

int main( int argc, char *argv[] ) {
  int falta_gerar;
  falta_gerar = atoi(argv[1]);
  printf("Serão gerados %d números hoje\n", falta_gerar);
  pipe(industriaPIPE);
  pid = fork();

  //filho tem pid == 0 e do pai é > 0
  if (pid > 0){
    
    close(industriaPIPE[0]);
    char industriaBUFFER[MAXBUFFER];
    int ni = 1;
    
    while(falta_gerar > 0){
      sprintf(industriaBUFFER, "%d", ni);
      write(industriaPIPE[1], industriaBUFFER, MAXBUFFER);
      printf("Objeto '%s' na linha de montagem\n", industriaBUFFER);
      ni += gerador();
      falta_gerar--;
      sleep((random() % 2) * 0.000001); // permite que consumidor entre em ação antes de toda a produção ser feita
    }
    printf("PARANDO A PRODUÇÃO\n");
    sprintf(industriaBUFFER, "%d", 0);
    write(industriaPIPE[1], industriaBUFFER, MAXBUFFER);
    exit(0);
  } else if(pid==0) {
    close(industriaPIPE[1]);
    char industriaBUFFER[MAXBUFFER];
    read(industriaPIPE[0],industriaBUFFER,MAXBUFFER);
    while(atoi(industriaBUFFER) != 0){
      printf("Consumindo objeto '%s'...\n", industriaBUFFER);
      receptor(atoi(industriaBUFFER));
      read(industriaPIPE[0], industriaBUFFER, MAXBUFFER);
      sleep((random() % 2) * 0.000001) ;
    }
    printf("PARANDO O CONSUMO\n");
    exit(0);
    
  }

  return 0;

}
