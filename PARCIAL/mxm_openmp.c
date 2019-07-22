
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

int main(int argc, char* argv[]) 
{
	int tam = atoi(argv[1]);
	printf("%d",tam);
	int A[tam][tam];
	int B[tam][tam];
	int C[tam][tam];
    int i,j,k;
    omp_set_num_threads(omp_get_num_procs());
    for (i= 0; i< tam; i++)
        for (j= 0; j< tam; j++)
	{
            A[i][j] = 2;
            B[i][j] = 2;
	}
    #pragma omp parallel for private(i,j,k) shared(A,B,C)
    for (i = 0; i < tam; ++i) {
        for (j = 0; j < tam; ++j) {
            for (k = 0; k < tam; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

}
 