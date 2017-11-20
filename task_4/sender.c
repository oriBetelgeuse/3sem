#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define LAST_MESSAGE 255

int main()
{
	FILE *sender;
	int msqid, len = 0;
	char pathname[] = "/home/betelgeuse/Рабочий стол/Инфа/sender.c";
	key_t key;
	struct mymsgbuf
	{
		long mtype;
		char mtext[512];
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
	sender = fopen("sender.txt", "r");
	do
	{
		mybuf.mtype = 1;
		while((len = fread(mybuf.mtext, sizeof(char), 512, sender)) != 512 && !feof(sender));
		if (msgsnd(msqid, (struct msgbuf*) &mybuf, len, 0) < 0 )
		{
			printf("Can't send message to queue\n");
			msgctl(msqid, IPC_RMID, (struct msqid_ds*) NULL);
			exit(-1);
		}		
	} while(!feof(sender));
	fclose(sender);
	mybuf.mtype = LAST_MESSAGE;
	len = 0;
	if (msgsnd(msqid, (struct msgbuf*) &mybuf, len, 0) < 0)
	{
		printf("Can't send message to queue (last message)\n");
		msgctl(msqid, IPC_RMID, (struct msqid_ds*) NULL);
		exit(-1);
	}

	return 0;
}
