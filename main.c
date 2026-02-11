#include "counter.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define CLS 64 //cache line size
#define L1_SIZE 3 * 1024 * 1024 //3 MB
#define L2_SIZE 80 * 1024 * 1024 //80 MB

int main(int argc, char *argv[]) {
    
    if(argc != 3) {
        printf("Uso: %s <D>, <L>\n", argv[0]);
        return 1;
    }

    int D = atoi(argv[1]);
    float L = atof(argv[2]);


    int R = L;
    if (D < CLS / sizeof(double)) {
        int R = L1_SIZE / (D * sizeof(double)); //calcular R para que el acceso a A ocupe toda la cache L1   
    }


    int S1 = L1_SIZE / CLS; 
    int S2 = L2_SIZE / CLS; //80 MB  

    //iniciar vector indices
    int * indices = (int *)malloc(R * sizeof(int));    

    for(int i = 0; i < R; i++) {
        indices[i] = i * D;
    }

    // 3. Calcular tamaño real necesario para A
    // El último elemento que tocaremos está en la posición (R-1)*D
    // Usamos size_t para evitar desbordamiento de enteros en arrays gigantes
    size_t max_indice = (size_t)(R - 1) * D;
    size_t num_bytes = (max_indice + 1) * sizeof(double);
    
    // Ajustar a múltiplo de CLS para aligned_alloc (requisito de la función)
    if (num_bytes % CLS != 0) {
        num_bytes += CLS - (num_bytes % CLS);
    }

    double * A = (double *)aligned_alloc(CLS, num_bytes); // A es un array de doubles, alineado a CLS, con el tamaño necesario para R accesos con stride D
    if(A == NULL) {
        free(indices);
        fprintf(stderr, "Error: No se pudo asignar memoria para A\n");
        return 1;
    }


    //inicializar A
    //Inicializar la semilla para que el número cambie en cada ejecución
    srand(time(0));

    // Solo inicializamos lo que vamos a tocar para ganar tiempo en D grandes
    // Ojo: Si quieres ser estricto, inicializa todo el vector, pero tardará mucho con D grande.
    for(int i = 0; i < R; i++) {
        A[indices[i]] = (double)rand() / RAND_MAX * 100.0;
    }

    double S[10] = {0};

    start_counter(); 

    for(int i = 0; i < 10; i++) {
        
        double sol = 0.0;

        for(int j = 0; j < R; j++) {
            sol += A[indices[j]];
        }

        S[i] = sol;

    }

    double ciclos_acceso = (double) get_counter() / (R*10);
    
    printf("%d\t%d\t%.4f\n", D, L, ciclos_acceso);

    free(A);
    free(indices);
    
    return 0;
}