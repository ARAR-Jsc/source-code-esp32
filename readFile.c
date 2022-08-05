#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

void red()
{
	printf("\033[0;31m");
}

void yellow()
{
	printf(" Yellow \033[0;33m");
}

void reset()
{
	printf("\033[0m");
}
                
int main()
{
    	
	//system("COLOR 06");
	/////////////// change color //////////////////
	
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    //SetConsoleTextAttribute(hStdOut, (FOREGROUND_BLUE | BACKGROUND_GREEN));
    //printf("I'm BLUE ");
    //SetConsoleTextAttribute(hStdOut, (7 | 0)); // chu trang nen den (default)
    //printf("and I'm RED ");
    //SetConsoleTextAttribute(hStdOut, (BACKGROUND_GREEN));
    //printf("and we both have green background!\r\n");
	///////////////////
	
	
	char line[20];
    char array20[20][20] = {0};
    int numberOfLine = 0;
    FILE *fd = fopen("dataFile.txt", "r");
    unsigned char breakWhile = 1;
    unsigned char ii = 0;
    unsigned char j = 0;
    while(breakWhile)
    {
        
        for(ii = 0; ii < 20; ii++)
		{
			if(fgets(line, 20, fd) != NULL)
			{
				//array20 = (char)line;
				//line[strlen(line) - 1] = '\0';
				line[0] = line[2];
				line[1] = line[3];
				line[2] = '\0';
				
				////convert line to binary
				for(j = 0; j < 2; j++)
				{
					if((line[j] == 'E') || (line[j] == 'F'))
					{
						line[j] = '1';
					}
					else if((line[j] == '0') || (line[j] == '8'))
					{
						line[j] = '0';
					}
				}
				
				///// apply color for changed line
				if(strcmp(line, array20[ii]) == 0)
				{
					printf("%s   ", line);
				}
				else
				{
					SetConsoleTextAttribute(hStdOut, (6 | 0)); 
					printf("%s   ", line);
					SetConsoleTextAttribute(hStdOut, (7 | 0)); // chu trang nen den (default)
				}	
				strncpy(array20[ii], line, 2);
				numberOfLine ++;
			}
			else
			{
				breakWhile = 0;
				break;
			}
		}
		
		printf("\n");
    }
    close(fd);
    printf("total line: %u\n", numberOfLine);
    getchar();
    return 0;
}
