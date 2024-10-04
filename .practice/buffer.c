#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char s[] = {'a', 'b', 'c'};
    

    scanf("%c", &s[0]);
    scanf("%c", &s[1]);
    scanf("%c", &s[2]);
    /*s[0] = getchar();*/
    /*getchar();*/
    /*s[1] =getchar();*/
    /*getchar();*/
    /*s[2] =   getchar();*/
    /*getchar();*/
    /*fgets(&s[0], sizeof(char), stdin);*/
    /*fgets(&s[1], sizeof(char), stdin);*/
    /*fgets(&s[2], sizeof(char), stdin);*/
    printf("s at 0: %c\n", s[0]);
    printf("s at 1: %c\n", s[1]);
    printf("s at 2: %c\n", s[2]);
}

