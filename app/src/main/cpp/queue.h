//
// Created by yanchunlan on 2018/7/1.
//

#ifndef FFMPEG_DEMO_QUEUE_H
#define FFMPEG_DEMO_QUEUE_H

#endif //FFMPEG_DEMO_QUEUE_H

#include <android/log.h>
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"ycl",FORMAT,##__VA_ARGS__);
#define LOGE(FORMAT,...) __android_log_print(ANDROID_LOG_ERROR,"ycl",FORMAT,##__VA_ARGS__);


typedef struct _Queue Queue;

//分配队列元素内存的函数
typedef void *(*queue_full_func)();
//释放队列中元素所占用的内存  -- 定义函数
typedef void *(*queue_free_func)(void *elem);

/**
 * 初始化队列
 */
Queue *queue_init(int size,queue_full_func full_func);

/**
 * 销毁队列
 */
void queue_free(Queue *queue, queue_free_func free_func);

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