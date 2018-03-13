#!/bin/bash

SCHEDLIST='cfq deadline noop'
for sched in $SCHEDLIST; do
	echo $sched | sudo tee /sys/block/sdd/queue/scheduler
	./bench
done
	
