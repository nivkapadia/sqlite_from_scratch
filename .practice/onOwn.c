/*
 *
 * Same code as main.c but trying to implement everything as myself as much as possible.
 *
 * pick from row_slot();
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <aio.h>
#include <stdint.h>

#define EXIT_SUCCESS 0
#define TABLE_MAX_PAGES 100
#define size_of_attribute(Struct, Attribute) sizeof((Struct *) 0)->Attribute
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255


typedef enum
{
    PREPARE_SYNTAX_ERROR,
    PREPARE_SUCCESS,
    PREPARE_STRING_TOO_LONG
}PrepareResult;

typedef enum
{
    EXECUTE_SUCCESS,
    EXECUTE_FAILURE,
    EXECUTE_TABLE_FULL
} ExecuteResult;

typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef enum
{
    META_COMMAND_SUCCESS,
    META_COMMAND_FAILURE
} MetaCommandResult;


typedef struct 
{
    char *buffer;
    size_t input_length;
    size_t input_size;
} InputBuffer;

typedef struct 
{
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
}Row;

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = USERNAME_SIZE + ID_OFFSET;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + EMAIL_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;


typedef struct
{
    StatementType type;
    Row row_to_insert;
} Statement;

typedef struct
{
    size_t num_rows;
    void *pages[TABLE_MAX_PAGES];
} Table;


InputBuffer *new_buffer();
Table *new_table();
void print_prompt();
void read_input(InputBuffer *input_buffer);
void close_table(InputBuffer *input_buffer);
MetaCommandResult do_meta_command(InputBuffer *input_buffer);
PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement);
ExecuteResult execute_statement(Table *table, Statement *statement);
void *row_slot(Table *table, uint32_t row_num);
ExecuteResult execute_insert(Table *table, Statement *statement);


int main(void)
{
    InputBuffer *input_buffer = new_buffer();
    Table *table = new_table();


    //TESTING & DEVELOPMENT
    /*printf("ID_SIZE: %d\n", ID_SIZE); printf("USERNAME_SIZE: %i\n", USERNAME_SIZE); printf("EMAIL_SIZE: %d\n", EMAIL_SIZE); printf("USERNAME_OFFSET: %d\n", USERNAME_OFFSET); printf("ROW_SIZE: %d\n", ROW_SIZE); /*printf("ID_SIZE: %d\n", ID_SIZE);*/ /*printf("ID_SIZE: %d\n", ID_SIZE);*/

    while (true)
    {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.')
        {
            do_meta_command(input_buffer);
        }
        Statement statement;

        switch (prepare_statement(input_buffer, &statement))
        {
            case PREPARE_SYNTAX_ERROR:
                printf("Syntax error while parsing '%s'\n", input_buffer->buffer);
                continue;
            case PREPARE_SUCCESS:
                break;
            case PREPARE_STRING_TOO_LONG:
                puts("Given string '%s' is too long\n");
                continue;
        }


        switch (execute_statement(table, &statement))
        {
            case EXECUTE_SUCCESS:
                break;
            case EXECUTE_FAILURE:
                puts("FAILED AT EXECUTE");
                continue;
            case EXECUTE_TABLE_FULL:
                puts("The table is full");
                continue;
        }

    }
}

InputBuffer *new_buffer()
{
    InputBuffer *input_buffer = (InputBuffer *)malloc(sizeof(InputBuffer));
    if (input_buffer == NULL)
    {
        puts("Malloc failed");
        exit(EXIT_FAILURE);
    }

    input_buffer->buffer = NULL;
    input_buffer->input_length = 0;
    input_buffer->input_size = 0;

    return input_buffer;
}

void print_prompt()
{
    printf("> db$ ");
}

void read_input(InputBuffer *input_buffer)
{
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->input_length), stdin);

    if (bytes_read <= 0)
    {
        puts("Error reading input, exit failure");
        exit(EXIT_FAILURE);
    }

    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read] = 0;
}

MetaCommandResult do_meta_command(InputBuffer *input_buffer)
{
    if (strcmp(input_buffer->buffer, ".exit\n") == 0)
    {
        exit(EXIT_SUCCESS);
    }
    return META_COMMAND_SUCCESS;
}

void close_table(InputBuffer *input_buffer)
{
    free(input_buffer->buffer);
    free(input_buffer);
}

PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement)
{
    if (strncmp(input_buffer->buffer, "insert", 6) == 0)
    {
        statement->type = STATEMENT_INSERT;

        uint32_t args_read = sscanf(input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id), 
                                    statement->row_to_insert.username, statement->row_to_insert.email);

        if (args_read != 3)
        {
            printf("Error during prepare_insert");
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }
    if (strncmp(input_buffer->buffer, "select", 6) == 0)
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_SYNTAX_ERROR;
}

/*PrepareResult prepare_insert(InputBuffer input_buffer, Statement *statement) */
/*{*/
/*    statement->row_to_insert->id*/
/*    return PREPARE_SYNTAX_ERROR;*/
/*}*/


void serialize_row(Row *source, void *destination)
{
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void *source, Row *destination)
{
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

Table *new_table()
{
    Table *table = (Table *)malloc(sizeof(Table));

    if (table == NULL)
    {
        puts("table malloc failed.");
        exit(EXIT_FAILURE);
    }

    table->num_rows = 0;
    
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        table->pages[i] = NULL;
    }
    return table;

}

void *row_slot(Table *table, uint32_t row_num)
{
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = table->pages[page_num]; 
    if (page == NULL)
    {
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }
    
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset  = row_offset * ROW_SIZE;

    return page + byte_offset;
}

ExecuteResult execute_statement(Table *table, Statement *statement)
{

    ExecuteResult execute_select(Table *table);
    ExecuteResult execute_insert(Table *table, Statement *statement);
    switch (statement->type)
    {
        case (STATEMENT_INSERT):
            return execute_insert(table, statement);
        case (STATEMENT_SELECT):
            return execute_select(table);
    }

    return EXECUTE_FAILURE;
}

ExecuteResult execute_insert(Table *table, Statement *statement)
{
    if (table->num_rows == TABLE_MAX_ROWS)
    {
        return EXECUTE_TABLE_FULL;
    }
    Row *row_to_insert = &(statement->row_to_insert);
    serialize_row(row_to_insert, row_slot(table, table->num_rows));

    table->num_rows += 1;
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Table *table)
{
    Row row;

    for (uint32_t i = 0; i < table->num_rows; i++)
    {
        deserialize_row(row_slot(table, i), &(row));
        printf("%i %s %s\n", row.id, row.username, row.email);
    }

    return EXECUTE_SUCCESS;
}
