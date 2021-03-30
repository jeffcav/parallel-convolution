#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mpi.h>
#include "include/conv.h"
#include "include/bmp.h"

void propagate_img_dimensions(int *num_rows, int *num_cols) {
	int data[2] = {(*num_rows), (*num_cols)};
	MPI_Bcast(data, 2, MPI_INT, 0, MPI_COMM_WORLD);

	(*num_rows) = data[0];
	(*num_cols) = data[1];
}

unsigned char *propagate_proc_rows(unsigned char *all_rows, int rows_per_proc, int num_cols) {
	// Allocate space for the rows of a process plus 2 rows from adjacent procs
	// TODO calculate extra space according to kernel n: 2*(n/2)
	unsigned char *proc_rows = malloc((rows_per_proc * num_cols) + (2 * num_cols));

	MPI_Scatter(all_rows, rows_per_proc * num_cols,
				MPI_UNSIGNED_CHAR, &proc_rows[num_cols],
				rows_per_proc * num_cols,
				MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	return proc_rows;
}

// TODO Support non-3x3 kernels
void exchange_proc_rows(unsigned char *proc_rows, int rank, int num_procs, int rows_per_proc, int num_cols) {
	// odd procs send, event procs receive
	if (rank & 1) {
		MPI_Send(&proc_rows[num_cols], num_cols,
				MPI_UNSIGNED_CHAR, rank - 1, 0, MPI_COMM_WORLD);

		if (rank < num_procs - 1)
			MPI_Send(&proc_rows[rows_per_proc * num_cols], num_cols,
					MPI_UNSIGNED_CHAR, rank + 1, 0, MPI_COMM_WORLD);
	} else {
		if (rank > 0)
			MPI_Recv(proc_rows, num_cols, MPI_UNSIGNED_CHAR,
					rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		if (rank < num_procs - 1)
			MPI_Recv(&proc_rows[num_cols + (rows_per_proc * num_cols)],
					num_cols, MPI_UNSIGNED_CHAR, rank + 1,
					0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	// even procs send, odd procs receive
	if (rank & 1) {
		if (rank > 0)
			MPI_Recv(proc_rows, num_cols, MPI_UNSIGNED_CHAR,
					rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		if (rank < num_procs - 1)
			MPI_Recv(&proc_rows[num_cols + (rows_per_proc * num_cols)],
					num_cols, MPI_UNSIGNED_CHAR, rank + 1,
					0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	} else {
		MPI_Send(&proc_rows[num_cols], num_cols,
				MPI_UNSIGNED_CHAR, rank - 1, 0, MPI_COMM_WORLD);

		if (rank < num_procs - 1)
			MPI_Send(&proc_rows[rows_per_proc * num_cols], num_cols,
					MPI_UNSIGNED_CHAR, rank + 1, 0, MPI_COMM_WORLD);
	}
}

struct bmp_image *convolution(unsigned char *img_rows, int rank, int world_size, int rows_per_proc, int num_cols) {
	int n = 3;
	struct bmp_image tmp, *out;
	char kernel[9] = {0, 0, 0, 0, 1, 0, 0, 0, 0};

	// Compute convolutions
	tmp.header.width = num_cols;

	// TODO calculate +1 or +2 times n/2
	if (rank == 0 || rank == (world_size - 1))
		tmp.header.height = rows_per_proc + 1;
	else
		tmp.header.height = rows_per_proc + 2;

	tmp.header.planes = 1;
	tmp.header.size = (tmp.header.width * tmp.header.height * tmp.header.planes) + sizeof(struct bmp_header);
	tmp.data = img_rows;

	out = conv_2d(&tmp, kernel, n);
	return out;
}

void process_image(const char *filepath, int rank, int num_procs) {
	struct bmp_image *img, *out;
	unsigned char *all_rows, *proc_rows;
	int rows_per_proc, num_cols;

	if (rank == 0) {
		img = bmp_load(filepath);
		all_rows = img->data;

		// TODO: support num rows not multiple of world_size (num_procs)
		rows_per_proc = img->header.height / num_procs;
		num_cols = img->header.width;
	}

	propagate_img_dimensions(&rows_per_proc, &num_cols);
	printf("%d: rows=%d, cols=%d\n", rank, rows_per_proc, num_cols);
	proc_rows = propagate_proc_rows(all_rows, rows_per_proc, num_cols);
	exchange_proc_rows(proc_rows, rank, num_procs, rows_per_proc, num_cols);
	out = convolution(proc_rows, rank, num_procs, rows_per_proc, num_cols);

	// Aggregate results
	//MPI_Gather()

	if (rank == 0)
		bmp_describe(out);

	free(out);
	free(proc_rows);
	if (rank == 0)
		bmp_destroy(img);
}

int main(int argc, char *argv[]) {
	int world_size, rank;

	if (argc != 2) {
    	fprintf(stderr, "Usage: parallel-mpi image.bmp\n");
    	exit(1);
  	}

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	process_image(argv[1], rank, world_size);

	MPI_Barrier(MPI_COMM_WORLD);
  	MPI_Finalize();
}
