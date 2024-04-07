/*
Parâmetros: matriz alocada como um vetor e quantidade de linhas dela
Saída: printa a matriz no terminal, retorna void
*/


#include <stdio.h>


void imprimeMat(float* mat, int dim){
    for (int i=0; i<dim; i++){
        for (int j=0; j<dim; j++){
            printf("%.2f ", mat[i * dim + j]);
        }
        printf("\n");
    }
    printf("\n");
}