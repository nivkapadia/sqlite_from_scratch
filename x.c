#include <stdio.h>

int main(void)
{
	char s[4];
	fgets(s, 4, stdin);
	
	for (int i = 0; i < 4; i++)
	{
		if ( s[i] == '\n')
		{
			printf("\\n || \\0 encountered");
		}

		printf("%c", s[i]);
	}
}
