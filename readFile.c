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
    while(breakWhile)
    {
        
        for(ii = 0; ii < 20; ii++)
		{
			if(fgets(line, 20, fd) != NULL)
			{
				//array20 = (char)line;
				line[strlen(line) - 1] = '\0';
				if(strcmp(line, array20[ii]) == 0)
				{
					printf("%s ", line);
				}
				else
				{
					SetConsoleTextAttribute(hStdOut, (6 | 0)); 
					printf("%s ", line);
					SetConsoleTextAttribute(hStdOut, (7 | 0)); // chu trang nen den (default)
				}
				strncpy(array20[ii], line, strlen(line));
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
    return 0;
}
