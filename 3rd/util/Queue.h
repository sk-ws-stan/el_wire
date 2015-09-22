#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>

class Queue
{
  public:
    struct Node
    {
        unsigned int value;
        struct Node* next;
    };

    Queue();
    ~Queue();
    void Push( const unsigned int element );
    void Pop();
    void Clear();
    const unsigned int GetMean() const;
    const unsigned int GetSize() const;

  private:
    unsigned int m_counter;
    struct Node* m_front;
    struct Node* m_rear;
};

#endif
