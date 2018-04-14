#ifndef _CMD_QUEUE
#define _CMD_QUEUE

typedef unsigned char qdata;
typedef unsigned short qsize;


typedef struct _QUEUE
{
	qsize _head; //队列头
	qsize _tail;  //队列尾
	qdata _data[QUEUE_MAX_SIZE];	//队列数据缓存区
}QUEUE;


extern void queue_reset(void);
extern void queue_push(qdata _data);
extern qsize queue_find_cmd(qdata *cmd,qsize buf_len);

#endif
