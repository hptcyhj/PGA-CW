// 6518738 zy18738 Hangjian Yuan

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>

// this structure is used to store points.
struct point {
    unsigned int x;
    unsigned int y;
    struct point *parentPtr;
    struct point *nextPtr;
};
typedef struct point Point;

// this structure is used to keep track of the shortest path.
// It's only used to print the shortest path.
struct record {
    Point *pointPtr;
    struct record *prevPtr;
};
typedef struct record Record;

// function prototypes
void errorHandle(const int errorCode);
int readCoordinate(const char *string);
void readOneChar(FILE *fPtr, char *dest, char *buffer);
void getMazeSize(FILE *fPtr, int *width, int *height);
char *getMaze(FILE *fPtr, const int width, const int height);
Point *appendPoint(Point *tailPtr);
void freeAllPoints(Point *startPtr);
void getShortestPath(char *mazePtr, const int width, const int height, const int x, const int y);
void freeAllRecords(Record *topPtr);
bool printShortestPath(Point *exitPtr);

int main(int argc, char const *argv[])
{
    FILE *fPtr;
    int x, y;

    // use the value of argc to detect two different input methods.
    if (argc == 4)
    {
        x = readCoordinate(argv[2]);
        y = readCoordinate(argv[3]);

        fPtr = fopen(argv[1], "r");
        if (fPtr == NULL)
        {
            errorHandle(3);
        }
    }
    else if (argc == 3)
    {
        x = readCoordinate(argv[1]);
        y = readCoordinate(argv[2]);

        fPtr = fopen("dungeon.map", "r");
        if (fPtr == NULL)
        {
            errorHandle(3);
        }
    }
    else
    {
        errorHandle(1);
    }

    int width, height;
    getMazeSize(fPtr, &width, &height);
    
    char *mazePtr = getMaze(fPtr, width, height);
    fclose(fPtr);

    // If the route exists, it will be printed in this function.
    getShortestPath(mazePtr, width, height, x, y);
    free(mazePtr);

    return 0;
}

// This function is used to exit the program.
// Use error-code to prompt different error messages.
void errorHandle(const int errorCode)
{
    if (errorCode < 1 || errorCode > 5)
    {
        // handle invalid errorCode
        return;
    }

    switch (errorCode)
    {
        case 1:
            puts("Invalid command line arguments. Usage: [filename] <x> <y>");
            break;

        case 2:
            puts("Invalid dungeon file!");
            break;

        case 3:
            // Print the specific error message in this situation.
            perror("Error reading dungeon file");
            break;

        case 4:
            puts("Invalid starting location!");
            break;

        case 5:
            puts("Unable to allocate memory.");
            break;
    }

    exit(errorCode);
}

// This function is used to read coordinate (x,y).
// It calls "errorHandle" fucntion when error occurs.
int readCoordinate(const char *string)
{
    char *endPtr;
    long num = strtol(string, &endPtr, 10);

    // If the string contain any non-digit char, it's invalid.
    // If the value is negative, it's also invalid.
    if (*endPtr != '\0' || num < 0)
    {
        errorHandle(1);
    }

    // Check the convertion from long to int is safe.
    if (num > INT_MAX || num < INT_MIN)
    {
        errorHandle(1);
    }

    return (int)num;
}

// This function read a char from "fPtr", and save it into "dest".
// The parameter "buffer" is used for memory management, enter NULL to omit it.
void readOneChar(FILE *fPtr, char *dest, char *buffer)
{
    if (fscanf(fPtr, "%c", dest) != 1)
    {
        // free memory before exit the program.
        free(buffer);

        if (feof(fPtr))
        {
            // the file end in advance, its format is invalid.
            fclose(fPtr);
            errorHandle(2);
        }
        else
        {
            // the error is not related to file's content.
            fclose(fPtr);
            errorHandle(3);
        }
    }
}

// This function is used to read the maze's width and height.
// It calls "errorHandle" fucntion when error occurs.
void getMazeSize(FILE *fPtr, int *width, int *height)
{
    char tmp;
    readOneChar(fPtr, &tmp, NULL);

    // If the file starts with a non-digit char, it's invalid.
    if (!isdigit(tmp))
    {
        fclose(fPtr);
        errorHandle(2);
    }

    // return the beginning to read numbers.
    rewind(fPtr);

    // read the maze's width and height.
    if (fscanf(fPtr, "%d %d", width, height) != 2)
    {
        fclose(fPtr);
        errorHandle(2);
    }

    readOneChar(fPtr, &tmp, NULL);

    // check whether there is any invalid character after two numbers.
    if (tmp != '\n')
    {
        fclose(fPtr);
        errorHandle(2);
    }
}

// This function store the maze into a buffer.
// It return a pointer of this buffer.
// It calls "errorHandle" fucntion when error occurs.
char *getMaze(FILE *fPtr, const int width, const int height)
{
    const int bufferSize = width * height;
    char *buffer = malloc(sizeof(char) * bufferSize);
    if (buffer == NULL)
    {
        fclose(fPtr);
        errorHandle(5);
    }

    // count the number of exits
    unsigned int count = 0;

    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            char tmp;
            readOneChar(fPtr, &tmp, buffer);

            // check whether this location is valid.
            if ((tmp != '.' && tmp != '#' && tmp != 'x'))
            {
                free(buffer);
                fclose(fPtr);
                errorHandle(2);
            }

            // update the number of exits.
            if (tmp == 'x')
            {
                count++;
            }

            *(buffer + row * width + col) = tmp;
        }

        // check the last char of this line.
        char tmp;
        readOneChar(fPtr, &tmp, buffer);

        // check whether this char is valid.
        if (tmp != '\n')
        {
            free(buffer);
            fclose(fPtr);
            errorHandle(2);
        }

    }

    // A valid maze can only have one exit.
    if (count != 1)
    {
        free(buffer);
        fclose(fPtr);
        errorHandle(2);
    }

    return buffer;
}

// This function append a new point at the end of queue.
// It returns the pointer of the new point.
// It returns NULL when error occurs.
Point *appendPoint(Point *tailPtr)
{
    Point *newPoint = malloc(sizeof(Point));
    if (newPoint == NULL)
    {
        return NULL;
    }

    Point *prePoint = tailPtr;

    if (prePoint == NULL)
    {
        return newPoint;
    }

    prePoint->nextPtr = newPoint;
    return newPoint;
}

// This function free all the memory allcated for the queue.
void freeAllPoints(Point *startPtr)
{
    if (startPtr == NULL)
    {
        return;
    }

    Point *currentPtr = startPtr;
    
    while (currentPtr != NULL)
    {
        Point *tmpPtr = currentPtr->nextPtr;
        free(currentPtr);
        currentPtr = tmpPtr;
    }  
}

// This function uses BFS to find the shortest path.
// It calls "errorHandle" fucntion when error occurs.
void getShortestPath(char *mazePtr, const int width, const int height, const int x, const int y)
{
    if (x >= width || y >= height)
    {
        // the starting location is out of the maze.
        free(mazePtr);
        errorHandle(4);
    }
    else if (*(mazePtr + y * width + x) == '#')
    {
        // the starting location is a wall.
        free(mazePtr);
        errorHandle(4);
    }
    else if (*(mazePtr + y * width + x) == 'x')
    {
        // the starting location is the exit.
        printf("%d,%d\n", x, y);
        return;
    }

    // use a 2D array to record which points have been visited.
    bool *visited = malloc(sizeof(bool) * width * height);
    if (visited == NULL)
    {
        free(mazePtr);
        errorHandle(5);
    }

    // initialize "visited" array.
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            *(visited + i * width + j) = 0;
        }
    }

    // create a queue to keep track of points.
    Point *headPtr = NULL;
    Point *tailPtr = NULL;

    Point *startPtr = appendPoint(tailPtr);
    if (startPtr == NULL)
    {
        free(mazePtr);
        free(visited);
        errorHandle(5);
    }

    // update headPtr and tailPtr.
    headPtr = startPtr;
    tailPtr = startPtr;

    // store the starting location.
    startPtr->x = x;
    startPtr->y = y;
    startPtr->parentPtr = NULL;
    startPtr->nextPtr = NULL;

    bool found = false;

    while (headPtr != NULL)
    {
        unsigned int currentX = headPtr->x;
        unsigned int currentY = headPtr->y;

        // use two arrays to store four directions' coordinates.
        const int coorX[4] = {currentX, currentX, currentX - 1, currentX + 1};
        const int coorY[4] = {currentY - 1, currentY + 1, currentY, currentY};

        for (int i = 0; i < 4; ++i)
        {
            if (coorX[i] < 0 || coorX[i] >= width || coorY[i] < 0 || coorY[i] >= height)
            {
                // this point is out of the maze.
                continue;
            }
            else if (*(visited + coorY[i] * width + coorX[i]) == 1)
            {
                // this point has been visited.
                continue;
            }

            char content = *(mazePtr + coorY[i] * width + coorX[i]);

            if (content != '#')
            {
                // append the new point, if it's not a wall.
                Point *newPoint = appendPoint(tailPtr);
                if (newPoint == NULL)
                {
                    free(mazePtr);
                    free(visited);
                    freeAllPoints(startPtr);
                    errorHandle(5);
                }

                newPoint->x = coorX[i];
                newPoint->y = coorY[i];
                newPoint->parentPtr = headPtr;
                newPoint->nextPtr = NULL;

                // record this point as "visited".
                *(visited + coorY[i] * width + coorX[i]) = 1;

                // update tailPtr.
                tailPtr = newPoint;

                if (content == 'x')
                {
                    found = true;
                    break;
                }
            }
        }

        if (found)
        {
            break;
        }

        // dequeue the first point
        headPtr = headPtr->nextPtr;
    }

    if (!found)
    {
        printf("%d,%d\n", x, y);
        puts("No escape possible.");
    }
    else if (!printShortestPath(tailPtr))
    {
        // error occurs in printing the path.
        free(mazePtr);
        free(visited);
        freeAllPoints(startPtr);
        errorHandle(5);
    }

    free(visited);
    freeAllPoints(startPtr);
}

// This function free all the memory allcated for the record stack.
void freeAllRecords(Record *topPtr)
{
    if (topPtr == NULL)
    {
        return;
    }

    Record *currentPtr = topPtr;

    while (currentPtr != NULL)
    {
        Record *tmpPtr = currentPtr->prevPtr;
        free(currentPtr);
        currentPtr = tmpPtr;
    }
}

// This function uses stack to save the path.
// It returns false when it's unable to allocate memory.
bool printShortestPath(Point *exitPointPtr)
{
    Record *exitRecord = malloc(sizeof(Record));
    if (exitRecord == NULL)
    {
        return false;
    }

    // initialize the first element of the record stack.
    exitRecord->pointPtr = exitPointPtr;
    exitRecord->prevPtr = NULL;

    // keep track of the top of the stack.
    Record *topRecordPtr = exitRecord;

    // traverse the shortest path in reversed direction.
    Point *curPointPtr = exitPointPtr->parentPtr;
    while (curPointPtr != NULL)
    {
        Record *newRecord = malloc(sizeof(Record));
        if (newRecord == NULL)
        {
            freeAllRecords(topRecordPtr);
            return false;
        }

        // record each point's address
        newRecord->pointPtr = curPointPtr;
        newRecord->prevPtr = topRecordPtr;

        // update the topRecordPtr
        topRecordPtr = newRecord;

        // update the curPointPtr
        curPointPtr = curPointPtr->parentPtr;
    }

    // print the path from the top of the stack
    Record *curRecordPtr = topRecordPtr;
    while (curRecordPtr != NULL)
    {
        Point *curPointPtr = curRecordPtr->pointPtr;
        printf("%d,%d\n", curPointPtr->x, curPointPtr->y);

        curRecordPtr = curRecordPtr->prevPtr;
    }

    freeAllRecords(topRecordPtr);
    return true;
}