#!/bin/zsh

make onOwn

for i in {1..1400}; do
    ./onOwn << EOF
insert $i foo bar
.exit
EOF
done
