#include "circular_buffer.h"

/* Adds data */
void buffer_write(volatile circular_buffer_t *buffer, DataPoint data)
{
    // Calculate the next write index
    uint8_t next_head = (buffer->head + 1) % TX_BUFFER_SIZE;
    uint8_t buffer_is_full = (next_head == buffer->tail);

    if (!buffer_is_full)
    {
        // Update the data at the head pointer
        buffer->data[buffer->head] = data;

        // Update the head pointer forward, or loop around
        buffer->head = next_head;
    }
}
DataPoint buffer_get(volatile circular_buffer_t *buffer)
{
    DataPoint point = buffer->data[buffer->tail];

    // Update the tail pointer
    buffer->tail = (buffer->tail + 1) % TX_BUFFER_SIZE;

    return point;
}

/*
void USART_Transmit(DataPoint data)
{
    buffer_write(&tx_buffer, data);
    // Enable Data Register Empty Interrupt
    UCSR0B |= (1 << UDRIE0);
}
*/