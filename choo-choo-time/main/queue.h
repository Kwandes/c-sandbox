#ifndef C_REPO_QUEUE_H
#define C_REPO_QUEUE_H

#ifdef __cplusplus
 extern "C" {
#endif

struct Queue* createQueue();
struct QNode* newNode(unsigned short k);
void enQueue(struct Queue* q, unsigned short k);
void deQueue(struct Queue* q);
unsigned short getFirst(struct Queue* q);
unsigned short getLast(struct Queue* q);

#ifdef __cplusplus
}
#endif


#endif //C_REPO_QUEUE_H
