#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
/* this library to use sleep function */
#include <unistd.h>

char listDir(const char* dirName, char** storeFolderName)
{
	struct dirent *entry;
	DIR *dp;
	char i = 0;
	char* matchFolder = NULL;
	dp = opendir(dirName);
	if(dp!=NULL)
	{
		while(entry = readdir(dp))
		{
			matchFolder = strstr(entry->d_name, "28-");
			if(matchFolder != NULL)
			{
				strcat(storeFolderName[i], "/sys/bus/w1/devices/");
				strcat(storeFolderName[i], entry->d_name);
				strcat(storeFolderName[i], "/w1_slave");
				i++;
			}
		}
		closedir(dp);
		return (i);
	}
	else
	{
		printf("open dir failed\n");
		return -1;
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
	////printf("dump string: %s\n", line);
	fclose(fd);
	return line; //concurrent free line pointer
}


float getTemp(char* fileName)
{
	char lineTemp[10];
	float tempVal;
	char i;
	strcpy(lineTemp, strchr(readFile(fileName), '='));
	// remove '=' in lineTemp
	for(i = 0; i< sizeof(lineTemp) - 1; i++)
	{
		lineTemp[i] = lineTemp[i+1];
	}
	lineTemp[sizeof(lineTemp)]='\0';
	tempVal = (float)atoi(lineTemp)/1000;
	//printf("dump lineTemp: %2.3f\n",tempVal);
	return tempVal;

}


int main(int argc, char* argv[])
{
	char** getNameDS=NULL;
	char maxSlave = 5;
	char totalFile = 0;
	char i;
	int numSlave;
	
	getNameDS = (char**)malloc(maxSlave*sizeof(char*));
	
	for(i = 0; i < maxSlave; i++)
	{
		getNameDS[i] = (char*)malloc(55*sizeof(char));
	}
	
	totalFile = listDir("/sys/bus/w1/devices/", getNameDS);
	
	numSlave = atoi(readFile("/sys/devices/w1_bus_master1/w1_master_slave_count"));

	printf("number of slave: %u\n", numSlave);

	if(totalFile != numSlave)
	{
		printf("Warning, number of slave and number of result files are different\n");
	}	

	i = 0; //reset i value

	if(numSlave >= 1)
	{
		printf("Found :%u w1 slave\n", numSlave);
		printf("this is function read temp unstop\n");
		while(i <= 1)
		{
			printf("temp at sensor %u: %2.3f\n", i, getTemp(getNameDS[i]));
			i++;
			if(i >= totalFile)
			{
				sleep(2);
				i = 0;
			}
		}	
	}
	else
	{
		printf("Not found thermel sensor\n");
	}
	//free pointer
	for(i = 0; i < maxSlave; i++)
	{
		free(getNameDS[i]);
	}
	free(getNameDS);
	return 0;
}
