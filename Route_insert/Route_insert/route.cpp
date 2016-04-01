#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_VEX 600
#define LEN sizeof(struct chain_node)
#define MAX_LINE_LEN 4000
//#define INF 10000

typedef struct _Maps
{
	int weight[MAX_VEX][MAX_VEX];
	int route[MAX_VEX][MAX_VEX];
	int visited[MAX_VEX];
	int max;
}Maps;

typedef struct chain_node
{
	int vex;
	int route;
	int weight;
	chain_node *next;
}Node;

void initfunc(char **topo, int edge_num, Maps *graph)
{
	int maxvex;
	int i, route, start, end, weight;

	memset(graph->weight, -1, sizeof(int) * MAX_VEX * MAX_VEX);
	memset(graph->route, -1, sizeof(int) * MAX_VEX * MAX_VEX);
	memset(graph->visited, 0, sizeof(int) * MAX_VEX);
	graph->max = 0;

	for (i = 0;i < edge_num;i++)
	{
		sscanf(topo[i], "%d,%d,%d,%d", &route, &start, &end, &weight);
		if ((start > maxvex) || (end > maxvex))
			if (start > maxvex)
				maxvex = start;
			else maxvex = end;
		graph->weight[start][end] = weight;
		graph->route[start][end] = route;		
	}
};

int read_file(char ** const buff, const unsigned int spec, const char * const filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Fail to open file %s.\n", filename);
        return 0;
    }
    printf("Open file %s OK.\n", filename);

    char line[MAX_LINE_LEN + 2];
    unsigned int cnt = 0;
    while ((cnt < spec) && !feof(fp))
    {
        line[0] = 0;
        fgets(line, MAX_LINE_LEN + 2, fp);
        if (line[0] == 0)   continue;
        buff[cnt] = (char *)malloc(MAX_LINE_LEN + 2);
        strncpy(buff[cnt], line, MAX_LINE_LEN + 2 - 1);
        buff[cnt][4001] = 0;
        cnt++;
    }
    fclose(fp);
    printf("There are %d lines in file %s.\n", cnt, filename);

    return cnt;
}

int demand2num(char *line, int *target)
{
	int i = 0;
	char *temp_str1[5], *temp_str2[55];

	while((temp_str1[i] = strtok(line, ",")) != NULL)
	{
		i++;
		line = NULL;
	}
	target[0] = atoi(temp_str1[0]);
	target[1] = atoi(temp_str1[1]);

	i = 0;
	while((temp_str2[i] = strtok(temp_str1[2], "|")) != NULL)
	{
		target[i + 2] = atoi(temp_str2[i]);
		i++;
		temp_str1[2] = NULL;
	}

	return 0;
}

int main()
{
	int edge_num, i;
	char *topo[5000];	
	char *demand;
	int demand_num;
	clock_t start, finish;

	start = clock();
	edge_num = read_file(topo, 5000, "topo.csv");
	demand_num = read_file(&demand, 1, "demand.csv");
	 //for (i = 0;i < edge_num;i++)
	 //	printf("%s", topo[i]);	
	search_route(topo, edge_num, demand);
	finish = clock();

	printf("%f ms used", (double)(finish - start));
	getchar();

	return 0;
}