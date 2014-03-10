#include<stdio.h>
#include<fcntl.h>
#include<sys/types.h>
#include<error.h>
#include<sys/stat.h>
#include<time.h>

#define O_FIFO "/dev/rtf1"
#define IP_FIFO "/dev/rtf0"
#define CNT_FIFO "/dev/rtf2"

struct mn
{
	float ll,diff;
};
struct _datapacket
{
	char mode;
	struct mn month[13];
}dp;

int main()
{
	FILE *fp;
	struct tm *tm_ptr;
	time_t the_time;
	
	
	int fi,fo,fc,dig;
	unsigned long int ticks=0;
	char rdn=0,r=0,cmd=0,month;
	float conv=0.00,dnow=0.00,cv;

	time(&the_time);
	tm_ptr=gmtime(&the_time);
	month=tm_ptr->tm_mon+1;
		
	printf("\n\n\t\tTEMPERATURE\n");
	fi=open(IP_FIFO,O_RDONLY|O_NONBLOCK);	
	fo=open(O_FIFO,O_NONBLOCK|O_WRONLY);
	fc=open(CNT_FIFO,O_NONBLOCK|O_WRONLY);
	fp=fopen("data.dat","rb");
	if(!(fi&&fo&&fc))
		printf("\nError !\n");
		
	if(!fp)
	{
		perror("mid.c : file read error !\n");
		return -1;
	}
	while(1)
	{
		++ticks;
	if(ticks == 1<<31 || !r)
	if(fread(&dp,sizeof(dp),1,fp)!=1){
		perror("mid.c : read error !");
		fclose(fp);
		return -1;
	}
	r=1;
	if(read(fi,&rdn,1)<0)
	{	perror("fifo empty");
		continue;
	}	
	dnow=(rdn/5.0f);
	month&=dp.mode;
	if(dnow<dp.month[month].ll)
		cmd=0;
	else
	{
		conv=((dnow-dp.month[month].ll)/dp.month[month].diff);
		dig=(int)conv;
		cv=(float)dig;
		if(conv>cv)++dig;
		if(dig>5)dig=5;
			cmd=1<<(dig-1);
		}
		write(fo,&cmd,1);
		write(fc,&r,1);
		printf("%4.1f",dnow);
		usleep(10000000);
	}
	close(fi);
	close(fo);
	close(fc);
	fclose(fp);
	return 0;
}
