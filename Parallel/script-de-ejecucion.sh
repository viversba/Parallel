#!/bin/bash

#1. Caso imagen 720p secuencial
{time ./blur-effect 720p.png 720p_out1.png 1 15}  &>>  Output.txt 
#2. Caso imagen 720p 2 hilos
time ./blur-effect 720p.png 720p_out2.png 2 15  &>>  Output.txt 
#3. Caso imagen 720p 4 hilos
time ./blur-effect 720p.png 720p_out4.png 4 15 &>> Output.txt 
#4. Caso imagen 720p 8 hilos
time ./blur-effect 720p.png 720p_out8.png 8 15 &>> Output.txt 
#5. Caso imagen 720p 16 hilos
time ./blur-effect 720p.png 720p_out16.png 16 15 &>> Output.txt 
#6. Caso imagen 1080p secuencial
time ./blur-effect 1080p.png 1080p_out1.png 1 15 &>> Output.txt 
#7. Caso imagen 1080p 2 hilos
time ./blur-effect 1080p.png 1080p_out2.png 2 15 &>> Output.txt 
#8. Caso imagen 1080p 4 hilos
time ./blur-effect 1080p.png 1080p_out4.png 4 15 &>> Output.txt 
#9. Caso imagen 1080p 8 hilos
time ./blur-effect 1080p.png 1080p_out8.png 8 15 &>> Output.txt 
#10. Caso imagen 1080p 16 hilos
time ./blur-effect 1080p.png 1080p_out16.png 16 15 &>> Output.txt 
#11. Caso imagen 4k secuencial
time ./blur-effect 4k.png 4k_out1.png 1 15 &>> Output.txt 
#12. Caso imagen 4k 2 hilos
time ./blur-effect 4k.png 4k_out2.png 2 15 &>> Output.txt  
#13. Caso imagen 4k 4 hilos
time ./blur-effect 4k.png 4k_out4.png 4 15 &>> Output.txt 
#14. Caso imagen 4k 8 hilos
time ./blur-effect 4k.png 4k_out8.png 8 15 &>> Output.txt  
#15. Caso imagen 4k 16 hilos
time ./blur-effect 4k.png 4k_out16.png 16 15 &>> Output.txt 

