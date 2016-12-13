#include <iostream>
#include <mpi.h>
#include <fstream>

using namespace std;

void init(int []);
int cmp(const void* ap, const void* bp, int);
int max_index(int* data, int);
int min_index(int* data, int);
void even_odd(int* data, int rank, int size, int);
void print(int* data, int rank, int);
string file_location = 
int min(int, int);
int max(int, int);

int main(int argc, char ** argv)
{
    int data[10000];
    
    int pid, nprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    
    int N = 3500/nprocs; //number of random numbers to be sorted by each proc.
    int stream[N];
    
    init(data);
    
    int begin = pid * N;
    
    for(int i = begin; i < (begin + N); i++)
        stream[i - begin] = data[i];
    
    even_odd(stream, pid, nprocs, N);
    
    cout << "****** ";
    print(stream, pid, N);
    cout << endl;
    
    MPI_Finalize();
    return 0;
    
}

void init(int data[])
{
    int i = 0;
    
    ifstream file("input_OddEven.txt");
    
    while (i < 10000)
    {
        file >> data[i];
        i++;
    }
}

int cmp(const void* ap, const void* bp)
{
    int a = * ((const int*) ap);
    int b = * ((const int*) bp);
    
    if (a < b) {
        return -1;
    } else if (a > b) {
        return 1;
    } else {
        return 0;
    }
}

int max_index(int* data, int N) {
    int i, max = data[0], maxi = 0;
    
    for (i = 1; i < N; i++) {
        if (data[i] > max) {
            max = data[i];
            maxi = i;
        }
    }
    return maxi;
}

/* find the index of the smallest item in an array */
int min_index(int* data, int N)
{
    int i, min = data[0], mini = 0;
    
    for (i = 1; i < N; i++)
    {
        if (data[i] < min)
        {
            min = data[i];
            mini = i;
        }
    }
    return mini;
}

void even_odd(int* data, int rank, int size, int N)
{
    int i;
    
    /* the array we use for reading from partner */
    int other[N];
    
    /* we need to apply P phases where P is the number of processes */
    for (i = 0; i < size; i++)
    {
        /* sort our local array */
        qsort(data, N, sizeof(int), &cmp);
        
        /* find our partner on this phase */
        int partener;
        
        /* if it's an even phase */
        if (i % 2 == 0) {
            /* if we are an even process */
            if (rank % 2 == 0) {
                partener = rank + 1;
            } else {
                partener = rank - 1;
            }
        } else {
            /* it's an odd phase - do the opposite */
            if (rank % 2 == 0) {
                partener = rank - 1;
            } else {
                partener = rank + 1;
            }
        }
        
        /* if the partener is invalid, we should simply move on to the next iteration */
        if (partener < 0 || partener >= size) {
            continue;
        }
        
        /* do the exchange - even processes send first and odd processes receive first
         * this avoids possible deadlock of two processes working together both sending */
        if (rank % 2 == 0) {
            MPI_Send(data, N, MPI_INT, partener, 0, MPI_COMM_WORLD);
            MPI_Recv(other, N, MPI_INT, partener, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            MPI_Recv(other, N, MPI_INT, partener, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(data, N, MPI_INT, partener, 0, MPI_COMM_WORLD);
        }
        
        /* now we need to merge data and other based on if we want smaller or larger ones */
        if (rank < partener) {
            /* keep smaller keys */
            while (1) {
                /* find the smallest one in the other array */
                int mini = min_index(other, N);
                
                /* find the largest one in out array */
                int maxi = max_index(data, N);
                
                /* if the smallest one in the other array is less than the largest in ours, swap them */
                if (other[mini] < data[maxi]) {
                    int temp = other[mini];
                    other[mini] = data[maxi];
                    data[maxi] = temp;
                } else {
                    /* else stop because the smallest are now in data */
                    break;
                }
            }
        } else {
            /* keep larger keys */
            while (1) {
                /* find the largest one in the other array */
                int maxi = max_index(other, N);
                
                /* find the largest one in out array */
                int mini = min_index(data, N);
                
                /* if the largest one in the other array is bigger than the smallest in ours, swap them */
                if (other[maxi] > data[mini]) {
                    int temp = other[maxi];
                    other[maxi] = data[mini];
                    data[mini] = temp;
                } else {
                    /* else stop because the largest are now in data */
                    break;
                }
            }
        }
    }
}

void print(int * data, int rank, int N) {
    int i;
    printf("P%d ******\n", rank);
    for (i = 0; i < N; i++) {
        printf("%d  ", data[i]);
    }
    printf("\n");
}
