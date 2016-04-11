#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LEN sizeof(struct chain_node)
#define MAX_LINE_LEN 4000
#define MAX_VEX 600
#define INFI 10000


typedef struct chain_node
{
	int point;	
	struct chain_node *next;
}Node;

typedef struct _Maps
{
	int weight[MAX_VEX][MAX_VEX];
	int route[MAX_VEX][MAX_VEX];
	int visited[MAX_VEX];
	int max;
}Maps;

typedef struct _Dijsktra_path
{
	int path[MAX_VEX];
	int weight;
	int flag;
}Dij_Path;

clock_t begin_t, finish_t;
Dij_Path DPath[MAX_VEX][MAX_VEX];

int topo2num(char *line, int *num)
{
	int i = 0;
	char *temp_str[10];

	while ((temp_str[i] = strtok(line, ",")) != NULL)
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

	while ((temp_str1[i] = strtok(line, ",")) != NULL)
	{
		i++;
		line = NULL;
	}
	target[0] = atoi(temp_str1[0]);
	target[1] = atoi(temp_str1[1]);

	i = 0;
	while ((temp_str2[i] = strtok(temp_str1[2], "|")) != NULL)
	{
		target[i + 2] = atoi(temp_str2[i]);
		i++;
		temp_str1[2] = NULL;
	}

	return i + 2;
}

int initfunc(Node **head, int *target, char **topo, int edge_num, Maps *graph)
{
	int maxvex = 0;
	int i, route, start, end, weight;

	//init Map
	memset(graph->weight, INFI, sizeof(int) * MAX_VEX * MAX_VEX);
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
	graph->max = maxvex;

	//init nodes
	for (i = 0;i < 60;i++)
	{
		target[i] = -1;
	}	

	for (i = 0;i < MAX_VEX;i++)
		head[i] = NULL;

	//init Dij
	memset(DPath, -1, sizeof(_Dijsktra_path) * MAX_VEX * MAX_VEX);

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

void creat_chain(Node **head, int *target, int nodes)
{
	int i, j;
	Node *ptr[MAX_VEX];
	
	//S node
	head[target[0]] = (Node *)malloc(LEN);
	ptr[target[0]] = (Node *)malloc(LEN);
	head[target[0]]->point = target[0];
	head[target[0]]->next = ptr[target[0]];
	for(i = 2;i < nodes;i++)
	{
		ptr[target[0]]->point = target[i];
		ptr[target[0]]->next = (Node *)malloc(LEN);
		ptr[target[0]] = ptr[target[0]]->next;
	}
	ptr[target[0]]->point = target[1];
	ptr[target[0]]->next = NULL;	

	//V nodes
	for (i = 2;i < nodes;i++)
	{
		head[target[i]] = (Node *)malloc(LEN);
		ptr[target[i]] = (Node *)malloc(LEN);
		head[target[i]]->point = target[i];
		head[target[i]]->next = ptr[target[i]];

		for(j = 2;j < nodes;j++)		
			if(i != j)
			{
				ptr[target[i]]->point = target[j];
				ptr[target[i]]->next = (Node *)malloc(LEN);
				ptr[target[i]] = ptr[target[i]]->next;
			}			
		ptr[target[i]]->point = target[1];
		ptr[target[i]]->next = NULL;		
	}

	//T node
	head[target[1]] = (Node *)malloc(LEN);
	head[target[1]]->point = target[1];
	head[target[1]]->next = NULL;
}

void print_chain(Node **head, int max)
{
	int i;
	Node *p;

	for (i = 0;i < MAX_VEX;i++)
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

int Dijkstra(Maps *graph, int *target, int S, int T, int *path)
{
	int i, j, min, k, temp;
	int D[MAX_VEX], final[MAX_VEX], temp_path[MAX_VEX], reverse_path[MAX_VEX];
	//int lenth = 40;

	//init
	memset(path, -1, sizeof(int) * MAX_VEX);
	memset(temp_path, -1, sizeof(int) * MAX_VEX);
	memset(reverse_path, -1, sizeof(int) * MAX_VEX);
	memset(D, INFI, sizeof(int) * MAX_VEX);
	memset(final, 0, sizeof(int) * MAX_VEX);
	for (i = 0; i < graph->max + 1; i++)
	{
		D[i] = graph->weight[S][i];
		if (D[i] < INFI)
			temp_path[i] = S;
	}
	D[S] = 0;
	final[S] = 1;
	for (i = 0; i < MAX_VEX; i++)
		if (graph->visited[i] == 1)
			final[i] = 1;
	for (i = 0;i < 60;i++)
	{
		if (target[i] == -1)
			break;
		if (target[i] != T)
			final[target[i]] = 1;
	}

	//main loop
	for (i = 0;i < graph->max + 1;i++)
	{
		min = INFI;
		k = S;
		for (j = 0;j <graph->max + 1;j++)
		{
			if (!final[j] && D[j] < min)
			{
				k = j;
				min = D[j];
			}
		}
		final[k] = 1;
		for (j = 0;j <graph->max + 1;j++)
		{
			if (!final[j] && (min + graph->weight[k][j] < D[j]))
			{
				D[j] = min + graph->weight[k][j];
				temp_path[j] = k;
				//if (D[j] > lenth)
				//	return INFI;	
			}
		}

		if (final[T] == 1)
			break;
	}

	if (final[T] != 1)
		return INFI;

	//output
	reverse_path[0] = T;
	temp = T;
	i = 1;
	while (1)
	{
		if (temp == S)
			break;
		reverse_path[i] = temp_path[temp];
		temp = temp_path[temp];
		i++;
	}
	temp = i - 1;
	for (j = 0;j < i;j++)
	{
		path[j] = reverse_path[temp];
		temp--;
	}
	return D[T];
}

void DFS(Node **head, Maps *graph, int pre_weight, int start, int *target, int *result_path, int *result_cost)
{
	Node *ptr;
	int i, j, flag = 1;
	int temp, pre_vex, cnt;
	int path_start, path_end;
	int Dij_flag;

	static int count = 0;
	static int cost = 0;
	// static int path_count = 0;
	static int path_stack[5000] = {0};
	static int path[MAX_VEX];	

	ptr = head[start];
	graph->visited[ptr->point] = 1;
	if (ptr->point == target[1])
	{
		i = 2;
		flag = 1;
		while (target[i] != -1)
		{
			if (graph->visited[target[i]] != 1)
			{
				flag = 0;
				break;
			}
			i++;
		}
		if (flag && (cost < *result_cost))
		{
			pre_vex = target[0];
			cnt = 0;
			memset(result_path, -1, sizeof(int) * INFI);
			for (i = 0;i < count;i++)
			{
				j = 1;
				while (DPath[pre_vex][path_stack[i]].path[j] != -1)
				{
					//path_start = DPath[pre_vex][path_stack[i]].path[j - 1];
					//path_end = DPath[pre_vex][path_stack[i]].path[j];

					result_path[cnt++] = graph->route[DPath[pre_vex][path_stack[i]].path[j - 1]][DPath[pre_vex][path_stack[i]].path[j]];
					j++;
				}
				
				pre_vex = path_stack[i];
			}
			//result_path[i] = -1;
			*result_cost = cost;
		}

	}
	else if (cost < *result_cost)
		while (ptr->next != NULL)
		{
			ptr = ptr->next;
			if (graph->visited[ptr->point] == 0)
			{
				//begin_t = clock();
				Dij_flag = 1;
				if (DPath[start][ptr->point].flag == 1)
				{
					i = 0;
					Dij_flag = 0;
					while (DPath[start][ptr->point].path[i] != -1)
					{
						if (graph->visited[DPath[start][ptr->point].path[i]] == 1)
						{
							Dij_flag = 1;
							break;
						}
						i++;
					}
					if (!Dij_flag)
					{
						path_stack[count++] = ptr->point;
						cost += DPath[start][ptr->point].weight;
						DFS(head, graph, DPath[start][ptr->point].weight, ptr->point, target, result_path, result_cost);
					}
				}
				if (Dij_flag)
				{
					temp = Dijkstra(graph, target, start, ptr->point, path);
					//finish_t = clock();
					//printf("%f ms used\n", (double)(finish_t - begin_t));
					if (temp < INFI)
					{
						DPath[start][ptr->point].weight = temp;
						DPath[start][ptr->point].flag = 1;
						i = 0;
						while (path[i] != -1)
						{
							DPath[start][ptr->point].path[i] = path[i];
							graph->visited[path[i]] = 1;
							i++;
						}
						DPath[start][ptr->point].path[i] = -1;
						path_stack[count++] = ptr->point;
						cost += temp;
						DFS(head, graph, temp, ptr->point, target, result_path, result_cost);
					}
				}				
			}
		}
	count--;
	cost -= pre_weight;
	graph->visited[start] = 0;
	i = 0;
	if (count == 0)
		while (DPath[target[0]][start].path[i] != -1)
		{
			graph->visited[DPath[target[0]][start].path[i]] = 0;
			i++;
		}
	else if (count > 0)	
		while (DPath[path_stack[count - 1]][start].path[i] != -1)
		{
			graph->visited[DPath[path_stack[count - 1]][start].path[i]] = 0;
			i++;
		}
}

void search_route(char *topo[5000], int edge_num, char *demand)
{
	Node *head[MAX_VEX];
	int target[60], visited[MAX_VEX];
	int path[INFI] = {0};
	int cost = INFI;
	Maps graph;
	int test_weight;
	int test_path[MAX_VEX];

	int i = 0;

	initfunc(head, target, topo, edge_num, &graph);
	int nodes = demand2num(demand, target);
	creat_chain(head, target, nodes);
	//print_chain(head, nodes);
	DFS(head, &graph, 0, target[0], target, path, &cost);
	//test_weight = Dijkstra(graph, visited, 2, 8, test_path);

	if (cost == INFI)
		printf("NA\n");
	else
	{
		printf("cost:%d path:", cost);
		while (path[i] != -1)
		{
			printf("%d, ", path[i]);
			i++;
		}
		printf("\n");
	}


	// unsigned short result[] = {2, 6, 3};//??????????????

	// for (int i = 0; i < 3; i++)
	//     record_result(result[i]);
}

int main()
{
	int edge_num, i;
	char *topo[5000];
	char *demand;
	int demand_num;
		

	begin_t = clock();
	edge_num = read_file(topo, 5000, "topo.csv");
	demand_num = read_file(&demand, 1, "demand.csv");
	//for (i = 0;i < edge_num;i++)
	//	printf("%s", topo[i]);	
	search_route(topo, edge_num, demand);
	finish_t = clock();

	printf("%f ms used", (double)(finish_t - begin_t));
	getchar();

	return 0;
}

