// C program for array implementation of stack
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

// A structure to represent a stack
struct Stack {
    int top;
    unsigned capacity;
    short* array;
};
 
// function to create a stack of given capacity. It initializes size of
// stack as 0
struct Stack* createStack(unsigned capacity)
{
    struct Stack* stack = (struct Stack*)malloc(sizeof(struct Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (short*)malloc(stack->capacity * sizeof(short));
    return stack;
}
 
// Stack is full when top is equal to the last index
short isFull(struct Stack* stack)
{
    return stack->top == stack->capacity - 1;
}
 
// Stack is empty when top is equal to -1
short isEmpty(struct Stack* stack)
{
    return stack->top == -1;
}
 
// Function to add an item to stack.  It increases top by 1
void push(struct Stack* stack, short item)
{
    if (isFull(stack))
        return;
    stack->array[++stack->top] = item;
    printf("%d pushed to stack\n", item);
}
 
// Function to remove an item from stack.  It decreases top by 1
short pop(struct Stack* stack)
{
    if (isEmpty(stack))
        return -1;
    return stack->array[stack->top--];
}
 
// Function to return the top from stack without removing it
short peek(struct Stack* stack)
{
    if (isEmpty(stack))
        return -1;
    return stack->array[stack->top];
}