#include "mpi.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#define  ARRAYSIZE	100
#define  MASTER		0

using namespace std;

double matrix[ARRAYSIZE * ARRAYSIZE];
double inverse[ARRAYSIZE * ARRAYSIZE];

int main (int argc, char *argv[])
{
	int N, i, j, it, row, column, rowno, recvrow, flag = 0;
	cin >> N;

	// double *matrix = new double[N * N];
	// double *inverse = new double[N * N];

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

	/***** Initializations *****/
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &taskid);

	if(taskid == MASTER)
	{
		row = 0;
		column = 0;
		loops = (N - 1) / (numtasks - 1);
		rem = (N - 1) % (numtasks - 1);
		for(dest = 1; dest < numtasks; dest++)
		{
			MPI_Send(&N, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
			MPI_Send(&numtasks, 1, MPI_INT, dest, tag2, MPI_COMM_WORLD);
			MPI_Send(&matrix[0], N*N, MPI_DOUBLE, dest, tag3, MPI_COMM_WORLD);
			MPI_Send(&inverse[0], N*N, MPI_DOUBLE, dest, tag4, MPI_COMM_WORLD);
		}
	}
	if(taskid > MASTER)
	{
		source = MASTER;
		MPI_Recv(&N, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
		MPI_Recv(&numtasks, 1, MPI_INT, source, tag2, MPI_COMM_WORLD, &status);
		MPI_Recv(&matrix[0], N*N, MPI_DOUBLE, source, tag3, MPI_COMM_WORLD, &status);
		MPI_Recv(&inverse[0], N*N, MPI_DOUBLE, source, tag4, MPI_COMM_WORLD, &status);
		// cout << numtasks << endl;

		row = 0;
		column = 0;
		loops = (N - 1) / (numtasks - 1);
		rem = (N - 1) % (numtasks - 1);
		// cout << loops << endl;

		// for(i = 0; i < N; i++)
		// {
		// 	for(j = 0; j < N; j++)
		// 		cout<<inverse[i * N + j] << " ";
		// 	cout<<endl;
		// }
	}

	for(i = 0; i < N; i++)
	{
		// printf ("MPI task %d has started...\n", taskid);
		if(taskid == MASTER)
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
			else if(matrix[row * N + column] != 1)
			{
				double divisor = matrix[row * N + column];
				for(it = 0; it < N; it++)
					matrix[row * N + it] /= divisor;

				for(it = 0; it < N; it++)
					inverse[row * N + it] /= divisor;
			}

			recvrow = rowno = -1;

			cout << "[" << taskid << "] " << "Sending, row - " << i << endl;

			for(j = 0; j < loops; j++)
			{
				for(dest = 1; dest < numtasks; dest++)
				{
					rowno++;
					if(rowno == row)
						rowno++;

					cout << "[" << taskid << "] " << "Sendprocess - " << dest << endl;

					MPI_Send(&row, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
					MPI_Send(&rowno, 1, MPI_INT, dest, tag2, MPI_COMM_WORLD);
				}

				for(dest = 1; dest < numtasks; dest++)
				{
					recvrow++;
					if(recvrow == row)
						recvrow++;

					cout << "[" << taskid << "] " << "Receiving, process - " << dest << endl;

					MPI_Recv(&matrix[recvrow * N], N, MPI_DOUBLE, dest, dest, MPI_COMM_WORLD, &status);
					MPI_Recv(&inverse[recvrow * N], N, MPI_DOUBLE, dest, dest, MPI_COMM_WORLD, &status);
				}
			}

			cout << "[" << taskid << "] " << "We are out with rem - " << rem << endl;

			// recvrow = row;
			for(dest = 1; dest <= rem; dest++)
			{
				rowno++;
				if(rowno == row)
					rowno++;

				cout << "[" << taskid << "] " << "Sendprocess - " << dest << endl;

				MPI_Send(&row, 1, MPI_INT, dest, tag1, MPI_COMM_WORLD);
				MPI_Send(&rowno, 1, MPI_INT, dest, tag2, MPI_COMM_WORLD);
			}

			for(dest = 1; dest <= rem; dest++)
			{
				recvrow++;
				if(recvrow == row)
					recvrow++;

				cout << "[" << taskid << "] " << "Receiving, process - " << dest << endl;
				MPI_Recv(&matrix[recvrow * N], N, MPI_DOUBLE, dest, dest, MPI_COMM_WORLD, &status);
				MPI_Recv(&inverse[recvrow * N], N, MPI_DOUBLE, dest, dest, MPI_COMM_WORLD, &status);
			}

			if(flag == 1)
			{
				MPI_Finalize();
				return 0;
			}

			for(i = 0; i < N; i++)
			{
				for(j = 0; j < N; j++)
					cout<<inverse[i * N + j] << " ";
				cout<<endl;
			}

			row++;
			column++;

			// MPI_Barrier(MPI_COMM_WORLD);
		}
	}

	for(int k = 0; k < N; k++)
	{
		/***** Non-master tasks only *****/
		if (taskid > MASTER)
		{
			cout << "[" << taskid << "] " << "Loops: " << loops << " Rem: "<< rem << endl;
			for(j = 0; j < loops; j++)
			{
				/* Receive my portion of array from the master task */
				source = MASTER;

				cout << "[" << taskid << "] " << "ID - " << taskid << endl;

				MPI_Recv(&row, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
				MPI_Recv(&rowno, 1, MPI_INT, source, tag2, MPI_COMM_WORLD, &status);

				double divisor = matrix[rowno * N + row] / matrix[row * N + row];

				cout << "[" << taskid << "] " << "Process " << taskid << " got row - " << rowno << endl;

				for(i = 0; i < N; i++)
				{
					matrix[rowno * N + i] = matrix[rowno * N + i] - (matrix[row * N + i] * divisor);
					inverse[rowno * N + i] = inverse[rowno * N + i] - (inverse[row * N + i] * divisor);
				}

				MPI_Send(&matrix[rowno * N], N, MPI_DOUBLE, source, taskid, MPI_COMM_WORLD);
				MPI_Send(&inverse[rowno * N], N, MPI_DOUBLE, source, taskid, MPI_COMM_WORLD);

				cout << "[" << taskid << "] " << "Sent from " << taskid ;
			}

			if( taskid <= rem )
			{
				/* Receive my portion of array from the master task */
				source = MASTER;

				cout << "[" << taskid << "] " << "Process ID - " << taskid << endl;

				MPI_Recv(&row, 1, MPI_INT, source, tag1, MPI_COMM_WORLD, &status);
				MPI_Recv(&rowno, 1, MPI_INT, source, tag2, MPI_COMM_WORLD, &status);

				double divisor = matrix[rowno * N + row] / matrix[row * N + row];

				cout << "[" << taskid << "] " << "MPI process " << taskid << " got row - " << rowno << endl;

				for(i = 0; i < N; i++)
				{
					matrix[rowno * N + i] = matrix[rowno * N + i] - (matrix[row * N + i] * divisor);
					// inverse[rowno * N + i] = inverse[rowno * N + i] - (inverse[row * N + i] * divisor);
				}

				MPI_Send(&matrix[rowno * N], N, MPI_DOUBLE, source, taskid, MPI_COMM_WORLD);
				MPI_Send(&inverse[rowno * N], N, MPI_DOUBLE, source, taskid, MPI_COMM_WORLD);

				cout << "[" << taskid << "] " << "Sent from " << taskid << endl;
			}

			// MPI_Barrier(MPI_COMM_WORLD);
		} /* end of non-master */
	}

	if( taskid == MASTER )
	{
		// cout<< "Hello" << endl;
		// for(i = 0; i < N; i++)
		// {
		// 	for(j = 0; j < N; j++)
		// 		cout<<inverse[i * N + j] << " ";
		// 	cout<<endl;
		// }
	}

	MPI_Finalize();
	return 0;

}/* end of main */
