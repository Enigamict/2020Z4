all:
	sudo gcc -o r1r2_route r1r2_route.c -ljansson
	sudo gcc -o r3_route r3_route.c -ljansson
	gcc -o libconfig libconfig.c -ljansson
run:
	nohup sudo ip netns exec R3 ./r3_route r3.json 2>&1 | tee -a /tmp/R3.log &
	sleep 3
	nohup sudo ip netns exec R2 ./r1r2_route r2.json 2>&1 | tee -a /tmp/R2.log & 
	nohup sudo ip netns exec R1 ./r1r2_route r1.json 2>&1 | tee -a /tmp/R1.log &
kill:
	pgrep -f r1_route |sudo xargs kill -9
	pgrep -f r2_route |sudo xargs kill -9
	pgrep -f r3_route |sudo xargs kill -9
check:
	ps aux | grep topo5 | grep -v grep
flush_route:
	sudo ip netns exec R3 ip route del 10.1.0.0/24 || true
	sudo ip netns exec R3 ip route del 10.2.0.0/24 || true
	sudo ip netns exec R3 ip route del 10.3.0.0/24 || true
	sudo ip netns exec R3 ip route del 10.4.0.0/24 || true
list_route:
	sudo ip netns exec R1 ip route
	sudo ip netns exec R2 ip route
	sudo ip netns exec R3 ip route
