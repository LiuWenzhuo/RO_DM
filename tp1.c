/*
Maximilien Danisch
September 2017
http://bit.ly/danisch
maximilien.danisch@gmail.com

Info:
Feel free to use these lines as you wish. This program loads a graph in main memory as a list of edges.

To compile:
"gcc edgelist.c -O9 -o edgelist".

To execute:
"./edgelist edgelist.txt".
"edgelist.txt" should contain the graph: one edge on each line (two unsigned long (nodes' ID)) separated by a space.
The prograph loads the graph in main memory and then it terminates.

Note:
If the graph is directed (and weighted) with selfloops and you want to make it undirected unweighted without selfloops, use the following linux command line.
awk '{if ($1<$2) print $1" "$2;else if ($2<$1) print $2" "$1}' net.txt | sort -n -k1,2 -u > net2.txt

Performance:
Up to 500 million edges on my laptop with 8G of RAM:
Takes more or less 1.6G of RAM and 25 seconds (I have an SSD hardrive) for 100M edges.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>//to estimate the runing time


#define NLINKS 100000000 //maximum number of edges for memory allocation, will increase if needed
#define MAXSIZE 1000

typedef struct {
	unsigned long s;
	unsigned long t;
} edge;

//edge list structure:
typedef struct {
	unsigned long n;//number of nodes
	unsigned long e;//number of edges
	edge *edges;//list of edges
} edgelist;

//compute the maximum of three unsigned long
inline unsigned long max3(unsigned long a,unsigned long b,unsigned long c){
	a=(a>b) ? a : b;
	return (a>c) ? a : c;
}

//reading the edgelist from file
edgelist* readedgelist(char* input){
	unsigned long e1=NLINKS;
	FILE *file=fopen(input,"r");

	edgelist *g=malloc(sizeof(edgelist));
	g->n=0;
	g->e=0;
	g->edges=malloc(e1*sizeof(edge));//allocate some RAM to store edges

	while (fscanf(file,"%lu %lu", &(g->edges[g->e].s), &(g->edges[g->e].t))==2) {
		g->n=max3(g->n,g->edges[g->e].s,g->edges[g->e].t);
		if (g->e++==e1) {//increase allocated RAM if needed
			e1+=NLINKS;
			g->edges=realloc(g->edges,e1*sizeof(edge));
		}
	}
	fclose(file);

	g->n++;

	g->edges=realloc(g->edges,g->e*sizeof(edge));

	return g;
}

void free_edgelist(edgelist *g){
	free(g->edges);
	free(g);
}




unsigned * count(char *input) {
	unsigned *c = malloc(2 * sizeof(unsigned));
	FILE *file = fopen(input, "r");
	unsigned n = 0;
	unsigned e = 0;
	unsigned s = 0;
	unsigned t = 0;

	while (fscanf(file, "%lu %lu", &s, &t) == 2) {
		e += 1;
		n = max3(n, s, t);
	}
	c[0] = n + 1;
	c[1] = e;
	return c;
}

unsigned * count_degree(char *input, int directed_graph, int create_file) {
	FILE *file = fopen(input, "r");
	unsigned *c = count(input);
	unsigned *d = malloc(c[0] * sizeof(unsigned));
	unsigned s;
	unsigned t;
	for (int i = 0; i < c[0]; i++) {
		d[i] = 0;
	}
	if (directed_graph == 0) {
		while (fscanf(file, "%lu %lu", &s, &t) == 2) {
			d[s] += 1;
			d[t] += 1;
		}
	}
	else {
		while (fscanf(file, "%lu %lu", &s, &t) == 2) {
			d[s] += 1;
		}
	}
	if (create_file == 1) {
		char * name = malloc( strlen(input) + 8);
		strcpy(name, "degree_");
		strcat(name, input);
		FILE *f = fopen(name, "w");
		if (f == NULL)
		{
			printf("Error opening file!\n");
			exit(1);
		}
		for (int i = 0; i < c[0]; i++)
		{
			fprintf(f, "%lu %lu\n", i, d[i]);
		}
	}
	return d;
}

unsigned Q_G(char* input, int directed_graph) {
	unsigned q = 0;
	unsigned s = 0;
	unsigned t = 0;
	FILE *file = fopen(input, "r");
	unsigned *d = count_degree(input, directed_graph, 0);
	while (fscanf(file, "%lu %lu", &s, &t) == 2) {
		q += d[s] * d[t];
	}
	return q;
}

void degree_distribution(char * input, int directed_graph) {
	unsigned *d = count_degree(input, directed_graph, 0);
	unsigned *c = count(input);
	unsigned *d_n = malloc(c[1] * sizeof(int));
	for (int i = 0; i < c[0]; i++) {
		d_n[i] = 0;
	}
	for (int i = 0; i < c[0]; i++) {
		d_n[d[i]] += 1;
	}

	char *name = malloc(strlen(input) + 14);
	strcpy(name, "distribution_");
	strcat(name, input);
	FILE *f = fopen(name, "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		exit(1);
	}
	for (int i = 0; i < c[0]; i++)
	{
		if (d_n[i] != 0) {
			fprintf(f, "%d %d\n", i, d_n[i]);
		}
	}
}


unsigned **createAdjacencyMatrix(char *input, int directed_graph) {
	unsigned *c = count(input);
	unsigned **m = malloc(c[0] * sizeof(unsigned*));
	unsigned s;
	unsigned t;
	for (int i = 0; i < c[0]; i++) {
		m[i] = malloc(c[0] * sizeof(unsigned));
		for (int j = 0; j < c[0]; j++) {
			m[i][j] = 0;
		}
	}
	FILE *file = fopen(input, "r");
	if (directed_graph) {
		while (fscanf(file, "%lu %lu", &s, &t) == 2) {
			m[s][t] = 1;
		}
	}
	else {
		while (fscanf(file, "%lu %lu", &s, &t) == 2) {
			m[s][t] = 1;
			m[t][s] = 1;
		}
	}
	return m;
}



typedef struct {
	unsigned n;
	unsigned e;
	unsigned *cd;
	unsigned *adj;
}adjarray;

adjarray *createAdjacencyArray(char* input, int directed_graph) {
	adjarray* l = malloc(sizeof(adjarray));
	unsigned *c = count(input);
	unsigned s;
	unsigned t;
	l->n = c[0];
	l->e = c[1];
	l->cd = malloc((c[0] + 1) * sizeof(unsigned));
	unsigned *d = count_degree(input, directed_graph, 0);
	unsigned *index = malloc(c[0] * sizeof(unsigned));
	l->cd[0] = 0;
	for (int i = 1; i <= c[0]; i++) {
		l->cd[i] = l->cd[i-1] + d[i-1];
		index[i - 1] = 0;
	}
	FILE *file = fopen(input, "r");
	if (directed_graph) {
		l->adj = malloc(c[1] * sizeof(unsigned));
		for (int i = 0; i < c[1]; i++){
			l->adj[i] = 0;
		}
		while (fscanf(file, "%lu %lu", &s, &t) == 2) {
			l->adj[l->cd[s]+index[s]] = t;
			index[s] += 1;
		}
	}
	else {
		l->adj = malloc(2*c[1] * sizeof(unsigned));
		for (int i = 0; i < 2*c[1]; i++) {
			l->adj[i] = 0;
		}
		while (fscanf(file, "%lu %lu", &s, &t) == 2) {
			l->adj[l->cd[s] + index[s]] = t;
			l->adj[l->cd[t] + index[t]] = s;
			index[s] += 1;
			index[t] += 1;
		}
	}
	return l;
}

typedef struct {
	unsigned data[MAXSIZE];
	unsigned front;
	unsigned rear;
	unsigned size;
}Queue;

Queue *CreateQueue() {
	Queue *q = malloc(sizeof(Queue));
	q->front = -1;
	q->rear = -1;
	q->size = 0;
	return q;
}

void addQ(Queue* q, unsigned item) {
	if (q->size == MAXSIZE) {
		printf("full queue\n");
	}
	q->rear++;
	q->rear %= MAXSIZE;
	q->size++;
	q->data[q->rear] = item;
}


unsigned deleteQ(Queue *q) {
	if (q->size == 0) {
		printf("empty queue\n");
		return -1;
	}
	q->front++;
	q->front %= MAXSIZE;
	q->size--;
	return q->data[q->front];
}

unsigned BFS(adjarray * l, unsigned s) {
	Queue *f = CreateQueue();
	unsigned *mark = malloc(l->n*sizeof(unsigned));
	unsigned u;
	unsigned n = 0;
	for (int i = 0; i < l->n; i++) {
		mark[i] = 0;
	}
	addQ(f, s);
	mark[s] = 1;
	while (f->size != 0) {
		u = deleteQ(f);
		printf("%lu -> ", u);
		for (int i = l->cd[u]; i < l->cd[u + 1]; i++) {
			unsigned v = l->adj[i];
			if (mark[v] == 0) {
				addQ(f, v);
				mark[v] = 1;
				n = n + 1;
			}
		}
	}
	return n;
}

unsigned count_triangle(edgelist *g, adjarray *l) {
	unsigned n = 0;
	for (int i = 0; i < g->e; i++) {
		unsigned u = g->edges[i].s;
		unsigned v = g->edges[i].t;
			
		if (l->cd[u] < l->cd[u + 1]-1 && l->cd[v] <l->cd[v+1]-1) {
			for (int j = l->cd[u]; j < l->cd[u + 1]; j++)
			{
					for (int k = l->cd[v]; k < l->cd[v + 1]; k++) {
						if (l->adj[j] == l->adj[k]) {
							n++;
						}
					}
			}
		}
				//n++;
			
	}
	return n/3;
}

int main(int argc,char** argv){
	time_t t1,t2;
	t1=time(NULL);
	edgelist* g;
	adjarray* l;
	printf("Counting number of nodes and edges from file %s\n", argv[1]);

	//unsigned *a = count(argv[1]);

	//printf("number of nodes : %lu\n", a[0]);
	//printf("number of edges: %lu\n", a[1]);

	//printf("Reading edgelist from file %s\n",argv[1]);
	//g=readedgelist(argv[1]);
	//printf("number of nodes : %lu\n", g->n);
	//printf("number of edges: %lu\n", g->e);
	l = createAdjacencyArray(argv[1], 0);
	//printf("%lu\n", count_triangle(g, l));
	printf("%lu\n", BFS(l, 5));

	//sort_data(g, "sorted.txt");
	//int * d = count_degree(argv[1], 0, 1);
	
	//int q = Q_G(argv[1], 0);

	//degree_distribution(argv[1], 0);

	//unsigned tri = count_triangle(g, l);

	//printf("Number of triangles: %lu\n", tri);

	//printf("Number of nodes: %lu\n",g->n);
	//printf("Number of edges: %lu\n",g->e);
	//printf("quantity Q: %d\n", q);
	//free_edgelist(g);

	t2=time(NULL);

	printf("- Overall time = %ldh%ldm%lds\n",(t2-t1)/3600,((t2-t1)%3600)/60,((t2-t1)%60));

	return 0;
}


