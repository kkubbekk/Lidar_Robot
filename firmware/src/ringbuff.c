#include "ringbuff.h"


void ring_buff_write(RingBuf *rb,int value)
{
    if(rb->is_full)
    {
        rb->rd_ptr +=1;
        rb->sum -= *(rb->wr_ptr);
    }
    rb->sum += value;

    *(rb->wr_ptr) = value;
    rb->wr_ptr +=1;


    if(rb->rd_ptr > &rb->arr[BUFFER_SIZE -1 ])
    {
        rb->rd_ptr = &rb->arr[0];
    }
    
    if(rb->wr_ptr > &rb->arr[BUFFER_SIZE -1 ])
    {
        rb->wr_ptr = &rb->arr[0];
    }
    if(rb->wr_ptr == rb->rd_ptr)
    {
        rb->is_full = true;
    }
}

int avg_ring_buff(RingBuf *rb)
{
    int current_size;

    if(rb->is_full)
    {
        current_size = BUFFER_SIZE;
    }
    else
    {
        current_size = rb->wr_ptr - rb->arr;
    }
    if(current_size == 0)
    {
        return 0;
    }
    
    return rb->sum / current_size;
}

void ring_buff_init(RingBuf *rb)
{
    rb->rd_ptr = &rb->arr[0];
    rb->wr_ptr = &rb->arr[0];
    rb->is_full = false;
    rb->sum = 0;
}
