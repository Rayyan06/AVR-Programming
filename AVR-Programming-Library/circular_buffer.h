#include <stdint.h>

#define TX_BUFFER_SIZE 64

typedef struct
{
    uint16_t time;    /* In milliseconds */
    int16_t position; /* In Ticks  */
} DataPoint;

// Buffer for encoder data
typedef struct
{
    DataPoint data[TX_BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
} circular_buffer_t;

void buffer_write(volatile circular_buffer_t *buffer, DataPoint data);

/* Retrievs */
DataPoint buffer_get(volatile circular_buffer_t *buffer);
