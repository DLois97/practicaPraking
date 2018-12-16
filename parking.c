#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int **parking;
int plazas;
int plantas;
int nPlazasLib;
int nCoches;
int nCamiones;
pthread_cond_t espera;
pthread_mutex_t mutex;

int aparcarCoche(int nReferenciaCoche){
	int i;
	int j;
	for (i = 0; i < plantas; i++){
		for (j = 0; j < plazas; j++){
			if (parking[i][j]==-1){
				parking[i][j] = nReferenciaCoche;
				nPlazasLib--;
				printf("ENTRADA: El coche %d ha aparcado en la planta %d plaza %d. Plazas Libres: %d.\n",nReferenciaCoche,i,j,nPlazasLib);
				return 1;
			}
		}
	}
	return 0;
}
int aparcarCamion(int nReferenciaCamion){
	int i;
	int j;
	for (i = 0; i < plantas; i++){
		for (j = 0; j < plazas-1; j++){
			if ((parking[i][j]==-1)&&(parking[i][j+1]==-1)){
				parking[i][j] = nReferenciaCamion;
				parking[i][j+1] = nReferenciaCamion;
				nPlazasLib--;
				nPlazasLib--;
				printf("ENTRADA: El camion %d ha aparcado en la planta %d plaza %d y plaza %d. Plazas Libres: %d.\n",nReferenciaCamion,i,j,j+1,nPlazasLib);
				return 1;
			}
		}
	}
	return 0;
}

void *entrada_coches(void *idCoche){
	int coche_id = *(int *)idCoche;	
	int i;
	int j;
	while(1){
		sleep((rand()%10)+1);
		pthread_mutex_lock(&mutex);
		while(!aparcarCoche(coche_id)){
			pthread_cond_wait(&espera,&mutex);
		}
		for(i=0; i<plantas; i++){
			printf("PLANTA %d:",i);
			for(j=0; j<plazas; j++){
				printf("[%d]",parking[i][j]);
			}
			printf("\n");
		}
		pthread_mutex_unlock(&mutex);
		sleep((rand()%10)+1);
		pthread_mutex_lock(&mutex);
		for (i = 0; i < plantas; i++){
			for (j = 0; j < plazas; j++){
				if (parking[i][j]==coche_id){
					parking[i][j] = -1;
					nPlazasLib++;
					printf("SALIDA: Coche %d saliendo. Plazas Libres: %d.\n",coche_id,nPlazasLib);
				}
			}
		}
		for(i=0; i<plantas; i++){
			printf("PLANTA %d:",i);
			for(j=0; j<plazas; j++){
				printf("[%d]",parking[i][j]);
			}
			printf("\n");
		}
		pthread_cond_broadcast(&espera);
		pthread_mutex_unlock(&mutex);		
	}
}
void *entrada_camion(void *idCamion){
	int camion_id = *(int *)idCamion;
	int i;
	int j;
	while(1){
		sleep((rand()%10)+1);
		pthread_mutex_lock(&mutex);
		while(!aparcarCamion(camion_id)){
			pthread_cond_wait(&espera,&mutex);
		}
		for(i=0; i<plantas; i++){
			printf("PLANTA %d:",i);
			for(j=0; j<plazas; j++){
				printf("[%d]",parking[i][j]);
			}
			printf("\n");
		}
		pthread_mutex_unlock(&mutex);
		sleep((rand()%10)+1);
		pthread_mutex_lock(&mutex);
		for (i = 0; i < plantas; i++){
			for (j = 0; j < plazas-1; j++){
				if (parking[i][j]==camion_id){
					parking[i][j] = -1;
					parking[i][j+1] = -1;
					nPlazasLib++;
					nPlazasLib++;
					printf("SALIDA: Camion %d saliendo. Plazas Libres: %d.\n",camion_id,nPlazasLib);
				}
			}
		}
		for(i=0; i<plantas; i++){
			printf("PLANTA %d:",i);
			for(j=0; j<plazas; j++){
				printf("[%d]",parking[i][j]);
			}
			printf("\n");
		}
		pthread_mutex_unlock(&mutex);		
	}
}
int main(int argc, char *argv[]){
	int i;
	int j;
	int *coches;
	int *camiones;
	pthread_t *thCoches;
	pthread_t *thCamiones;
	//Iniciamos todos los datos con los argumentos de entrada.
	if ((argc < 3)||(argc > 5)) {
		fprintf(stderr,"ERROR: hay que introducir minimo 2 argumentos, máximo 4 argumentos: \n PLAZAS PLANTAS - PLAZAS PLANTAS COCHES - PLAZAS PLANTAS COCHES CAMIONES\n");
		exit(1);
	}
	if (argc == 3){
		plazas = atoi(argv[1]);
		plantas = atoi(argv[2]);
		nCoches = 2*plazas*plantas;
		nCamiones = 0;
	}
	if (argc == 4){
		plazas = atoi(argv[1]);
		plantas = atoi(argv[2]);
		nCoches = atoi(argv[3]);
		nCamiones = 0;
	}
	if (argc == 5){
		plazas = atoi(argv[1]);
		plantas = atoi(argv[2]);
		nCoches = atoi(argv[3]);
		nCamiones = atoi(argv[4]);
	}
	parking=malloc(sizeof(int*)*plantas);
	nPlazasLib = plantas*plazas;
	thCoches=malloc(sizeof(pthread_t)*nCoches);
	thCamiones=malloc(sizeof(pthread_t)*nCamiones);
	coches = malloc(sizeof(int)*nCoches);
	camiones = malloc(sizeof(int)*nCamiones);
	for(i=0;i<plantas;i++){
		
		parking[i]=malloc(sizeof(int)*plazas);
	}
	for(i=0;i<plantas;i++){
		for(j=0;j<plazas;j++){
			parking[i][j]=-1;
		}
	}
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&espera,NULL);
	//Creamos un thread por cada coche y camión que hay.
	for (i = 0; i < nCoches; i++){		
		coches[i] = i;
		pthread_create(&thCoches[i],NULL,entrada_coches,(void*) &coches[i]);
	}
	for (i = 0; i < nCamiones; i++){
		
		camiones[i] = i+100;
		pthread_create(&thCamiones[i],NULL,entrada_camion,(void*) &camiones[i]);
	}
	
	//Cerramos todos los threads previamente creados.
	for (i = 0; i < nCoches; i++){
		pthread_join(thCoches[i],NULL);
	}
	for (i = 0; i < nCamiones; i++){
		pthread_join(thCamiones[i],NULL);
	}
	
	//liberar el espacio ocupado por el array de coches y el de threads de cada coche.
	free(coches);
	free(thCoches);
	//liberamos el espacio ocupado por el array de camiones y el de threads de cada camión.
	free(camiones);
	free(thCamiones);
	//Destruimos la condición de espera de la entrada de camiones.
	pthread_cond_destroy(&espera);
	//Destruimos el mutex.
	pthread_mutex_destroy(&mutex);
}
