#!/bin/bash

while read cmd; do
    echo "Audio command received for $cmd"
    aplay `find turretsounds/$cmd/ -type f | shuf -n 1`
done

