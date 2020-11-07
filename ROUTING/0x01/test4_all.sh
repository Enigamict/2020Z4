# !/bin/bash
set -eux

function ping() {
  ip netns exec $1 ping -c1 10.1.0.1
  ip netns exec $1 ping -c1 10.1.0.2
  ip netns exec $1 ping -c1 10.2.0.1
  ip netns exec $1 ping -c1 10.2.0.2
  ip netns exec $1 ping -c1 10.3.0.1
  ip netns exec $1 ping -c1 10.3.0.2
  ip netns exec $1 ping -c1 10.4.0.1
  ip netns exec $1 ping -c1 10.4.0.2
  ip netns exec $1 ping -c1 10.255.1.1
  ip netns exec $1 ping -c1 10.255.1.2
  ip netns exec $1 ping -c1 10.255.2.1
  ip netns exec $1 ping -c1 10.255.2.2
  ip netns exec $1 ping -c1 10.255.3.1
  ip netns exec $1 ping -c1 10.255.3.2
  ip netns exec $1 ping -c1 10.255.4.1
  ip netns exec $1 ping -c1 10.255.4.2
  ip netns exec $1 ping -c1 10.255.5.1
  ip netns exec $1 ping -c1 10.255.5.2
}

ping C1
ping C2
ping C3
ping C4
