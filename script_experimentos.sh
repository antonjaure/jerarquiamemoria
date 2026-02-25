#!/bin/bash
# Solicitamos un nodo con 64 cores y 256 GB de memoria durante 2 horas
#SBATCH -n 1 -c 64 -t 01:00:00 --mem=8G
# Ponemos nombre a nuestro trabajo para poder identificarlo.
# ATENCIÓN - Debes sustituir el NN por el número de equipo.
#SBATCH --job-name p1acgcurso1003

# Sustituir los valores de Di y Li por los calculados para la realización de la práctica.

gcc main.c -o acp1 -O0

for i in {1..10}
do
	for D in {1,8,64,512,4096}
	do
		for L in {384,1152,10240,15360,40960,81920,163840}
		do
			./acp1 $D $L
		done
	done
done


