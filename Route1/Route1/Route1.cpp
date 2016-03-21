#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LEN sizeof(struct chain_node)
#define MAX_LINE_LEN 4000

typedef struct chain_node
{
	int point;
	int weight;
	int route;
	struct chain_node *next;
}Node;


int topo2num(char *line, int *num)
{
	int i = 0;
	char *temp_str[10];

	while((temp_str[i] = strtok(line, ",")) != NULL)
	{
		num[i] = atoi(temp_str[i]);
		i++;
		line = NULL;
	}

	return 0;
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

int initfunc(Node **head, Node **ptr, int *target, int *visited)
{
	int i;

	for (i = 0;i < 600;i++)
	{
		head[i] = NULL;
		ptr[i] = NULL;
		visited[i] = 0;
	}
	for (i = 0;i < 52;i++)
		target[i] = -1;

	return 0;
}

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

int creat_chain(Node **head, Node **ptr, char *topo[5000], int edge_num)
{
	int i, max = 0;
	int num[4];
	Node *temp[600];
	int visited[600] = {0};

	for (i = 0;i < edge_num;i++)
	{
		topo2num(topo[i], num);
		if(num[1] > max)
			max = num[1];
		if (visited[num[1]] == 0)
		{
			head[num[1]] = (Node *)malloc(LEN);
			ptr[num[1]] = (Node *)malloc(LEN);

			head[num[1]]->point = num[1];
			head[num[1]]->weight = -1;
			head[num[1]]->route = -1;
			head[num[1]]->next = ptr[num[1]];

			ptr[num[1]]->point = num[2];
			ptr[num[1]]->weight = num[3];
			ptr[num[1]]->route = num[0];
			ptr[num[1]]->next = NULL;

			visited[num[1]] = 1;
		}
		else
		{
			temp[num[1]] = (Node *)malloc(LEN);
			temp[num[1]]->point = num[2];
			temp[num[1]]->weight = num[3];
			temp[num[1]]->route = num[0];
			temp[num[1]]->next = NULL;
			ptr[num[1]]->next = temp[num[1]];
			ptr[num[1]] = temp[num[1]];
		}
	}

	max++;
	for (i = 0;i < max;i++)
	{
		if (head[i] == NULL)
		{
			head[i] = (Node *)malloc(LEN);
			head[i]->point = i;
			head[i]->weight = -1;
			head[i]->route = -1;
			head[i]->next = NULL;
		}
		ptr[i] = head[i];
	}

	return max;
}

void print_chain(Node **head, int max)
{
	int i;
	Node *p;

	for (i = 0;i < max;i++)
	{
		p = head[i];
		while (p != NULL)
		{
			printf("%d ", p->point);
			p = p->next;
		}
		printf("\n");
	}
}

void DFS(Node **head, int pre_weight, int start, int *target, int *visited, int *result_path, int *result_cost)
{
	Node *ptr;
	int i, flag = 1;
	static int count = 0;
	static int cost = 0;
	static int path_count = 0;
	static int path_stack[5000] = {0};

	ptr = head[start];
	visited[ptr->point] = 1;
	if (ptr->point == target[1])
	{
		i = 2;
		flag = 1;
		while (target[i] != -1)
		{
			if (visited[target[i]] != 1)
			{
				flag = 0;
				break;
			}				
			i++;
		}
		if (flag && (cost < *result_cost))
		{
			for (i = 0;i < count;i++)
				result_path[i] = path_stack[i];
			result_path[i] = -1;
			*result_cost = cost;
			path_count++;
			//for (i = 0; i < 45; i++)
			//	if (visited[i] == 1)
			//		printf("%d ", i);
			//printf("\n");
		}
		
	}
	else
		while (ptr->next != NULL)
		{
			ptr = ptr->next;
			if (visited[ptr->point] == 0)
			{
				path_stack[count++] = ptr->route;
				cost += ptr->weight;
				DFS(head, ptr->weight, ptr->point, target, visited, result_path, result_cost);
			}
		}
	count--;
	cost -= pre_weight;
	visited[start] = 0;
}

void search_route(char *topo[5000], int edge_num, char *demand)
{
    Node *head[600], *ptr[600];
	int target[52], visited[600], max;
	int path[100] = {0};
	int cost = 100000;

	int i = 0;

	initfunc(head, ptr, target, visited);	
	demand2num(demand, target);
	max = creat_chain(head, ptr, topo, edge_num);
	DFS(head, 0, target[0], target, visited, path, &cost);

	
	if (cost < 100000)
	{
		while (path[i] != -1)
		{
			record_result(path[i]);
			i++;
		}		
	}
	

    // unsigned short result[] = {2, 6, 3};//Ê¾ÀýÖÐµÄÒ»¸ö½â

    // for (int i = 0; i < 3; i++)
    //     record_result(result[i]);
}

int main()
{
	int edge_num, i;
	char *topo[5000];	
	char *demand;
	int demand_num;

	edge_num = read_file(topo, 5000, "topo.csv");
	demand_num = read_file(&demand, 1, "demand.csv");
	// for (i = 0;i < edge_num;i++)
	// 	printf("%s", topo[i]);
	//print_chain(head, max);
	search_route(topo, edge_num, demand);

	return 0;
}

