
run: clean shellax.out
	./shellax.out

shellax.out:
	gcc shellax-skeleton.c -o shellax.out

clean:
	rm ./shellax.out

write:
	vim shellax-skeleton.c
	
dummy:
	rm ./dummy.out
	gcc dummy.c -o dummy.out
	./dummy.out

make a:
	touch ./shellax.out
