.PHONY: docs

all:
	gcc -Ofast -Wall -Wextra -Werror -pedantic -o soundwave soundwave.c -lm

docs:
	gcc -Ofast -Wall -Wextra -Werror -pedantic -o soundwave soundwave.c -lm
	doxygen Doxyfile

free:
	gcc -Ofast -o soundwave soundwave.c -lm