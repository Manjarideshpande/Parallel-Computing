#include <iostream>
#include <mpi.h>
#include <cmath>

using namespace std;

#define number 1001

int main(int argc, char ** argv)
{
    int pid, nprocs;
    int partial_sum = 0;
    int p = 1;
    int temp = 0;
    
    int chunk_size;
    int lowerlimit;
    int upperlimit;
    int rem;
    int rem_sum = 0;
    int first_n_numbers;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    
    rem = number % nprocs;
    first_n_numbers = (number - rem);
    
    chunk_size = first_n_numbers/nprocs;
    lowerlimit = (pid * chunk_size) + 1;
    upperlimit = (pid * chunk_size) + chunk_size;
    
    for(int i = lowerlimit; i <= upperlimit; i++)
        partial_sum += i;
    
    if(pid < nprocs - 1)
        cout << "|*| Partial Sum S" << pid << " is " << partial_sum << endl;
    
    //calculating sum of remaining numbers
    if(pid == nprocs - 1)
    {
        if(rem > 0)
        {
            for(int i = first_n_numbers + 1; i <= number; i++)
                rem_sum += i;
            printf("|*| Partial Sum S%d* of Remaining Numbers is %d\n", pid, rem_sum);
            printf("|*| Partial Sum S%d is %d + %d = %d\n", pid, partial_sum, rem_sum, partial_sum+rem_sum);
        }
        else
            cout << "|*| Partial Sum S" << pid << " is " << partial_sum << endl;
        partial_sum += rem_sum;
    }
    
    while(p <= log2(nprocs))
    {
        if(pid % (int) pow(2, p-1) == 0)
        {
            if(pid % (int) pow(2, p) != 0)
            {
                MPI_Send(&partial_sum, 1, MPI_INT, (pid - (pow(2, p-1))), 0, MPI_COMM_WORLD);
                cout << "|s| P" << pid << " sent " << partial_sum << " to P" << (pid - (pow(2, p-1))) << endl;
            }
            else
            {
                MPI_Recv(&temp, 1, MPI_INT, (pid + (pow(2, p-1))), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                cout << "|r| P" << pid << " received " << temp << " from P" << (pid + (pow(2, p-1))) << endl;
                partial_sum = temp + partial_sum;
                printf("\n***** Parallel Sum in P%d = %d *****\n\n", pid, partial_sum);
            }
        }
        p++;
    }
    
    MPI_Finalize();
    
    if(pid == 0)
        printf("Total Sum: %d.\n\n", partial_sum);
}
