#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include <time.h>

int nthreads; //Número de threads
int *id; //Identificador de thread
int dim; //Dimensão da matriz
float* matA;
float* matB;
float* saida;

typedef struct{
    int id;
    int dim;
} tArgs;

void * thread(void * arg){

    tArgs* args = (tArgs*) arg;

    for(int i = args->id; i < args->dim; i += nthreads){
        for( int j = 0; j<args->dim; j++){
            for(int k = 0; k<args->dim; k++){
                saida[i*(args->dim)+j] += matA[i*(args->dim)+k] * matB[k*(args->dim)+j];
            }
        }
    }

    pthread_exit(NULL);
}

int main(int argc , char* argv[]){

    pthread_t *tid;
    tArgs* args;
    float* matTeste;
    clock_t inicio, fim;
    double delta;

    //Início da medição do tempo de inicialização
    inicio = clock();

    //Check de input
    if(argc < 3){
        printf("A entrada deve ser do tipo:  ./'nomeDoPrograma' <dimensao da matriz> <numero de threads>");
        return 1;
    }

    //Armazenando inputs
    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);

    //Check de inutilização de threads
    if (nthreads > dim){
        nthreads = dim;
    }

    //Alocação da memória para ponteiros do programa com checks de erro
    matA  = (float *) malloc(sizeof(float)*dim*dim);
    if (matA == NULL){
        printf("ERRO -- malloc\n");
        return 2;
    }

    matB = (float *) malloc(sizeof(float)*dim*dim);
    if (matB == NULL){
        printf("ERRO -- malloc\n");
        return 2;
    }

    saida = (float *) malloc(sizeof(float)*dim*dim);
    if (saida == NULL){
        printf("ERRO -- malloc\n");
        return 2;
    }

    matTeste = (float *) malloc(sizeof(float)*dim*dim);
    if (saida == NULL){
        printf("ERRO -- malloc\n");
        return 2;
    }

    //Alocação de memória dos ponteiros que se referem as threads com checks de erro
    tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
    if (tid == NULL){
        printf("ERRO -- malloc\n");
        return 2;
    }

    args = (tArgs*)malloc(sizeof(tArgs)*nthreads);
    if(args == NULL){
        puts("ERROR === 'malloc'\n");
        return 2;
    }

    //Preenchimento dos elementos de forma a medir a distância até o primeiro elemento da matriz no valor da célula em questão
    for(int i = 0; i < dim; i++){
        for(int j = 0; j < dim; j++){
            matA[i*dim+j] = i + j;
            matB[i*dim+j] = i + j;
            saida[i*dim+j] = 0; //Preenchendo a matriz de saída com zeros incialmente
            matTeste[i*dim+j] = 0;
        }
    }

    //Fim da medição do tempo de inicialização
    fim = clock();
    delta = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("Tempo de inicializacao: %lf\n",delta);

    //Início da medição do tempo de multiplicação
    inicio = clock();

    //Criando as threads
    for(int i = 0; i < nthreads; i++){

        (args+i)->id = i;
        (args+i)->dim = dim;

        if(pthread_create(tid+i,NULL,thread,(void*)(args+i))){
            printf("ERRO -- pthread_create");
            return 3; 
        }
    }

    //Esperando a finalização das threads
    for(int i = 0; i < nthreads; i++){
        pthread_join(*(tid+i), NULL);
    }

    //Fim da medição do tempo de multiplicação
    fim = clock();
    delta = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("Tempo de multiplicacao: %lf\n",delta);
    
    //Verificação do resultado concorrente comparando com o sequencial
    for(int i = 0; i < dim; i++){
        for(int j = 0; j < dim; j++){
            for(int k = 0; k < dim; k++){
                matTeste[i*dim+j] += matA[i*dim+k] * matB[k*dim+j];
            }
            if (matTeste[i*dim+j] != saida[i*dim+j]){
                printf("ERRO -- incompatibilidade entre forma concorrente e sequencial");
                return 4;
            }
        }
    }

    printf("Confirmada compatibilidade entre forma concorrente e sequencial!\n");

    //Prints para visualização
    /*
    for(int i=0;i<dim;i++){
        for(int j=0;j<dim;j++){
            printf("%lf ", matA[i*dim+j]);
        }
        printf("\n");
    }
    printf("\n\n");
    for(int i=0;i<dim;i++){
        for(int j=0;j<dim;j++){
            printf("%lf ", matB[i*dim+j]);
        }
        printf("\n");
    }
    printf("\n\n");
    for(int i=0;i<dim;i++){
        for(int j=0;j<dim;j++){
            printf("%lf ",saida[i*dim+j]);
        }
        printf("\n");
    }
    printf("\n\n");
    for(int i=0;i<dim;i++){
        for(int j=0;j<dim;j++){
            printf("%lf ",matTeste[i*dim+j]);
        }
        printf("\n");
    }
    printf("\n\n");
    */

    //Início da medição do tempo de finalização
    inicio = clock();

    //Liberação de memória
    free (matA);
    free (matB);
    free (saida);
    free (matTeste);
    free (tid);
    free (args);

    //Fim da medição do tempo de finalização
    fim = clock();
    delta = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("Tempo de finalizacao: %lf\n",delta);

    return 0;
}
