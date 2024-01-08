#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUFFER_SIZE 65536

typedef struct IntegerSet
{
    uint64_t size;
    uint64_t capacity;
    uint64_t* data;
} IntegerSet;

IntegerSet create_set()
{
    uint64_t size = 0;
    return (IntegerSet) {
        .size = 0,
        .capacity = 10,
        .data = malloc(sizeof(uint64_t) * 10)
    }; 
}

void delete_list(IntegerSet* collection)
{
    free(collection->data);
}

bool contains_item(IntegerSet* collection, uint64_t item)
{
    for (uint64_t i = 0; i < collection->size; i++)
    {
        if (collection->data[i] == item)
        return true;
    }
    return false;
}

void add_item(IntegerSet* collection, uint64_t item)
{
    if (contains_item(collection, item))
        return;

    if (collection->capacity == collection->size)
    {
        collection->capacity *= 2;
        collection->data = realloc(collection->data, collection->capacity);
    }
    collection->data[collection->size] = item;
    collection->size++;
}

void read_integers_from_file(FILE* source, IntegerSet* collection)
{

}

int main(int argc, char* argv[])
{

    if (argc <= 1)
    {
        printf("No arguments were given. Exiting.");
        return 0;
    }

    IntegerSet lines = create_set();
    if (argc >= 3)
    {
        if (strcmp(argv[2], "-f") == 0)
        {
            if (argc != 4)
            {
                printf("Wrong usage of command.");
                return -1;
            }

            FILE* indices_source = fopen64(argv[3], "r");
            if (indices_source == NULL)
            {
                printf("Couldn't read file: %s", argv[3]);
                return -1;
            }

            read_integers_from_file(indices_source, &lines);
            fclose(indices_source);
        }
        else
        {
            for (int i = 2; i < argc; i++)
            {
                add_item(&lines, atoi(argv[i]));
            }
        }
    }
    else
    {
        printf("No line numbers were given. Exiting.");
        return 0;
    }

    FILE* file = fopen64(argv[1], "r");
    if (file == NULL)
    {
        printf("Couldn't read file: %s", argv[1]);
        return -1;
    }


    // retrieve lines
    char** output = malloc(lines.size * sizeof(char*));

    char buffer[BUFFER_SIZE];
    
    uint64_t line_buffer_size = 256;
    char* line_buffer = malloc(line_buffer_size);
    uint64_t line_buffer_index = 0;

    size_t collection_index = 0;
    uint64_t line_counter = 0;

    while (true)
    {
        if (collection_index >= lines.size)
            break;

        size_t res = fread(buffer, 1, BUFFER_SIZE, file);
        if (ferror(file))
        {
            printf("Error during reading of file.");
            return -1;
        }

        for (int i = 0; i < res; i++)
        {
            if (line_counter == lines.data[collection_index])
            {
                if (line_buffer_index >= line_buffer_size)
                {
                    line_buffer_size *= 2;
                    line_buffer = realloc(line_buffer, line_buffer_size);
                }
                line_buffer[line_buffer_index++] = buffer[i];
            }

            if (buffer[i] == '\n')
            {
                if (line_counter == lines.data[collection_index])
                {
                    char* line_string = malloc(line_buffer_index + 1);
                    strncpy(line_string, line_buffer, line_buffer_index);
                    line_string[line_buffer_index] = '\0';
                    output[collection_index++] = line_string;

                    line_buffer_index = 0;
                }
                line_counter++;
            }
        }

        if (feof(file))
        {
            if (line_counter == lines.data[collection_index])
            {
                char* line_string = malloc(line_buffer_index + 1);
                strncpy(line_string, line_buffer, line_buffer_index);
                line_string[line_buffer_index] = '\0';
                output[collection_index++] = line_string;
            }
            break;
        }
    }

    for (int i = 0; i < lines.size; i++)
    {
        printf("%s", output[i]);
        free(output[i]);
    }

    free(line_buffer);
    fclose(file);
    delete_list(&lines);

    return 0;
}


