# !/bin/bash
set -eu
#set -x

if [ $1 = "del" ] ; then
    sudo ip -all netns del
fi

if [ $1 = "all" ] ; then
    sudo ip netns list
fi