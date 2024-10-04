/**
 * Here's how far the code has been implemented:
 * 
 *  - The code is able to take input from the console.
 *  - Implement the ability for the code to parse insert and select statements. -- Done
 *  - Execute Insert (DONE) and select statements (REMAINING).
 *  - 
 */


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255
#define size_of_attribute(Struct, Attribute) sizeof(((Struct *) 0)->Attribute)
#define MALLOC_FAILURE 437


typedef struct
{
    char *buffer;
    size_t buffer_size;
    size_t input_length;
} InputBuffer;

typedef enum
{
    PREPARE_SYNTAX_ERROR,
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNISED_STATEMENT
} PrepareResult;

typedef enum
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef enum
{
    EXECUTE_TABLE_FULL,
    EXECUTE_SUCCESS
}ExecuteResult;

typedef struct
{
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];
} Row;

#define TABLE_MAX_PAGES 1000

typedef struct 
{
    uint32_t num_rows;
    void *pages[TABLE_MAX_PAGES];
} Table;

typedef struct 
{
    Table *table;
    uint32_t row_num;
    bool end_of_table;
} Cursor;



const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

typedef struct 
{
    StatementType type;
    Row row_to_insert;
} Statement;



InputBuffer *init_buffer();
void read_input(InputBuffer *input_buffer);
MetaCommandResult do_meta_command(InputBuffer *input_buffer);
PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement);
void execute_statement(Statement *statement, Table *table);
void *serialize_row(Row* source, void* destination);
PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement);
void execute_statement(Statement *statement, Table *table);
Table* new_table();


int main(void)
{
    void print_prompt();

    InputBuffer *input_buffer = init_buffer();
    Table *table = new_table();
    while (true)
    {
        print_prompt();
        read_input(input_buffer);
        

        if (input_buffer->buffer[0] == '.')
        {
            switch (do_meta_command(input_buffer))
            {
            case (META_COMMAND_SUCCESS):
                continue;
            case (META_COMMAND_UNRECOGNIZED_COMMAND):
                printf("Unrecognized command '%s'\n", input_buffer->buffer);
                continue;
            }
        }

        Statement statement;
        switch (prepare_statement(input_buffer, &statement))
        {
            case (PREPARE_SUCCESS):
                execute_statement(&statement, &table);
                break;
            case (PREPARE_SYNTAX_ERROR):
                printf("Syntax error at the start of '%s'\n", input_buffer->buffer);
                break;
            case (PREPARE_UNRECOGNISED_STATEMENT):
                break;
        }
    }
}

InputBuffer *init_buffer()
{
    InputBuffer *input_buffer = (InputBuffer *)malloc(sizeof(InputBuffer));
    if (input_buffer == NULL)
    {
        puts("Error: malloc for input_buffer failed");
        exit(EXIT_FAILURE);
    }

    input_buffer->buffer = NULL;
    input_buffer->input_length = 0;
    input_buffer->buffer_size = 0;

    return input_buffer;
}

void print_prompt()
{
    printf("> db $ ");
}

void read_input(InputBuffer *input_buffer)
{
    ssize_t read_bytes =
        getline(&(input_buffer->buffer), &(input_buffer->buffer_size), stdin);

    if (read_bytes == -1)
    {
        printf("Read line failed\n");
        exit(EXIT_FAILURE);
    }

    input_buffer->input_length = read_bytes - 1;
    input_buffer->buffer[read_bytes - 1] = '\0';
}

MetaCommandResult do_meta_command(InputBuffer *input_buffer)
{
    if (strcmp(input_buffer->buffer, ".exit") == 0)
    {
        exit(EXIT_SUCCESS);
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}


void *serialize_row(Row* source, void* destination)
{
    memcpy(&(destination) + ID_SIZE, &(source), ID_SIZE);
    memcpy(&(destination) + USERNAME_SIZE, &(source), USERNAME_SIZE);
    memcpy(&(destination) + EMAIL_SIZE, &(source), EMAIL_SIZE);
}

void *deserialize_row(void* source, Row *destination)
{
    memcpy(&(destination->id), &source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), &source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), &source + EMAIL_OFFSET, EMAIL_SIZE);
}

void execute_select(Table *table)
{

}

PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement)
{
    if (strncmp(input_buffer->buffer, "insert", 6) == 0)
    {
        statement->type = STATEMENT_INSERT;
        int args_assigned = sscanf(input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id), &(statement->row_to_insert.username), &(statement->row_to_insert.username));

        if (args_assigned != 3)
        {
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }
    else if (strncmp(input_buffer->buffer, "select", 6) == 0)
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNISED_STATEMENT;
}

void execute_statement(Statement *statement, Table *table)
{
    ExecuteResult execute_insert(Statement *statement, Table *table);
    switch(statement->type)
    {
        case (STATEMENT_INSERT):
            printf("Executing insert statement.\n");
            execute_insert(statement, table);
            break;
        case (STATEMENT_SELECT):
            printf("Executing select statement.\n");
            break;
    }
}
void *get_page(Pager *pager, uint32_t page_num)
{
    if (page_num > TABLE_MAX_PAGES)
    {
        printf("Tried to fetch a page outside TABLE_MAX_PAGES\n");

        exit(EXIT_FAILURE);
    }

    
}
void *cursor_value(Cursor *cursor)
{
    uint32_t page_num = cursor->row_num / ROW_SIZE;
    /*void *page = cursor->table->pages[page_num];*/

    /*if (page == NULL)*/
    /*{*/
    /*    page = table->pages[page_num] = malloc(sizeof(PAGE_SIZE));*/
    /*}*/
    void* page = get_page(table->pager, page_num);
    uint32_t row_offset = cursor->row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    puts("row slot assigned from row_lsot");
    return byte_offset + page;
}

ExecuteResult execute_insert(Statement *statement, Table *table)
{
    puts("inside insert");
    if (table->num_rows >= TABLE_MAX_ROWS)
    {
        return EXECUTE_TABLE_FULL;
    }

    Row *row_to_insert = &(statement->row_to_insert);
    serialize_row(row_to_insert, cursor_value(table, table->num_rows));
    puts("row slot assigned");

    table->num_rows++;
    return EXECUTE_SUCCESS;
}

Table* new_table()
{
    Table *table = (Table *)malloc(sizeof(Table));
    if (table == NULL)
    {
        exit(MALLOC_FAILURE);
    }

    table->num_rows = 0;

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        table->pages[i] = NULL;
    }
    return table;
}

Cursor *table_start(Table *table)
{
    Cursor *cursor = malloc(sizeof(Cursor));

    if (cursor == NULL)
    {
        puts("Cursor malloc failed");
        exit(MALLOC_FAILURE);
    }

    cursor->table = table;
    cursor->row_num = 0;
    cursor->end_of_table = (table->num_rows == 0);
    return cursor;
}

Cursor *table_end(Table *table)
{
    Cursor *cursor = malloc(sizeof(Cursor));

    if (cursor == NULL)
    {
        puts("Cursor malloc failed");
        exit(MALLOC_FAILURE);
    }

    cursor->row_num = table->num_rows;
    cursor->table = table;
    cursor->end_of_table = true;

    return cursor;
}
