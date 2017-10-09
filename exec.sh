#!/bin/sh

bin/game $@ |& tee tmp/log.txt &
PID=$!

trap 'kill -s INT $PID' INT
wait $PID
