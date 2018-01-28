#include "mpi.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
// #define  ARRAYSIZE	16
#define  MASTER		0

using namespace std;

int main (int argc, char *argv[])
{
	int N, i, j;
	cin >> N;

	double *matrix = new double[N * N];
	double *inverse = new double[N * N];

	int numtasks, taskid, rc, dest, offset, i, j, tag1, tag2, source, chunksize; 
	// int mysum, sum;
	
	// int update(int myoffset, int chunk, int myid);
	
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

	// for(i = 0; i < N; i++)
	// {
	// 	for(j = 0; j < N; j++)
	// 	{
	// 		cout << inverse[i * N + j] << " ";
	// 	}
	// 	cout << endl;
	// }

	/***** Initializations *****/
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
	printf ("MPI task %d has started...\n", taskid);

	if(taskid == MASTER)
	{
		int loops = (numtasks - 1) / (N - 1);
		int rem = (numtasks - 1) % (N - 1);

		int row = 0, column = 0;

		for(i = 0; i < N; i++, column++)
		{
			// checks if the head of the current row is zero
			if(matrix[row * N + column] == 0)
			{
				int check = row + 1;
				while(check < N)
				{
					if(matrix[check * N + column] == 0)
						check++;
				}

				// if it reaches the end, that means the matrix is not invertible
				if(check == N)
				{
					cout << "Matrix not invertible" << endl;
					flag = 1;
					break;
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

			for(j = 0; j < loops; j++)
			{

			}
		}
	}




	// int numtasks, taskid, rc, dest, offset, i, j, tag1, tag2, source, chunksize; 
	// int mysum, sum;
	
	// int update(int myoffset, int chunk, int myid);
	
	// MPI_Status status;

	// /***** Initializations *****/
	// MPI_Init(&argc, &argv);
	// MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	// MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
	// printf ("MPI task %d has started...\n", taskid);
	
	// chunksize = (ARRAYSIZE / numtasks);
	// tag2 = 1;
	// tag1 = 2;

	// /***** Master task only ******/
	// if (taskid == MASTER)
	// {
	//     /* Initialize the array */
	//     sum = 0;
	//     for(i = 0; i < ARRAYSIZE; i++)
	//     {
	//         data[i] =  i;
	//         sum = sum + data[i];
	//     }

	//     cout << "Initialized Sum " << sum << endl;

	//     /* Send each task its portion of the array - master keeps 1st part */
	//     offset = chunksize;
	//     for (dest = 1; dest < numtasks; dest++) 
	//     {
	//         MPI_Send(&offset, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
	//         MPI_Send(&data[offset], chunksize, MPI_INT, dest, tag2, MPI_COMM_WORLD);
	//         printf("Sent %d elements to task %d offset= %d\n",chunksize,dest,offset);
	//         offset = offset + chunksize;
	//     }

	//     /* Master does its part of the work */
	//     offset = 0;
	//     mysum = update(offset, chunksize, taskid);


	//     /* Get Final Sum*/  
	//     sum = 0;
	//     MPI_Reduce(&mysum, &sum, 1, MPI_FLOAT, MPI_SUM, MASTER, MPI_COMM_WORLD);        
	//     cout << "Final Recieved Sum: " << sum << endl;

	// }  /* end of master section */



	// /***** Non-master tasks only *****/
	// if (taskid > MASTER)
	// {
	//     /* Receive my portion of array from the master task */
	//     source = MASTER;
	//     MPI_Recv(&offset, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
	//     MPI_Recv(&data[offset], chunksize, MPI_FLOAT, source, tag2, MPI_COMM_WORLD, &status);

	//     mysum = update(offset, chunksize, taskid);

	//     MPI_Reduce(&mysum, &sum, 1, MPI_FLOAT, MPI_SUM, MASTER, MPI_COMM_WORLD);
	// } /* end of non-master */


	// MPI_Finalize();

}/* end of main */
