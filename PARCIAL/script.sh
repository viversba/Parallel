#!/bin/bash
echo "@@@------INICIO DEL PROGRAMA------@@@" &>>  Output.txt 
echo "1. Caso tamaño 8" &>>  Output.txt 
(time ./p2−matrixMult_cpu−omp 8 8)  &>>  Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "2. Caso tamaño 16" &>>  Output.txt 
(time ./p2−matrixMult_cpu−omp 16 16)  &>>  Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "3. Caso tamaño 32" &>>  Output.txt 
(time ./p2−matrixMult_cpu−omp 32 32) &>> Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "4. Caso tamaño 64" &>>  Output.txt 
(time ./p2−matrixMult_cpu−omp 64 64) &>> Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "5. Caso tamaño 128" &>>  Output.txt 
(time ./p2−matrixMult_cpu−omp 128 128) &>> Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "6. Caso tamaño 256" &>>  Output.txt 
(time ./p2−matrixMult_cpu−omp 256 256) &>> Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "7. Caso tamaño 512" &>>  Output.txt 
(time ./p2−matrixMult_cpu−omp 512 512) &>> Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo " 8. Caso tamaño 1024" &>>  Output.txt 
(time ./p2−matrixMult_cpu−omp 1024 1024) &>> Output.txt 
echo "@@@-------FIN DEL PROGRAMA--------@@@" &>>  Output.txt 
