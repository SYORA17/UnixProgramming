.PHONY: clean all
mydhcpc: mydhcpc.c
	gcc -o mydhcpc mydhcpc.c list.c
mydhcps: mydhcps.c
	gcc -o mydhcps mydhcps.c list.c
all:
	gcc -o mydhcpc mydhcpc.c list.c
	gcc -o mydhcps mydhcps.c list.c
clean:
	rm mydhcpc mydhcps
