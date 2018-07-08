#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//file deal
#define MAX 400000
#define URLIDLEN 6
#define ELL_COLUMN 0
#define COO_LENGTH 2000000
#define TOPNUM 10

//page rank deal
void CalPageRank();
void findTop10();
void getNumforCoo(int source, int dest);
void getValueforCoo();

int coo_row[COO_LENGTH]={0};
int coo_column[COO_LENGTH] ={0};
float coo_values[COO_LENGTH] = {0};
float pagerank[MAX] ={1};

char *URLs[MAX];

int LinkOutNum[MAX]= {0};
int LinkInNum[MAX] ={0};

int COONUM = 0;
int URLNUM = 0; //Actual number of pages  (URLNUM<MAX)
int RelationNUM =0;

int top10ID[TOPNUM] = {0};
float top10Pagerank[TOPNUM] ={0};

int main(int argc, char* argv[])
{

    FILE *url, *top10;
    if((url = fopen(argv[1], "r")) == NULL)
    {
    	printf("file url.txt read failed!\n");
    	return -1;
	}
	if((top10 = fopen(argv[2], "w")) == NULL)
    {
    	printf("file top10.txt write failed!\n");
    	return -1;
	}
	char row[100], strSourceID[10], strDestID[10];
	int space=0, end=0, sourceID=0, destID=0;
	int formerId =0, latterId = 0;
	int flag = 0, n = 0;

	while(!feof(url))
    {
        fgets(row, 100, url);
        if (*row == '\n')
        {
            continue;
        }
        for (space = 0; row[space] != ' ' ; ++space);
        if(row[space+1] == 'h' | row[space+1] == 'n') // Read the respective ID of a Web Page
        {
            for (end = space; row[end] != '\n' ; ++end);
            if ((URLs[n] = malloc(end - space - 1)) == NULL)
                    return -1;
            memset(URLs[n], 0, end - space - 1);
            strncpy(URLs[n], row + space + 1, end - space - 1);
            n++;
            URLNUM++;
            //continue;
        }
        if(row[space+1] != 'h' && row[space+1] != 'n' && row[space+1] != '\n')  // Read Link Relationship: Source(int)->Destination(int)
        {
            memset(strSourceID, 0, 10);
            memset(strDestID, 0, 10);
            strncpy(strSourceID, row, space);
            formerId = atoi(strSourceID);
            for (end = space; row[end] != '\n' ; ++end);
                strncpy(strDestID, row+space+1, end-space-1);
            latterId = atoi(strDestID);
            //for URL-WZW ID(from 0 to MAX-1: 28w-1)
            //sourceID = latterId;
            //destID = formerId;

            //for URL-GCY ID(from 0 to MAX-1: 20w)
            sourceID = formerId;
            destID = latterId;

            LinkOutNum[sourceID]++;
            LinkInNum[destID]++;
            getNumforCoo(sourceID, destID);
        }

    }

    getValueforCoo();
    printf("Now start cal Pagerank\n");
    CalPageRank();
    printf("Now start find TOP10 URL\n");
    findTop10();
    printf("TOP10 URLs Saved...\n");
    for(int i=0;i<10;i++)
    {
        //char buf[] = *URLs[ top10ID[i] ];
        fwrite(URLs[ top10ID[i] ], strlen(URLs[ top10ID[i] ]),1, top10 );
        fputc(' ',top10);
       // printf("**%d: %d | %.16lf\n",i,top10ID[i],top10Pagerank[i]);
        fprintf(top10,"%.16lf",top10Pagerank[i]);
        fputc('\n',top10);
    }

    for(int i =0;i<MAX;i++)
    {
        free(URLs[i]);
    }
    fclose(url);
    fclose(top10);

}

void getNumforCoo(int source, int dest)
{
    static int coo_num = 0;
    if(coo_num == COO_LENGTH)
    {
        printf("ERROR : the coo has been full \n");
    }
    else
    {
        coo_column[coo_num] = source;
        coo_row[coo_num] = dest;
        coo_num++;
        RelationNUM++;
       // printf("coo length: %d \n",coo_num);
        //printf("linknum = %d  |  value: %f \n",linkNum,value);
    }
}

void getValueforCoo()
{
    for(int i=0; i<RelationNUM;i++)
    {
        int sourceID = coo_column[i];   //coo_column[] from 0 to (MAX-1)
        int linkNum = LinkOutNum[ sourceID ];
        float value = 0.85 * 1.0 / linkNum;  //other value = 0.15/MAx
        coo_values[i] = value;
       // printf("source: %d | dest: %d | linkOutNum: %d | value:%.16f\n", coo_column[i],coo_row[i],LinkOutNum[sourceID],value);

    }
}

// Sequential Cal when only using coo
void CalPageRank()
{
    //printf("  **  ---  **   \n");
    if(ELL_COLUMN != 0)
    {
        printf("ERROR: When ELL_COLUMN !=0, Can not use this function!");
        return;
    }

    float lastrank[MAX] ={0};

    float limitation = 0.00001;
    double distance = 1;

    float comf = 15/URLNUM;
    float comp = 0;

    //The actual size of the matrix calculated = URLNUM
    while(distance>=limitation * limitation)
    {
    	printf("distance is : %.3lf\n", distance); 
        comp = 0;
        for(int i =0; i<URLNUM; i++)
        {
            lastrank[i] = pagerank[i];
            pagerank[i] = 0;
            comp = comp + lastrank[i];
        }
        for(int i =0; i<URLNUM; i++)
        {
            pagerank[i] = comp * comf;
        }
        for(int i =0; i<COO_LENGTH;i++)
        {
            pagerank[ coo_row[i] ] = coo_values[i] * lastrank[ coo_column[i] ] + pagerank[ coo_row[i] ];
            pagerank[ coo_row[i] ] = pagerank[ coo_row[i] ] - comf*lastrank[ coo_column[i] ];
        }
        distance = 0;
        for(int i =0; i<URLNUM; i++)
        {
            distance = (lastrank[i] - pagerank[i]) * (lastrank[i] - pagerank[i]) + distance;
           // printf("distance = %.16lf\n",distance);
        }
    }
    if(distance < limitation * limitation)
    {
        printf("cal Finished...\n\n");
    }

}

void findTop10()
{
    //printf("\n**TOP 10**\n");
    int i =0;
    int j =0;
    for(i =0; i<MAX; i++)
    {
        for(j=0;j<TOPNUM;j++)
        {
            if(pagerank[i]>top10Pagerank[j])
            {
                for(int q=TOPNUM-1;q>j;q--)
                {
                    top10Pagerank[q]= top10Pagerank[q-1];
                    top10ID[q] = top10ID[q-1];
                }
                top10Pagerank[j] = pagerank[i];
                top10ID[j]=i;   //top10ID save the URL-ID from 0 to MAX-1
                break;
            }
        }
    }
    //printf("The top 10 URL is:\n");
    for(int i =0; i<TOPNUM;i++)
    {
        //printf("**%d: %d | %.16lf\n",i,top10ID[i],top10Pagerank[i]);
        //printf("  URL:%s\n",URLs[ top10ID[i] ]);

    }
}
