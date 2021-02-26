sim: shell.c sim.c isa.c
	gcc -std=gnu99 -g -O2 $^ -o $@

.PHONY: clean
clean:
	rm -rf *.o *~ sim sim.dSYM

