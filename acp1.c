#include "counter.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define CLS 64 //cache line size

// Tamaños en bytes por NÚCLEO
#define L1_SIZE 49152    // 48 KB
#define L2_SIZE 1310720  // 1.25 MB

int main(int argc, char *argv[]) {
    
    if(argc != 3) {
        printf("Uso: %s <D>, <L>\n", argv[0]);
        return 1;
    }

    int D = atoi(argv[1]);
    int L = atoi(argv[2]); 

    int elementos_por_linea = CLS / sizeof(double); // Esto da 8
    int R;
    
    if (D < elementos_por_linea) {
        R = L * (elementos_por_linea / D); 
    } else {
        R = L;
    }

    int S1 = L1_SIZE / CLS; // 49152 / 64 = 768 líneas
    int S2 = L2_SIZE / CLS; // 1310720 / 64 = 20480 líneas

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
    for(int i = 0; i < R; i++) {
        // Genera un valor entre 1.0 y 2.0
        double val = 1.0 + ((double)rand() / RAND_MAX);
        // Aleatoriamente lo hace positivo o negativo
        if (rand() % 2 == 0) {
            val = -val;
        }
        A[indices[i]] = val;
    }

    double S[10] = {0};

    double basura = 0.0;
    for(int j = 0; j < R; j++) {
        basura += A[indices[j]];
    }

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

    // Imprimir S para evitar optimizaciones indeseadas del compilador
    printf("Resultados S: ");
    for(int i = 0; i < 10; i++) {
        printf("%.2f ", S[i]);
    }
    printf("\n");

    free(A);
    free(indices);
    
    return 0;
}