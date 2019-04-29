#!/bin/bash
echo "@@@------INICIO DEL PROGRAMA------@@@" &>>  Output.txt 
echo "1. Caso imagen 720p secuencial" &>>  Output.txt 
(time ./blur-effect 720p.png 720p_out1.png 1 15)  &>>  Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "2. Caso imagen 720p 2 hilos" &>>  Output.txt 
(time ./blur-effect 720p.png 720p_out2.png 2 15)  &>>  Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "3. Caso imagen 720p 4 hilos" &>>  Output.txt 
(time ./blur-effect 720p.png 720p_out4.png 4 15) &>> Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "4. Caso imagen 720p 8 hilos" &>>  Output.txt 
(time ./blur-effect 720p.png 720p_out8.png 8 15) &>> Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "5. Caso imagen 720p 16 hilos" &>>  Output.txt 
(time ./blur-effect 720p.png 720p_out16.png 16 15) &>> Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "6. Caso imagen 1080p secuencial" &>>  Output.txt 
(time ./blur-effect 1080p.png 1080p_out1.png 1 15) &>> Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "7. Caso imagen 1080p 2 hilos" &>>  Output.txt 
(time ./blur-effect 1080p.png 1080p_out2.png 2 15) &>> Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo " 8. Caso imagen 1080p 4 hilos" &>>  Output.txt 
(time ./blur-effect 1080p.png 1080p_out4.png 4 15) &>> Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "9. Caso imagen 1080p 8 hilos" &>>  Output.txt 
(time ./blur-effect 1080p.png 1080p_out8.png 8 15) &>> Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo" 10. Caso imagen 1080p 16 hilos" &>>  Output.txt 
(time ./blur-effect 1080p.png 1080p_out16.png 16 15) &>> Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "11. Caso imagen 4k secuencial" &>>  Output.txt 
(time ./blur-effect 4k.png 4k_out1.png 1 15) &>> Output.txt 
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "12. Caso imagen 4k 2 hilos" &>>  Output.txt 
(time ./blur-effect 4k.png 4k_out2.png 2 15) &>> Output.txt  
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "13. Caso imagen 4k 4 hilos" &>>  Output.txt 
(time ./blur-effect 4k.png 4k_out4.png 4 15) &>> Output.txt 
echo "14. Caso imagen 4k 8 hilos" &>>  Output.txt 
(time ./blur-effect 4k.png 4k_out8.png 8 15) &>> Output.txt  
echo "@@@-------------------------------@@@" &>>  Output.txt 
echo "15. Caso imagen 4k 16 hilos" &>>  Output.txt 
(time ./blur-effect 4k.png 4k_out16.png 16 15) &>> Output.txt 
echo "@@@-------FIN DEL PROGRAMA--------@@@" &>>  Output.txt 

