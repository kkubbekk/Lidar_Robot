#include <stdbool.h>

#define BUFFER_SIZE 50

typedef struct
{
    int* rd_ptr;
    int* wr_ptr;
    int  arr[BUFFER_SIZE];
    bool is_full;
    int  sum;
} RingBuf;

void ring_buff_init(RingBuf *rb);
void ring_buff_write(RingBuf *rb, int value);
int avg_ring_buff(RingBuf *rb);


