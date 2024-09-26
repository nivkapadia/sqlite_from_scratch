#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

typedef struct {
	char *buffer;
	size_t buffer_length;
	size_t input_length;
} InputBuffer;


ssize_t getline(char **lineptr, size_t *n, FILE *stream);
InputBuffer *new_input_buffer();
void *close_input_buffer(InputBuffer *input_buffer);
void read_input(InputBuffer *input_buffer);
void print_prompt();

int main(int argv, char *argc[])
{
	InputBuffer *input_buffer = new_input_buffer();

	if (input_buffer == NULL)
	{
		puts("Error: Input Buffer Malloc Failed");
		exit(1);
	}

	while(true)
	{
		print_prompt();
		read_input(input_buffer);

		if (strcmp(input_buffer->buffer, ".exit") == 0)
		{
			close_input_buffer(input_buffer);
			exit(EXIT_SUCCESS);
		}
		else
		{
			printf("Unrecognised commands '%s'\n", input_buffer->buffer);
		}
	}
}

InputBuffer *new_input_buffer()
{
	InputBuffer *buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
	buffer->buffer = NULL;
	buffer->buffer_length = 0;
	buffer->input_length = 0;

	return buffer;

}

void print_prompt()
{
	printf("> db$ ");
}

void read_input(InputBuffer *input_buffer)
{
	ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
	
	if (bytes_read <= 0)
	{
		puts("Error reading input");
		exit(EXIT_FAILURE);
	}
	
	input_buffer->input_length = bytes_read - 1;
	input_buffer->buffer[bytes_read - 1] = 0;

}

void *close_input_buffer(InputBuffer *input_buffer)
{
	free(input_buffer->buffer);
	free(input_buffer);
}