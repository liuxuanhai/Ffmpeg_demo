#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* thr_fun(void* arg){
	char* no = (char*)arg;
	int i = 0;
	for(; i < 10; i++){
		printf("%s thread, i:%d\n",no,i);
		if(i==5){
			//线程退出（自杀）
			pthread_exit(2);
			//他杀pthread_cancel			
		}
	}	
	return 1;
}

void main(){
	printf("main thread\n");
	//线程id
	pthread_t tid;
	//线程的属性，NULL默认属性
	//thr_fun，线程创建之后执行的函数
	pthread_create(&tid,NULL,thr_fun,"1");
	void* rval;
	//等待tid线程结束
	//thr_fun与退出时传入的参数，都作为第二个参数的内容
	pthread_join(tid,&rval);
	printf("rval:%d\n",(int)rval);
}
