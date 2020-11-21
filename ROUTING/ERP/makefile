all:
	sudo gcc -o r1_route r1_route.c -ljansson
	sudo gcc -o r2_route r2_route.c -ljansson
	gcc -o libconfig libconfig.c -ljansson
run:
	nohup sudo ip netns exec R1 ./topo2keps r1.json 2>&1 | tee -a /tmp/R1.log &
	nohup sudo ip netns exec R2 ./topo2kepc r2.json 2>&1 | tee -a /tmp/R2.log & 
kill:
	pgrep -f topo2keps |sudo xargs kill -9
	pgrep -f topo2kepc |sudo xargs kill -9
check:
	ps aux | grep topo2 | grep -v grep
