#ifndef __RING_BUF_H
#define __RING_BUF_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t *buffer;
    uint16_t buf_size;
    uint16_t wr_idx;
    uint16_t rd_idx;
} ring_buf_t;

/**
 * @brief 初始化环形缓冲区，外部传入内存数组
 * @param rb 环形缓冲区句柄
 * @param mem 用户提供的内存数组
 * @param mem_size 数组字节大小
 */
void ring_buf_init(ring_buf_t *rb, uint8_t *mem, uint16_t mem_size);

/**
 * @brief 写入单字节
 * @retval true成功，false缓冲区满
 */
bool ring_buf_put(ring_buf_t *rb, uint8_t ch);

/**
 * @brief 读取单字节
 * @param ch 输出读到的数据
 * @retval true成功，false缓冲区空
 */
bool ring_buf_get(ring_buf_t *rb, uint8_t *ch);

/**
 * @brief 批量写入多个字节
 * @return 实际成功写入字节数
 */
uint16_t ring_buf_write(ring_buf_t *rb, const uint8_t *data, uint16_t len);

/**
 * @brief 批量读取多个字节
 * @return 实际读到的字节数
 */
uint16_t ring_buf_read(ring_buf_t *rb, uint8_t *data, uint16_t len);

/**
 * @brief 获取当前有效数据个数
 */
uint16_t ring_buf_count(ring_buf_t *rb);

/**
 * @brief 获取剩余可写空间
 */
uint16_t ring_buf_free_space(ring_buf_t *rb);

bool ring_buf_is_empty(ring_buf_t *rb);
bool ring_buf_is_full(ring_buf_t *rb);

/**
 * @brief 清空环形缓冲区
 */
void ring_buf_clear(ring_buf_t *rb);

#endif
