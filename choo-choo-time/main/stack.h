#ifndef C_REPO_DEFINITIONS_H
#define C_REPO_DEFINITIONS_H

struct Stack* createStack(unsigned capacity);
int isFull(struct Stack* stack);
int isEmpty(struct Stack* stack);
void push(struct Stack* stack, int item);
int pop(struct Stack* stack);
int peek(struct Stack* stack);

#endif //C_REPO_DEFINITIONS_H
