#ifndef C_REPO_STACK_H
#define C_REPO_STACK_H

#ifdef __cplusplus
 extern "C" {
#endif


struct Stack* createStack(unsigned capacity);
short isFull(struct Stack* stack);
short isEmpty(struct Stack* stack);
void push(struct Stack* stack, short item);
short pop(struct Stack* stack);
short peek(struct Stack* stack);


#ifdef __cplusplus
}
#endif


#endif //C_REPO_STACK_H
