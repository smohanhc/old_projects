#include<stdio.h>
#include<fcntl.h>
#include<error.h>
#include<sys/types.h>

#define DAT "data.dat"



struct mn
{
	float ll,diff;
};

struct _datapacket
{
	char mode;
	struct mn month[13];	
}dp;


void chmode()
{
	int ch;
	printf("\n\t\t\t\tCHANGE MODE\n\t\t\t1)Normal Preferance\n\t\t\t2)Monthly Preferance\n\t\t\t3)Exit\n\n\t\tEnter Choice :");
	scanf("%d",&ch);
	switch (ch)
	{
		case 1:
			dp.mode = 0;
			break;
		case 2:
			dp.mode = 1;
			break;
	}
}
void chnset()
{
	printf("\n\t\t\t\tCHANGING NORMAL SETTINGS\n\t\t\tEnter lower temperarure :");
	scanf("%f",&dp.month[0].ll);
	printf("\n\t\t\tEnter temperature difference :");
	scanf("%f",&dp.month[0].diff);
}
void chmset()
{
	int ch;char con;
	printf("\n\t\t\t\tCHANGING MONTHLY SETTINGS\n");
	do
	{
		printf("1-Jan 2-Feb 3-Mar 4-Apr 5-May 6-Jun 7-Jul 8-Aug 9-Sep 10-Oct 11-Nov 12-Dec\n");
		do
		{
			printf("\t\tEnter Choice :");
			scanf("%d",&ch);
		}while(ch<1||ch>12);
		printf("\t\tEnter lower temperarure :");
		scanf("%f",&dp.month[ch].ll);
		printf("\n\t\tEnter temperature difference :");
		scanf("%f",&dp.month[ch].diff);
		do
		{
			printf("\n\tDo you want to continue (y/n):");
			scanf(" %c",&con);
		}while(con!='n'&&con!='y'&&con!='Y'&&con!='N');
	}while(con!='n'&&con!='N');
}
void chtset()
{
	int ch;
	printf("\n\t\t\t\tCHANGE TEMPERATURE SETTINGS\n\t\t\t1)Normal Settings\n\t\t\t2)Monthly Settings\n\t\t\t3)Exit\n\n\t\tEnter Choice :");	
	scanf("%d",&ch);
	switch (ch)
	{
		case 1:
			chnset();
			break;
		case 2:
			chmset();
			break;
	}
}
void chset()
{
	int ch;
	printf("\n\t\t\t\tCHANGE SETTINGS\n\t\t\t1)Change mode\n\t\t\t2)Change Temperature Settings\n\t\t\t3)Exit\n\n\t\tEnter Choice :");
	scanf("%d",&ch);
	switch(ch)
	{
		case 1:
			chmode();
			break;
		case 2:
			chtset();
			break;
	}
}
void vcset()
{
	int i;
	printf("\t\t\t\tCURRENT SETTINGS\n\n\tMode :");
	if(dp.mode==0)
		printf("Normal");	
	else
		printf("Monthly");
	printf("\n\t \tLower Temperature\t\tDifference\n");
	for(i=0;i<13;i++)
	{
		printf("\n\t%d\t%3.2f               \t\t%3.2f",i,dp.month[i].ll,dp.month[i].diff);
	}
	printf("\n\n");
}	
int main()
{
	int ch;
	FILE *fp;
	fp=fopen(DAT,"rb");
	if(!fp)
	{
		perror("main : Cannot access data file !\n");
		return -1;
	}
	
	if(fread((void *)&dp,sizeof(dp),1,fp)!=1)
	{
		perror("main : Corrupted data file !\n");
		fclose(fp);
		return -1;
	}
	
	do
	{
		printf("\n\n\t\t\t\t iROOM CONTROLLER\n\t\t\t1)Change Settings\n\t\t\t2)View Current Settings\n\t\t\t3)Bye Bye\n\n\t\t\tEnter choice :");
		scanf("%d",&ch);
		switch(ch)
		{
			case 1:
				chset();
				break;
			case 2:
				vcset();
				break;
		}
	}while(ch!=3);
	fclose(fp);
	remove(DAT);
	fp=fopen(DAT,"wb");
	if(fwrite((void *)&dp,sizeof(dp),1,fp)!=1)
		perror("main : file write error !\n");
	
	fclose(fp);
	return 0;
}
