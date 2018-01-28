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

double matrix[ARRAYSIZE * ARRAYSIZE];
double inverse[ARRAYSIZE * ARRAYSIZE];

void printmatrix(double *A, int n)
{
	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < n; j++)
			printf("%.2f ", A[i * n + j]);
		printf("\n");
	}
	cout<<endl;
}

int main (int argc, char *argv[])
{
	int N, i, j, it, rowno, recvrow, flag = 0;
	cin >> N;

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
		for(j = 0; j < N; j++)
			cin >> matrix[i * N + j];

	for(i = 0; i < N; i++)
	{
		for(j = 0; j < N; j++)
		{
			if(i == j)
				inverse[i * N + j] = 1;
			else
				inverse[i * N + j] = 0;
		}
	}

	if(N == 1)
	{
		cout << "Matrix is already it's inverse" << endl;
		return 0;
	}

	int row, column;

	/***** Initializations *****/
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);


	if(taskid == MASTER)
	{
		for(dest = 1; dest < numtasks; dest++)
		{
			MPI_Send(&N, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
			MPI_Send(&numtasks, 1, MPI_INT, dest, tag2, MPI_COMM_WORLD);
		}
	}
	if(taskid > MASTER)
	{
		source = MASTER;
		MPI_Recv(&N, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
		MPI_Recv(&numtasks, 1, MPI_INT, source, tag2, MPI_COMM_WORLD, &status);
	}

	if(taskid == MASTER)
	{
		for(i = 0; i < N; i++)
		{
			row = i;
			column = i;

			// checks if the head of the current row is zero
			if(matrix[row * N + column] == 0)
			{
				int check = row + 1;
				while(check < N)
				{
					if(matrix[check * N + column] == 0)
						check++;
					else
						break;
				}

				// if it reaches the end, that means the matrix is not invertible
				if(check == N)
				{
					cout << "Matrix not invertible" << endl;
					flag = 1;
					MPI_Abort(MPI_COMM_WORLD, 0);
				}
				// otherwise, add the two rows, and make the head 1
				else
				{
					double divisor = matrix[check * N + column];

					for(it = 0; it < N; it++)
						matrix[row * N + it] = (matrix[row * N + it] + matrix[check * N + it])/divisor;

					for(it = 0; it < N; it++)
						inverse[row * N + it] = (inverse[row * N + it] + inverse[check * N + it])/divisor;
				}
			}
			else if(matrix[row * N + column] != 1)
			{
				double divisor = matrix[row * N + column];
				for(it = 0; it < N; it++)
					matrix[row * N + it] /= divisor;

				for(it = 0; it < N; it++)
					inverse[row * N + it] /= divisor;
			}

			rowno = -1;
			int temp = row;

			for(dest = 1; dest < N; dest++)
			{
				rowno++;
				if(rowno == temp)
					rowno++;

				int to = ((dest - 1) % (numtasks - 1)) + 1;

				MPI_Send(&rowno, 1, MPI_INT, to, tag1, MPI_COMM_WORLD);
				MPI_Send(&inverse[rowno * N], N, MPI_DOUBLE, to, tag2, MPI_COMM_WORLD);
				MPI_Send(&matrix[rowno * N], N, MPI_DOUBLE, to, tag3, MPI_COMM_WORLD);

				MPI_Send(&temp, 1, MPI_INT, to, tag4, MPI_COMM_WORLD);
				MPI_Send(&inverse[temp * N], N, MPI_DOUBLE, to, tag5, MPI_COMM_WORLD);
				MPI_Send(&matrix[temp * N], N, MPI_DOUBLE, to, tag6, MPI_COMM_WORLD);

				MPI_Recv(&inverse[rowno * N], N, MPI_DOUBLE, to, tag3, MPI_COMM_WORLD, &status);
				MPI_Recv(&matrix[rowno * N], N, MPI_DOUBLE, to, tag4, MPI_COMM_WORLD, &status);

			}
		}

		// int dummy = 1;
		// for(int k = 1; k < numtasks; k++)
		// 	MPI_Send(&dummy, 1, MPI_INT, k, STOP, COMM);
	}

	if(taskid > MASTER)
	{
		while(true)
		{
			// terminate all processes which are not really needed
			if(taskid >= N)
				break;

			// cout << "[" << taskid << "]" << "Hello" << endl;
			int idx, pivot, current;

			MPI_Recv(&idx, 1, MPI_INT, MASTER, tag1, MPI_COMM_WORLD, &status);
			MPI_Recv(&inverse[idx * N], N, MPI_DOUBLE, MASTER, tag2, MPI_COMM_WORLD, &status);
			MPI_Recv(&matrix[idx * N], N, MPI_DOUBLE, MASTER, tag3, MPI_COMM_WORLD, &status);

			current = idx;
			// cout << "C: " << current << endl;

			MPI_Recv(&idx, 1, MPI_INT, MASTER, tag4, MPI_COMM_WORLD, &status);
			MPI_Recv(&inverse[idx * N], N, MPI_DOUBLE, MASTER, tag5, MPI_COMM_WORLD, &status);
			MPI_Recv(&matrix[idx * N], N, MPI_DOUBLE, MASTER, tag6, MPI_COMM_WORLD, &status);

			pivot = idx;

			double multiplier = matrix[current * N + pivot] / matrix[pivot * N + pivot];
			for(int it = 0; it < N; it++)
			{
				matrix[current * N + it] -= (matrix[pivot * N + it] * multiplier);
				inverse[current * N + it] -= (inverse[pivot * N + it] * multiplier);
			}

			MPI_Send(&inverse[current * N], N, MPI_DOUBLE, MASTER, tag3, MPI_COMM_WORLD);
			MPI_Send(&matrix[current * N], N, MPI_DOUBLE, MASTER, tag4, MPI_COMM_WORLD);

			// int dummy;
			// if(MPI_Iprobe(MPI_ANY_SOURCE, STOP, COMM, &flag, &status)) {
			// 	MPI_Recv(&dummy, 1, MPI_INT, MPI_ANY_SOURCE, STOP, COMM, NO_STATUS);
			// 	break;
			// }

			// last computation, so terminate
			if(pivot == N - 1 && current > N - numtasks - 1)
				break;
		}
	}

	if(taskid == MASTER)
	{
		printmatrix(inverse, N);
	}

	MPI_Finalize();
	return 0;

}/* end of main */
