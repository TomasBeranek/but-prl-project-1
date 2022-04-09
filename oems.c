#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

void MPI_Isend_safe(  const void *buf,
                      int count,
                      MPI_Datatype datatype,
                      int dest,
                      int tag,
                      MPI_Comm comm) {
  MPI_Request request;
  int ierr = MPI_Isend(buf, count, datatype, dest, tag, comm, &request);

  if (ierr != MPI_SUCCESS){
    fprintf(stderr, "ERROR: MPI_Isend failed with error %d!\n", ierr);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
}

void MPI_Irecv_safe(  void *buf,
                      int count,
                      MPI_Datatype datatype,
                      int source,
                      int tag,
                      MPI_Comm comm,
                      MPI_Request *status) {
  int ierr = MPI_Irecv(buf, count, datatype, source, tag, comm, status);

  if (ierr != MPI_SUCCESS){
    fprintf(stderr, "ERROR: MPI_IRecv failed with error %d!\n", ierr);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
}

void MPI_Waitall_safe(  int count,
                        MPI_Request array_of_requests[],
                        MPI_Status array_of_statuses[]){
  int ierr = MPI_Waitall(count, array_of_requests, array_of_statuses);

  if (ierr != MPI_SUCCESS){
    fprintf(stderr, "ERROR: MPI_Waitall failed with error %d!\n", ierr);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
}

void swap(int* a, int* b){
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

int main(int argc,char *argv[]){
  int rank, size;
  int min_out[20] = {-1, 5, 5, 7, 7, 11, 9, 11, 10, 13, 13, 0, 15, 17, 16, 17, 18, 0, 0, 0};
  int max_out[20] = {-1, 6, 6, 8, 8, 9, 14, 10, 14, 12, 12, 15, 19, 16, 0, 18, 19, 0, 0, 0};

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // masters code
  if (rank == 0){
    // load 8 numbers
    FILE *f = fopen("numbers", "rb");
    int numbers[8];

    for (int i = 0; i < 8; i++){
      int c = getc(f);

      if (c == EOF){
        fprintf(stderr, "ERROR: Program needs 8 numbers to run, but file 'numbers' contain %d!\n", i);
        MPI_Abort(MPI_COMM_WORLD, 1);
      }

      numbers[i] = c;
    }

    // prints the numbers
    for (int i = 0; i < 8; i++){
      if (i == 7)
        printf("%d\n", numbers[i]);
      else
        printf("%d ", numbers[i]);
    }

    // send numbers to the first layer of processes
    for (int proc_rank = 1; proc_rank <= 4; proc_rank++){
        MPI_Isend_safe(numbers + proc_rank*2 - 2, 1, MPI_INT, proc_rank, 0, MPI_COMM_WORLD);
        MPI_Isend_safe(numbers + proc_rank*2 - 1, 1, MPI_INT, proc_rank, 0, MPI_COMM_WORLD);
    }

    // load the result
    int res_rank[8] = {11, 17, 17, 18, 18, 19, 19, 14};
    int res_tag[8]  = {1, 1, 2, 1, 2, 1, 2, 2};
    MPI_Request* array_of_requests = (MPI_Request*)malloc(sizeof(MPI_Request)*8);
    MPI_Status* array_of_statuses = (MPI_Status*)malloc(sizeof(MPI_Status)*8);

    for (int i = 0; i < 8; i++){
      MPI_Irecv_safe(numbers + i, 1, MPI_INT, res_rank[i], res_tag[i], MPI_COMM_WORLD, array_of_requests + i);
    }

    MPI_Waitall_safe(8, array_of_requests, array_of_statuses);

    // print the result
    for (int i = 0; i < 8; i++){
      printf("%d\n", numbers[i]);
    }

  } else { // workers code
    MPI_Request* array_of_requests = (MPI_Request*)malloc(sizeof(MPI_Request)*2);
    MPI_Status* array_of_statuses = (MPI_Status*)malloc(sizeof(MPI_Status)*2);
    int min, max;

    MPI_Irecv_safe(&min, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, array_of_requests);
    MPI_Irecv_safe(&max, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, array_of_requests + 1);

    MPI_Waitall_safe(2, array_of_requests, array_of_statuses);

    // switch min a max if needed
    if (min > max)
      swap(&min, &max);

    MPI_Isend_safe(&min, 1, MPI_INT, min_out[rank], 1, MPI_COMM_WORLD);
    MPI_Isend_safe(&max, 1, MPI_INT, max_out[rank], 2, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
