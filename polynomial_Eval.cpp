#include <iostream>
#include <mpi.h>
#include <cmath>
#include <fstream>

using namespace std;

string file_location = "/Users/samin_khan/Documents/College/Fall 2016/CSC770/Project 2/ex/c1_2/c1_2/poly.txt";
/* format: degree, a[0, 1,...,degree+1], x */

void bcast(void * data, int count, MPI_Datatype datatype, int root, MPI_Comm communicator);
void init(int * data);
void init(int &x, int &degree);

int main(int argc, char ** argv)
{
    int pid, nprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    
    int x = 0;
    int * a;
    int DEGREE = 0; //DEGREEEEE
    int nterms = 0;
    
    int temp = 0;
    int i = 1;
    
    int local_sum = 0;
    int rest_terms = 0;
    int nbyp = 0;
    
    if(pid == 0)
    {
        init(x, DEGREE);
        nterms = DEGREE + 1; //a0x^0 is a term.
        
        rest_terms = nterms % nprocs;
        nbyp = (nterms - rest_terms)/nprocs;
        
        bcast(&DEGREE, 1, MPI_INT, 0, MPI_COMM_WORLD);
        bcast(&nterms, 1, MPI_INT, 0, MPI_COMM_WORLD);
        bcast(&nbyp, 1, MPI_INT, 0, MPI_COMM_WORLD);
        bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        printf("|-| P0 broadcasting: degree = %d, x = %d\n", DEGREE, x);
    }
    else
    {
        bcast(&DEGREE, 1, MPI_INT, 0, MPI_COMM_WORLD);
        bcast(&nterms, 1, MPI_INT, 0, MPI_COMM_WORLD);
        bcast(&nbyp, 1, MPI_INT, 0, MPI_COMM_WORLD);
        bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        printf("|+| P%d received degree = %d, x = %d from broadcast\n", pid, DEGREE, x);
    }

    a = new int[nterms];
    
    if(pid == 0)
    {
        init(a);
        
        bcast(a, nterms, MPI_INT, 0, MPI_COMM_WORLD);
        printf("|-| P0 broadcasting a[%d] = |", nterms);
        for(int i = 0; i < nterms; i++)
            cout << a[i] << "|";
        cout << endl;
    }
    else
    {
        bcast(a, nterms, MPI_INT, 0, MPI_COMM_WORLD);
        printf("|+| P%d received a[%d] = |", pid, nterms);
        for(int i = 0; i < nterms; i++)
            cout << a[i] << "|";
        cout << " from broadcast" << endl;
    }
    
    if(nprocs < nterms)
        for(int j = pid; j < nterms; j += nprocs)
            local_sum += a[(pid * nbyp)] * pow(x, (j * nbyp));
    
    if(nprocs >= nterms)
        if(pid < nterms)
            local_sum = a[pid] * pow(x, pid);
    
    printf("\n|*| Partial Sum in P%d is %d\n", pid, local_sum);
    
    while (i <= nprocs)
    {
        if(pid < nprocs - i)
        {
            MPI_Send(&local_sum, 1, MPI_INT, (pid + i), 0, MPI_COMM_WORLD);
            printf("|s| P%d sent %d to P%d\n", pid, local_sum, pid + i);
        }
        
        if(pid >= i)
        {
            MPI_Recv(&temp, 1, MPI_INT, (pid - i), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("|r| P%d received %d from P%d\n", pid, temp, pid - i);
            local_sum += temp;
        }
        i *= 2;
    }
    
    printf("\n***** Prefix Sum in P%d is %d *****\n\n", pid, local_sum);
    
    MPI_Finalize();
    
    if(pid == nprocs - 1)
        printf("Value of the Polynomial: %d.\n\n", local_sum);
    
    return 0;
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

void init(int * data)
{
    int j = 0, degree;
    
    ifstream infile;
    infile.open(file_location);
    
    infile >> degree;
    
    while(j <= degree)
    {
        infile >> data[j];
        j++;
    }
}

void init(int &x, int &degree)
{
    int j = 0;
    int temp = 0;
    
    ifstream infile;
    infile.open(file_location);

    infile >> degree;
    while(j <= degree)
    {
        infile >> temp;
        j++;
    }
    infile >> x;
}
