obj-m += mymodule2.o


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
	
module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

cleanmodule:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

prepare:
	gcc ./uniq.c -o ./uniq.out
	gcc ./chatroom.c -o ./chatroom.out
	gcc ./saat.c -o ./saat.out
	
cleanprep:
	rm uniq.out chatroom.out saat.out
