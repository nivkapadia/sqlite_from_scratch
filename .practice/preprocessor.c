#include <stdio.h>
#include <stdlib.h>

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *) 0)->Attribute)
typedef struct 
{
    int id;
    char *name;
} Student;

int main(void)
{
    Student s;
    printf("%zu\n", size_of_attribute(Student, name));
}