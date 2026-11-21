#include "ring_buf.h"

void ring_buf_init(ring_buf_t *rb, uint8_t *mem, uint16_t mem_size)
{
    rb->buffer  = mem;
    rb->buf_size = mem_size;
    rb->wr_idx  = 0;
    rb->rd_idx  = 0;
}

bool ring_buf_put(ring_buf_t *rb, uint8_t ch)
{
    if(ring_buf_is_full(rb))
    {
        return false;
    }
    rb->buffer[rb->wr_idx] = ch;
    rb->wr_idx = (rb->wr_idx + 1) % rb->buf_size;
    return true;
}

bool ring_buf_get(ring_buf_t *rb, uint8_t *ch)
{
    if(ring_buf_is_empty(rb))
    {
        return false;
    }
    *ch = rb->buffer[rb->rd_idx];
    rb->rd_idx = (rb->rd_idx + 1) % rb->buf_size;
    return true;
}

uint16_t ring_buf_write(ring_buf_t *rb, const uint8_t *data, uint16_t len)
{
    uint16_t i;
    uint16_t write_num = 0;
    for(i = 0; i < len; i++)
    {
        if(ring_buf_put(rb, data[i]))
        {
            write_num++;
        }
        else
        {
            break;
        }
    }
    return write_num;
}

uint16_t ring_buf_read(ring_buf_t *rb, uint8_t *data, uint16_t len)
{
    uint16_t i;
    uint16_t read_num = 0;
    for(i = 0; i < len; i++)
    {
        if(ring_buf_get(rb, &data[i]))
        {
            read_num++;
        }
        else
        {
            break;
        }
    }
    return read_num;
}

uint16_t ring_buf_count(ring_buf_t *rb)
{
    if(rb->wr_idx >= rb->rd_idx)
    {
        return rb->wr_idx - rb->rd_idx;
    }
    else
    {
        return (rb->buf_size - rb->rd_idx) + rb->wr_idx;
    }
}

uint16_t ring_buf_free_space(ring_buf_t *rb)
{
    return (rb->buf_size - 1U) - ring_buf_count(rb);
}

bool ring_buf_is_empty(ring_buf_t *rb)
{
    return rb->wr_idx == rb->rd_idx;
}

bool ring_buf_is_full(ring_buf_t *rb)
{
    return ((rb->wr_idx + 1U) % rb->buf_size) == rb->rd_idx;
}

void ring_buf_clear(ring_buf_t *rb)
{
    rb->wr_idx = 0;
    rb->rd_idx = 0;
}

