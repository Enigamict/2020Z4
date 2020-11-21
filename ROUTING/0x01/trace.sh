# !/bin/bash
set -eu
set -x

ip netns exec C1 tracepath -n 10.3.0.2
ip netns exec C1 tracepath -n 10.2.0.2
ip netns exec C1 tracepath -n 10.2.0.1
ip netns exec C1 tracepath -n 10.4.0.2
ip netns exec C1 tracepath -n 10.3.0.1
ip netns exec C1 tracepath -n 10.4.0.1

# R3 R4
ip netns exec C1 tracepath -n 10.255.2.2
ip netns exec C1 tracepath -n 10.255.2.1

ip netns exec C1 tracepath -n 10.255.4.2
ip netns exec C1 tracepath -n 10.255.4.1

ip netns exec C1 tracepath -n 10.255.5.2 #
ip netns exec C2 tracepath -n 10.255.5.1 #

ip netns exec C2 tracepath -n 10.255.3.2 #
ip netns exec C1 tracepath -n 10.255.3.1 # この4つが帰ってこない

# R1 R2
ip netns exec C1 tracepath -n 10.255.1.2
ip netns exec C1 tracepath -n 10.255.1.1