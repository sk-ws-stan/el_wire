#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>

class Queue
{
  public:
    struct Node
    {
        unsigned char value;
        struct Node* next;
    };

    Queue();
    void Push( const unsigned char& element );
    void Pop();
    const unsigned char GetMean();

  private:
    struct Node* m_front;
    struct Node* m_rear;
};

#endif
