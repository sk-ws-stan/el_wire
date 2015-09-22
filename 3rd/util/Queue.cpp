#include "Queue.h"
#include "Arduino.h"

Queue::Queue() :
    m_counter( 0U )
{
    m_front = NULL;
    m_rear = NULL;
}

Queue::~Queue()
{
    free( m_front );
    free( m_rear );
}

void Queue::Push( const unsigned int element )
{
    struct Node* temp = (struct Node*) malloc( sizeof( struct Node) );
    temp->value = element;
    temp->next = NULL;

    if( ( m_front == NULL ) && ( m_rear == NULL ) )
    {
        m_front = m_rear = temp;
        return;
    }
    m_rear->next = temp;
    m_rear = temp;
    m_counter++;
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
    }
    else
    {
        m_front = m_front->next;
    }
    free( temp );
    m_counter--;
}

void Queue::Clear()
{
   // for( unsigned int x = 0U; x <= m_counter; x++ )
   // {
   //     struct Node* temp = m_front;
   //     if( ( m_front != NULL ) && ( m_front != m_rear ) )
   //     {
   //         m_front = m_front->next;
   //         free( temp );
   //     }
   //     else if( m_front == m_rear )
   //     {
   //         m_front = m_rear = NULL;
   //         free( temp );
   //     }
   // }
    struct Node* temp = m_front;
    while( temp != NULL )
    {
        struct Node* temp2 = temp;
        temp = temp->next;
        free( temp2 );
    }
    m_front = m_rear = NULL;
    m_counter = 0U;
}

const unsigned int Queue::GetMean() const
{
    unsigned long int allValues = 0U;
    struct Node* temp = m_front;

    while( temp != NULL )
    {
        allValues += max( temp->value, 0U );
        temp = temp->next;
    }
    double result = (double)allValues / (double)m_counter;
    return (int)result;
}

const unsigned int Queue::GetSize() const
{
    return m_counter;
}

