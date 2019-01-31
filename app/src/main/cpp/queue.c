//
// Created by yanchunlan on 2018/7/1.
//

#include <malloc.h>
#include "queue.h"


/**
 * 队列，这里主要用于存放AVPacket的指针
 * 这里，使用生产者消费模式来使用队列，至少需要2个队列实例，分别用来存储音频AVPacket和视频AVPacket
 *  1.生产者：read_stream线程负责不断的读取视频文件中AVPacket，分别放入两个队列中
	2.消费者：
	1）视频解码，从视频AVPacket Queue中获取元素，解码，绘制
	2）音频解码，从音频AVPacket Queue中获取元素，解码，播放
 */
struct _Queue {
    int size;    //长度

    //任意类型的指针数组，这里每一个元素都是AVPacket指针，总共有size个
    //AVPacket **packets;
    void **tab;

    //push或者pop元素时需要按照先后顺序，依次进行
    int next_to_write;
    int next_to_read;

    int *ready;

};

/**
 * 初始化队列
 */
Queue *queue_init(int size,queue_full_func full_func) {
    Queue *queue = malloc(sizeof(Queue));
    queue->size = size;
    queue->next_to_read = 0;
    queue->next_to_write = 0;
    //数组开辟空间  *queue->tab 子类指针大小
    queue->tab = malloc(sizeof(*queue->tab) * size);
    // 数组元素指定大小
    for (int i = 0; i < size; ++i) {
//        queue->tab[i] = malloc(sizeof(*queue->tab));
        queue->tab[i] = full_func;
    }
    return queue;
};

/**
 * 销毁队列
 */
void queue_free(Queue *queue, queue_free_func free_func) {
    for (int i = 0; i < queue->size; ++i) {
        //销毁队列的元素，通过使用回调函数,具体的释放avpacket
        free_func((void *) queue->tab[i]);
//        free(queue->tab[i]);
    }
    free(queue->tab);
    free(queue);
};

/**
 * 获取下一个索引位置
 */
int queue_get_next(const Queue *queue, int current) {
    return (current + 1) % queue->size;
};

/**
 * 队列压入元素
 * 返回下一个压入位置
 */
void *queue_push(Queue *queue,pthread_mutex_t *mutex,pthread_cond_t *cond) {
    int current = queue->next_to_write;
    int next_to_write;
    for (;;) {
        //下一个要读的位置等于下一个要写的，等我写完，在读
        //不等于，就继续 ,可能只执行一次就break了
        next_to_write=queue_get_next(queue, current);
        if (next_to_write != queue->next_to_read) {
            break;
        }
        // 阻塞
        pthread_cond_wait( cond,mutex);
    }
    queue->next_to_write = next_to_write;
    LOGI("queue_push queue:%#x, %d",(unsigned int)queue,current);
    //通知
    pthread_cond_broadcast(cond);
    return queue->tab[current];
}


/**
 * 弹出元素
 */
void *queue_pop(Queue *queue,pthread_mutex_t *mutex,pthread_cond_t *cond) {
    int current = queue->next_to_read;
    for (;;) {
        if (queue->next_to_read != queue->next_to_write) {
            break;
        }
        // 阻塞
        pthread_cond_wait(cond,mutex);
    }
    queue->next_to_read = queue_get_next(queue, current);
    LOGI("queue_pop queue:%#x, %d",(unsigned int)queue,current);
    //通知
    pthread_cond_broadcast(cond);

    return queue->tab[current];
}

