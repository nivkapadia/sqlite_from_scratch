#!/bin/bash

make db

for i in {1..25}; do
    ./db table.db << EOF
insert $i foo bar
.exit
EOF
done
