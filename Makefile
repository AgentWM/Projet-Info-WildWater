#Compilateur
CC = gcc

#Option du compilateur
CFLAGS = -Wall -Wextra -g -D_POSIX_C_SOURCE=200809L -O2

# Fichiers communs
COMMON_SRC = AVL_USINES.c CSV.c leaks.c network.c

# Les 2 mains selon histo ou leaks
SRC_AVL = main_avl.c $(COMMON_SRC)
SRC_FUITES = main_fuites.c $(COMMON_SRC)

# tr .c en .o
OBJ_AVL = $(SRC_AVL:.c=.o)
OBJ_FUITES = $(SRC_FUITES:.c=.o)

#Exécution make de base
all: exec_AVL exec_fuites

#créa exécutable
exec_AVL: $(OBJ_AVL)
	$(CC) $(CFLAGS) -o $@ $^

exec_fuites: $(OBJ_FUITES)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#suppression des fichiers générés avec make clean
clean:
	rm -f *.o exec_AVL exec_fuites vol_max.dat vol_captation.dat vol_traitement.dat vol_max10.png vol_min50.png leaks.dat
