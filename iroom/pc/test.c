#include<stdio.h>
#include<fcntl.h>
#include<error.h>
#include<sys/types.h>

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
	FILE *fd;
	fd=fopen("data.dat","wb");
	if(!fd)
	{
		perror("main : Already exists !\n");
		return;
	}
	fwrite(&dp,sizeof(dp),1,fd);
	fclose(fd);
}