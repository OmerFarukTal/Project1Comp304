
run: clean a.out
	./a.out

a.out:
	gcc shellax-skeleton.c

clean:
	rm ./a.out

write:
	vim shellax-skeleton.c
	
dummy:
	rm ./dummy.out
	gcc dummy.c -o dummy.out
	./dummy.out

make a:
	touch ./a.out
