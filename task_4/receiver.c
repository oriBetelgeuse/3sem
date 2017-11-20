#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>

#define LAST_MESSAGE 255

int main()
{
	FILE *receiver;
	int msqid, len, maxlen;
	char pathname[] = "/home/betelgeuse/Рабочий стол/Инфа/sender.c";
	key_t key;
	struct mymsgbuf
	{
		long mtype;
		char mtext[1024];
	} mybuf;

	if ((key = ftok(pathname, 0)) < 0)
	{
		printf ("Can't generate key\n");
		exit (-1);
	}
	if ((msqid = msgget (key, 0666 | IPC_CREAT)) < 0)
	{
		printf("Can't get msqid\n");
		exit (-1);
	}
	receiver = fopen("receiver.txt", "w");
	while(1)
	{
		maxlen = 1024;
		if ((len = msgrcv(msqid, (struct msgbuf*) &mybuf, maxlen, 0, 0)) < 0)
		{
			printf("Can't receive message from queue'n");
			exit(-1);
		}
		if (mybuf.mtype == LAST_MESSAGE)
		{
			msgctl(msqid, IPC_RMID, (struct msqid_ds*) NULL);
			break;
		}
		printf("%s ", mybuf.mtext);
		fwrite(mybuf.mtext, sizeof(char), 512, receiver);
	}
	fclose(receiver);

	return 0;
}

