
#!/bin/bash

# Compile the C program
gcc -o test_db your_program.c

# Check if compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

# Run the program and test insert and select commands
# Here, we use a here-document to simulate user input.
./test_db << EOF
insert 1 user1 user1@example.com
insert 2 user2 user2@example.com
select
.exit
EOF
