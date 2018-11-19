// 6518738 zy18738 Hangjian Yuan

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BUFFER_SIZE 20

struct directory {
    char *name;
    char *number;
    char *department;
    struct directory *nextPtr;
};
typedef struct directory Dir;

void printMenu(void);
char *prompt(const char *message);
int readOption(const char *message);
void printEntry(Dir *entryPtr);
int getLength(Dir *startPtr);
Dir *getEntryPtr(Dir *startPtr, const int index);
void freeEntry(Dir *entryPtr);
void freeDirectory(Dir *startPtr);
void printDirectory(Dir *startPtr);
int searchNumber(Dir *startPtr, const char *target);
bool addNumber(Dir **startPtr);
bool modifyNumber(Dir *startPtr);
bool deleteNumber(Dir **startPtr);
bool searchSubstring(const char *source, const char *target);
bool searchDirectory(Dir *startPtr);

int main(int argc, char const *argv[])
{
    Dir *start = NULL;
    int option;

    do
    {
        printMenu();

        option = readOption("Option: ");
        if (option == -1)
        {
            puts("Unable to allocate memory.");
            freeDirectory(start);
            exit(-1);
        }

        switch (option)
        {
            case 1:
                printDirectory(start);
                break;
            
            case 2:
                if (!addNumber(&start))
                {
                    puts("Unable to allocate memory.");
                    freeDirectory(start);
                    exit(-1);
                }
                break;
            
            case 3:
                if (!modifyNumber(start))
                {
                    puts("Unable to allocate memory.");
                    freeDirectory(start);
                    exit(-1);
                }
                break;
            
            case 4:
                if (!deleteNumber(&start))
                {
                    puts("Unable to allocate memory.");
                    freeDirectory(start);
                    exit(-1);
                }
                break;
            
            case 5:
                if (!searchDirectory(start))
                {
                    puts("Unable to allocate memory.");
                    freeDirectory(start);
                    exit(-1);
                }
                break;
            
            case 6:
                freeDirectory(start);
                break;
            
            default:
                puts("Unknown option!");
                break;
        }
    } while (option != 6);
    
    return 0;
}

void printMenu(void)
{
    puts("1) Print directory");
    puts("2) Add number");
    puts("3) Modify number");
    puts("4) Delete number");
    puts("5) Search");
    puts("6) Quit");
}

// This function is from lecture.
// With my own modification to read arbitrary length.
char *prompt(const char *message)
{
    // initialize the limit of input
    int limit = BUFFER_SIZE;
    
    // allocate the string buffer
    char *buffer = malloc(sizeof(char) * (limit + 1));
    if (buffer == NULL)
    {
        return NULL;
    }

    printf("%s", message);

    char tmp;
    int index = 0;
    do
    {
        // tmp saves the next character
        scanf("%c", &tmp);

        // malloc more memory when reach the limit
        if (index == limit)
        {
            limit += BUFFER_SIZE;
            buffer = realloc(buffer, sizeof(char) * (limit + 1));
            if (buffer == NULL)
            {
                return NULL;
            }
        }

        if (tmp == '\n')
        {
            // the input reaches the end
            buffer[index] = '\0';
            continue;
        }
        else
        {
            buffer[index] = tmp;
            index++;
        }
    } while (tmp != '\n');

    return buffer;
}

// Prompt user to enter a option.
// Return the option number if the input is valid, return -1 if error happens in malloc, otherwise return 0.
int readOption(const char *message)
{
    char *input = prompt(message);
    if (input == NULL)
    {
        // handle exception: error happens in malloc
        return -1;
    }

    if (strcmp(input, "1") == 0)
    {
        free(input);
        return 1;
    }
    else if (strcmp(input, "2") == 0)
    {
        free(input);
        return 2;
    }
    else if (strcmp(input, "3") == 0)
    {
        free(input);
        return 3;
    }
    else if (strcmp(input, "4") == 0)
    {
        free(input);
        return 4;
    }
    else if (strcmp(input, "5") == 0)
    {
        free(input);
        return 5;
    }
    else if (strcmp(input, "6") == 0)
    {
        free(input);
        return 6;
    }
    else
    {
        free(input);
        return 0;
    }
}

// Print the entry pointed by the entryPtr.
void printEntry(Dir *entryPtr)
{
    if (entryPtr == NULL)
    {
        // handle exception
        return;
    }

    printf("%s\t%s (%s)\n", (entryPtr->number), (entryPtr->name), (entryPtr->department));
}

// This function return the length of the directory.
int getLength(Dir *startPtr)
{
    if (startPtr == NULL)
    {
        return 0;
    }

    int length = 0;
    Dir *currentPtr = startPtr;

    while (currentPtr != NULL)
    {
        currentPtr = currentPtr->nextPtr;
        length++;
    }

    return length;
}

// This function return the entryPtr of the target index.
Dir *getEntryPtr(Dir *startPtr, const int index)
{
    if (startPtr == NULL)
    {
        // handle exception: empty directory
        return NULL;
    }
    else if (index >= getLength(startPtr))
    {
        // handle exception: index is out of bound
        return NULL;
    }

    Dir *currentPtr = startPtr;
    for (int i = 0; i < index; ++i)
    {
        currentPtr = currentPtr->nextPtr;
    }

    return currentPtr;
}

// Free the entry pointed by the entryPtr.
void freeEntry(Dir *entryPtr)
{
    if (entryPtr == NULL)
    {
        // handle exception
        return;
    }

    free(entryPtr->name);
    free(entryPtr->number);
    free(entryPtr->department);
    free(entryPtr);
}

// This function free the whole directory.
// Only use when error happens in malloc.
void freeDirectory(Dir *startPtr)
{
    int length = getLength(startPtr);

    // Free the whole directory from the end.
    for (int i = length - 1; i >= 0; --i)
    {
        Dir *entryPtr = getEntryPtr(startPtr, i);
        freeEntry(entryPtr);
    }
}

// This function print the directory in the specified format.
void printDirectory(Dir *startPtr)
{
    if (startPtr == NULL)
    {
        // Do nothing when the directory is empty
        return;
    }

    Dir *currentPtr = startPtr;

    while (currentPtr != NULL)
    {
        printEntry(currentPtr);
        currentPtr = currentPtr->nextPtr;
    }
}

// This function is used by add, modify, delete
// Return the index of the target, or -1 if not found
int searchNumber(Dir *startPtr, const char *target)
{
    if (startPtr == NULL)
    {
        // handle the empty directory
        return -1;
    }

    Dir *currentPtr = startPtr;
    int index = 0;

    while (currentPtr != NULL)
    {
        if (strcmp((currentPtr->number), target) == 0)
        {
            return index;
        }

        currentPtr = currentPtr->nextPtr;
        index++;
    }

    // If not found, return -1.
    return -1;
}

// Add a new entry to the end of the directory.
bool addNumber(Dir **startPtr)
{
    Dir *newEntry = malloc(sizeof(Dir));
    if (newEntry == NULL)
    {
        // handle exception: error happens in malloc
        return false;
    }

    newEntry->name = prompt("Name: ");
    newEntry->number = prompt("Number: ");
    newEntry->department = prompt("Department: ");
    newEntry->nextPtr = NULL;

    if (newEntry->name == NULL || newEntry->number == NULL || newEntry->department == NULL)
    {
        // handle exception: error happens in malloc
        freeEntry(newEntry);
        return false;
    }

    if (searchNumber(*startPtr, (newEntry->number)) != -1)
    {
        puts("Entry already exists, ignoring duplicate entry.");
        freeEntry(newEntry);
        return true;
    }
    else if (strlen(newEntry->number) == 0)
    {
        puts("Phone number cannot be empty, ignoring entry.");
        freeEntry(newEntry);
        return true;
    }
    else
    {
        if (*startPtr == NULL)
        {
            // directly change the startPtr, when the directory is empty
            *startPtr = newEntry;
        }
        else
        {
            Dir *currentPtr = *startPtr;

            // move the pointer to the end of the directory
            while ((currentPtr->nextPtr) != NULL)
            {
                currentPtr = currentPtr->nextPtr;
            }

            currentPtr->nextPtr = newEntry;
        }

        return true;
    }
}

// Modify a entry in the directory.
bool modifyNumber(Dir *startPtr)
{
    char *number = prompt("Enter phone number to modify: ");
    if (number == NULL)
    {
        // handle exception: error happens in malloc
        return false;
    }

    int index = searchNumber(startPtr, number);
    free(number);

    if (index == -1)
    {
        puts("Entry does not exist.");
        return true;
    }

    Dir *entryPtr = getEntryPtr(startPtr, index);

    printf("Enter new %s (return to keep [%s]): ", "name", entryPtr->name);
    char *name = prompt("");
    
    printf("Enter new %s (return to keep [%s]): ", "number", entryPtr->number);
    number = prompt("");

    printf("Enter new %s (return to keep [%s]): ", "department", entryPtr->department);
    char *department = prompt("");

    if (name == NULL || number == NULL || department == NULL)
    {
        // handle exception: error happens in malloc
        free(name);
        free(number);
        free(department);
        return false;
    }

    // handle two input exceptions with phone number.
    if (strlen(number) == 0 && strlen(entryPtr->number) == 0)
    {
        puts("Phone number cannot be empty, ignoring entry.");
        free(name);
        free(number);
        free(department);
        return true;
    }
    else if (searchNumber(startPtr, number) != -1 && searchNumber(startPtr, number) != index)
    {
        puts("Entry already exists, ignoring duplicate entry.");
        free(name);
        free(number);
        free(department);
        return true;
    }

    // If the input is empty, keep the original value.
    if (strlen(name) == 0)
    {
        free(name);
    }
    else
    {
        free(entryPtr->name);
        entryPtr->name = name;
    }

    if (strlen(number) == 0)
    {
        free(number);
    }
    else
    {
        free(entryPtr->number);
        entryPtr->number = number;
    }

    if (strlen(department) == 0)
    {
        free(department);
    }
    else
    {
        free(entryPtr->department);
        entryPtr->department = department;
    }

    return true;
}

// Delete a entry if it exists, otherwise do nothing.
bool deleteNumber(Dir **startPtr)
{
    char *number = prompt("Enter phone number to delete (return to cancel): ");
    if (number == NULL)
    {
        // handle exception: error happens in malloc
        free(number);
        return false;
    }

    if (strlen(number) == 0)
    {
        free(number);
        return true;
    }

    int index = searchNumber(*startPtr, number);
    free(number);
    if (index == -1)
    {
        // the entry does not exist.
        return true;
    }

    if (index == 0)
    {
        Dir *tmp = (*startPtr)->nextPtr;
        freeEntry(*startPtr);
        *startPtr = tmp;
    }
    else
    {
        Dir *previousPtr = getEntryPtr(*startPtr, index - 1);
        Dir *currentPtr = getEntryPtr(*startPtr, index);
        previousPtr->nextPtr = currentPtr->nextPtr;
        freeEntry(currentPtr);
    }

    return true;
}

// Search the target string in the source string.
// Return true if found, otherwise false.
bool searchSubstring(const char *source, const char *target)
{
    int souLen = strlen(source);
    int tarLen = strlen(target);
    
    if (souLen < tarLen)
    {
        // if target is longer, it's impossible to match.
        return false;
    }
    else if (strcmp(source, target) == 0)
    {
        return true;
    }
    else
    {
        int lenDif = souLen - tarLen;

        // compare target with all the same-length substring in source.
        for (int offset = 0; offset <= lenDif; ++offset)
        {
            // this loop compare each character.
            bool found = true;
            for (int i = 0; i < tarLen; ++i)
            {
                if (source[i + offset] != target[i])
                {
                    // not match in this offset
                    found = false;
                    break;
                }
            }

            if (found)
            {
                return true;
            }
        }

        return false;
    }
}

// Search the directory if entrys' substring match the target.
bool searchDirectory(Dir *startPtr)
{
    char *target = prompt("Search: ");
    if (target == NULL)
    {
        // handle exception: error happens in malloc
        free(target);
        return false;
    }

    if (strlen(target) == 0 || startPtr == NULL)
    {
        free(target);
        return true;
    }

    Dir *currentPtr = startPtr;

    while (currentPtr != NULL)
    {
        if (searchSubstring((currentPtr->name), target) ||
            searchSubstring((currentPtr->number), target) ||
            searchSubstring((currentPtr->department), target))
        {
            printEntry(currentPtr);
        }

        currentPtr = currentPtr->nextPtr;
    }

    free(target);
    return true;
}