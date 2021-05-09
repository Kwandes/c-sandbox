#ifndef C_REPO_STACK_H
#define C_REPO_STACK_H

#ifdef __cplusplus
 extern "C" {
#endif


struct Stack* createStack(unsigned capacity);
int isFull(struct Stack* stack);
int isEmpty(struct Stack* stack);
void push(struct Stack* stack, int item);
int pop(struct Stack* stack);
int peek(struct Stack* stack);


#ifdef __cplusplus
}
#endif


#endif //C_REPO_STACK_H
