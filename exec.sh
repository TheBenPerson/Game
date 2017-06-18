#!/bin/sh

LD_LIBRARY_PATH=bin bin/game $@ |& tee tmp/log.txt
