CC=gcc
CFLAGS=-mavx -mavx2 -Wall -Wextra

comp:
	$(CC) -o math_simul_exo_49 math_simul_exo_49_carres_xorshift.c $(CFLAGS)
	./math_simul_exo_49

db:
	$(CC) -o math_simul_exo_49 math_simul_exo_49_carres_xorshift.c $(CFLAGS) -g
	gdb math_simul_exo_49