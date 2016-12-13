#include <iostream>
#include <mpi.h>
#include <fstream>

using namespace std;

string file_location = "/Users/samin_khan/Documents/College/Fall 2016/CSC770/Project 2/ex/Stock_Market/Stock_Market/stock.txt";

int findminmax(int [], int, int);
void init(int * data);
void init(int &x);
void bcast(void * data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator);

int main(int argc, char ** argv)
{
    int pid, nprocs;
    int * price;
    int n = 0;
    int i = 1;
    int temp = 0;
    int temp1 = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    
    if(pid == 0)
    {
        init(n);
        
        bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        printf("|-| P0 broadcasting num = %d\n", n);
    }
    else
    {
        bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
        printf("|+| P%d received num = %d from broadcast.\n", pid, n);
    }
    
    price = new int[n];
    
    if(pid == 0)
    {
        init(price);
        
        bcast(price, n, MPI_INT, 0, MPI_COMM_WORLD);
        printf("\n|-| P0 broadcasting price[%d] = |", n);
        for(int k = 0; k < n; k++)
            cout << price[k] << "|";
        cout << endl;
    }
    else
    {
        bcast(price, n, MPI_INT, 0, MPI_COMM_WORLD);
        printf("|+| P%d received price[%d] = |", pid, n);
        for(int k = 0; k < n; k++)
            cout << price[k] << "|";
        cout << " from broadcast." << endl;
    }
    
    int buy;
    int sell;
    int * stream =  new int[n/nprocs];
    
    for(int i = pid * n/nprocs; i < (pid * n/nprocs + n/nprocs); i++)
        stream[i - pid * n/nprocs] = price[i];
    
    buy = findminmax(stream, n/nprocs, 0);
    sell = findminmax(stream, n/nprocs, 1);
    
    while (i <= nprocs)
    {
        if(pid < nprocs - i)
        {
            MPI_Send(&buy, 1, MPI_INT, (pid + i), 0, MPI_COMM_WORLD);
            //printf("|s| P%d sent %d to P%d\n", pid, buy, pid + i);
            MPI_Send(&sell, 1, MPI_INT, (pid + i), 1, MPI_COMM_WORLD);
            //printf("|s| P%d sent %d to P%d\n", pid, sell, pid + i);

        }
        if(pid >= i)
        {
            MPI_Recv(&temp, 1, MPI_INT, (pid - i), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //printf("|r| P%d received %d from P%d\n", pid, temp, pid - i);
            MPI_Recv(&temp1, 1, MPI_INT, (pid - i), 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //printf("|r| P%d received %d from P%d\n", pid, temp1, pid - i);
    
            if(pid > i)
                if(temp1 > sell)
                    sell = temp1;
            if(buy > temp)
                buy = temp;
        }
        i *= 2;
    }
    
    printf("\n|*| P%d: buy = %d, sell = %d, profit = %d\n", pid, buy, sell, sell-buy);
    
    MPI_Finalize();
    if(pid == nprocs-1)
        printf("\nBuy at %d, Sell at %d, Profit: %d.\n\n", buy, sell, sell - buy);
}

int findminmax(int array[], int n, int t)
{
    int temp = 0;
    if(t == 0)
    {
        temp = 1000;
        for(int i = 0; i < n; i++)
            if(array[i] < temp)
                temp = array[i];
    }
    if(t == 1)
    {
        temp = 0;
        for(int i = 0;i < n;i++)
            if(array[i] > temp)
                temp = array[i];
    }
    return temp;
}

void init(int * data)
{
    int j = 0;
    int x;
    ifstream infile;
    infile.open(file_location);
    
    infile >> x;
    while(j < x)
    {
        infile >> data[j];
        j++;
    }
}

void init(int &x)
{
    ifstream infile;
    infile.open(file_location);
    
    infile >> x;
}

void bcast(void * data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator)
{
    int world_rank;
    MPI_Comm_rank(communicator, &world_rank);
    int world_size;
    MPI_Comm_size(communicator, &world_size);
    
    if (world_rank == root)
    {
        for (int i = 0; i < world_size; i++)
            if (i != world_rank)
                MPI_Send(data, count, datatype, i, 0, communicator);
    }
    else
        MPI_Recv(data, count, datatype, root, 0, communicator, MPI_STATUS_IGNORE);
}
