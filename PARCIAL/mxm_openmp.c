#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main(int argc, char* argv[]){
	
int size = atoi(argv[1]);
int first[size][size];
int second[size][size];
int third[size][size];
int i,j,k;
srand(time(NULL));
for( i=0; i < size; i++){
	for( j=0; j<size; j++){
		first[i][j] = rand();
    second[i][j] = rand();
  }
} 

#pragma omp parallel for private(j,k)
	for( i=0; i<size; i++ ){
		for( j=0; j<size; j++ ){
			for( k=0; k<size; k++){
				third[i][j] = first[i][j] + first[i][k]*second[k][j];
			}
		}
	}	

for( i=0; i < size; i++){
	for( j=0; j<size; j++){
		printf("%d ", third[i][j]);
  }
} 

return 0;
}
