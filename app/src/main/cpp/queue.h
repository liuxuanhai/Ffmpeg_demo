//
// Created by yanchunlan on 2018/7/1.
//

#ifndef FFMPEG_DEMO_QUEUE_H
#define FFMPEG_DEMO_QUEUE_H

#endif //FFMPEG_DEMO_QUEUE_H


typedef struct _Queue Queue;

/**
 * 初始化队列
 */
Queue *queue_init(int size);

/**
 * 销毁队列
 */
void queue_free(Queue *queue);
/**
 * 获取下一个索引位置
 */
int queue_get_next(const Queue *queue, int current);

/**
 * 队列压入元素
 */
void *queue_push(Queue *queue);


/**
 * 弹出元素
 */
void *queue_pop(Queue *queue);