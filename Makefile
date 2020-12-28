nonblock:
	gcc -o nonblock -pthread NonBlocking_2019472.c
	./nonblock 5

block:
	gcc -o block -pthread Blocking_2019472.c
	./block 5

	