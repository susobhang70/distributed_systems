#include "mpi.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#define  ARRAYSIZE	100
#define  MASTER		0

#define COMM MPI_COMM_WORLD
#define NO_STATUS MPI_STATUS_IGNORE
#define STOP 42

using namespace std;

int graph[ARRAYSIZE * ARRAYSIZE];

void printmatrix(int *A, int n)
{
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < n; j++)
			printf("%d ", A[i * n + j]);
		printf("\n");
	}
	cout<<endl;
}

int main (int argc, char *argv[])
{
	int N, M, i, j, a, b, it, rowno, recvrow, flag = 0;
	cin >> N >> M;

	int tag1, tag2, tag3, tag4, tag5, tag6;
	int numtasks, taskid, dest, source, loops, rem; 

	tag6 = 1;
	tag5 = 2;
	tag4 = 3;
	tag3 = 4;
	tag2 = 5;
	tag1 = 6;
	
	MPI_Status status;
	
	for(i = 0; i < N; i++)
	{
		for(j = 0; j < N; j++)
		{
			graph[i * N + j] = 0;
		}
	}

	for(i = 0; i < M; i++)
	{
		cin >> a >> b;
		graph[a * N + b] = 1;
		graph[b * N + a] = 1;
	}

	/***** Initializations *****/
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

	if(taskid == MASTER)
	{
		for(dest = 1; dest < numtasks; dest++)
		{
			MPI_Send(&N, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);

			if( dest < N )
			{
				MPI_Send(&graph[dest * N], N, MPI_INT, dest, tag3, MPI_COMM_WORLD);
			}
		}

		if (numtasks < N)
		{
			cout << "Restart with atleast the same number of processes as there are nodes in the graph\n";
			MPI_Finalize();
			exit(0);
		}

		// now do master's work

		int colors[ARRAYSIZE];
		colors[taskid] = taskid;

		for(i = taskid + 1; i < N; i++)
		{
			if(graph[taskid * N + i] == 1)
			{
				MPI_Ssend(&colors[taskid], 1, MPI_INT, i, tag4, MPI_COMM_WORLD);
			}
		}

		int max = -1;
		for(i = taskid + 1; i < N; i++)
		{
			// int a;
			MPI_Recv(&colors[i], 1, MPI_INT, i, tag4, MPI_COMM_WORLD, &status);
			if(colors[i] > max)
				max = colors[i];
		}

		cout << max + 1 << endl;
		for(i = 0; i < N; i++)
			cout << colors[i] << endl;

	}

	if(taskid > MASTER)
	{
		source = MASTER;
		MPI_Recv(&N, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
		if(taskid < N)
		{
			MPI_Recv(&graph[taskid * N], N, MPI_INT, source, tag3, MPI_COMM_WORLD, &status);
		}

		if(numtasks < N || taskid >= N)
			exit(0);

		int colors[ARRAYSIZE];
		int k = 0;
		for(i = 0; i < N; i++)
			colors[i] = -1;

		for(i = 0; i < taskid; i++)
			if(graph[taskid * N + i] == 1 && colors[i] == -1)
				MPI_Recv(&colors[i], 1, MPI_INT, i, tag4, MPI_COMM_WORLD, &status);

		// chooses lowest color available
		int chosen = -1;
		for(int it1 = 0; it1 < N; it1++)
		{
			int it2;	
			for(it2 = 0; it2 < N; it2++)
				if(graph[taskid *N + it2] == 1 && colors[it2] == it1)
					break;
			if(it2 == N)
			{
				chosen = it1;
				break;
			}
		}

		// create this buffer, as there is no guarantee of MPI_Send's blocking.
		// Ensure no blocks and safe buffers
		int p[1000];
		for(int it1 = 0; it1 < N; it1++)
			p[it1] = chosen;

		for(i = taskid + 1; i < N; i++)
		{
			if(graph[taskid * N + i] == 1)
			{
				// Safe buffers, can move on
				MPI_Ssend(&p[i], 1, MPI_INT, i, tag4, MPI_COMM_WORLD);
			}
		}

		// send final allocation to master
		MPI_Ssend(&chosen, 1, MPI_INT, MASTER, tag4, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;

}/* end of main */
