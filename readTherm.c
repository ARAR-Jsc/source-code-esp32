#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

char listDir()
{
	struct dirent *entry;
	DIR *dp;
	dp = opendir("./");
	if(dp == NULL)
	{
		printf("Not found dir\n");
		return 0;
	}
	else
	{
		while(entry = readdir(dp))
		{
			puts(entry->d_name);
		printf("dump dir: %s\n", entry->d_name);
		}
		closedir(dp);
		return 0;
	}
}

char* readFile(char* fileName)
{
	int len = 125;
	char buffer[256];
	FILE *fd=fopen(fileName, "r");
	char* line = (char*)malloc(100*sizeof(char));
	if(fd == NULL)
	{
		printf("Failed to open file\n");
		return 0;
	}
	while(fgets(buffer, len, fd) != NULL)
	{
		strcpy(line, buffer);
		line[strlen(line)]='\0';
	}
	printf("dump string: %s\n", line);
	fclose(fd);
	return line;
}


int main(int argc, char* argv[])
{
	listDir();
	int numSlave;
	char buffer[100];
	numSlave = atoi(readFile("/sys/devices/w1_bus_master1/w1_master_slave_count"));
	printf("dump num slave: %u\n", numSlave);
	if(numSlave >= 1)
	{
		printf("Found :%u w1 slave\n", numSlave);
		realpath("./", buffer);
		//readFile("/sys/devices/w1_bus_master1/28-/w1_slave");
		printf("dumpo buffer: %s\n", buffer);
	}
	else
	{
		printf("Not found thermel sensor\n");
	}
	return 0;
}
