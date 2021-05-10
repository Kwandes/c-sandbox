#ifndef C_REPO_QUEUE_H
#define C_REPO_QUEUE_H

#ifdef __cplusplus
 extern "C" {
#endif

struct Queue* createQueue();
struct QNode* newNode(int k);
void enQueue(struct Queue* q, int k);
void deQueue(struct Queue* q);
short getFirst(struct Queue* q);
short getLast(struct Queue* q);

#ifdef __cplusplus
}
#endif


#endif //C_REPO_QUEUE_H
