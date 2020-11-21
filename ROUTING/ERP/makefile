all:
	sudo gcc -o r1_route r1_route.c -ljansson
	sudo gcc -o r2_route r2_route.c -ljansson
	gcc -o libconfig libconfig.c -ljansson
run:
	nohup sudo ip netns exec R1 ./r1_route r1.json 2>&1 | tee -a /tmp/R1.log &
	nohup sudo ip netns exec R2 ./r2_route r2.json 2>&1 | tee -a /tmp/R2.log & 
kill:
	pgrep -f r1_route |sudo xargs kill -9
	pgrep -f r1_route |sudo xargs kill -9
check:
	ps aux | grep topo2 | grep -v grep
flush_route:
	ip netns exec R1 ip route del 10.3.0.0/24 || true
	ip netns exec R1 ip route del 10.4.0.0/24 || true
	ip netns exec R2 ip route del 10.1.0.0/24 || true
	ip netns exec R2 ip route del 10.2.0.0/24 || true
