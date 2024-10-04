username=""

for i in {1..32}; do
    username+="a" 
done
email=""

for i in {1..255}; do
    email+="b"
done

gcc -o test_db your_program.c

if [$? -ne 0]; then
    echo "Compilation failed"
    exit 1
fi

./test_db << EOF
insert $username user1@example.com
insert user2 $email
select
.exit
EOF
