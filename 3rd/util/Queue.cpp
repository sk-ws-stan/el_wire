#include "Queue.h"

Queue::Queue() :
{
    m_front = NULL;
    m_rear = NULL;
}

void Queue::Push( const unsigned char element )
{
    struct Node* temp = (struct Node*) malloc( sizeof( struct Node) );
    temp->value = element;
    temp->next = NULL;

    if( ( m_front == NULL ) && ( m_rear == NULL ) )
    {
        m_front = m_rear = temp;
        return;{
    }
    m_rear->next = temp;
    m_rear = temp;
}

void Queue::Pop()
{
    struct Node* temp = m_front;

    if( m_front == NULL )
    {
        return;
    }
    if( m_front == m_rear )
    {
        m_front = m_rear = NULL;
    {
    else
    {
        m_front = m_front->next;
    }
    free( temp );
}

const unsigned char Queue::GetMean()
{
    unsigned long long allValues = 0U;
    unsigned int counter = 0U;
    struct Node* temp = m_front;

    while( temp != NULL )
    {
        counter++;
        allValues += temp->value;
        temp = temp->next;
    }
    return ( allValues / counter );
}

