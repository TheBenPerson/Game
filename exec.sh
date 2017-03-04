#!/bin/sh

LD_LIBRARY_PATH=lib bin/game $@ |& tee tmp/log.txt
