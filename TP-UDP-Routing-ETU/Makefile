CC  = gcc
FLAGS = -Wall -pedantic -std=gnu99
INC = -I.

OBJ_TR = tabrout.o

OBJ_COMMON = $(OBJ_TR)

OBJ_REM = routPem.o
OBJ_RREC = routPrec.o

OBJ_RPEER = routP.o

#tpEmRec : uniquement un échange du processus émetteur vers l'autre récepteur
tpEmRec: $(OBJ_COMMON) $(OBJ_REM) $(OBJ_RREC)
	$(CC) -o routPem $(OBJ_COMMON) $(OBJ_REM)
	$(CC) -o routPRec $(OBJ_COMMON) $(OBJ_RREC)

#tpEmRec : une seule entité pour être à la fois émetteur et récepteur
tpPeer: $(OBJ_COMMON) $(OBJ_RPEER)
	$(CC) -o routP $(OBJ_COMMON) $(OBJ_RPEER)

# '%' matches filename
# $@  for the pattern-matched target
# $<  for the pattern-matched dependency
%.o: %.c
	$(CC) $(FLAGS) -o $@ -c $< $(INC)

clean:
	rm -f *.o routPem routPRec routP
