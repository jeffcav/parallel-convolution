#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mpi.h>
#include "include/conv.h"
#include "include/image.h"

void propagate_img_dimensions(int *num_rows, int *num_cols) {
	int data[2] = {(*num_rows), (*num_cols)};
	MPI_Bcast(data, 2, MPI_INT, 0, MPI_COMM_WORLD);

	(*num_rows) = data[0];
	(*num_cols) = data[1];
}

unsigned char *propagate_proc_rows(unsigned char *all_rows, int rows_per_proc, int num_cols, int kernel_dim) {
	int num_rows;
	unsigned char *proc_rows;

	num_rows = rows_per_proc + (2 * (kernel_dim/2)); // space for adjacent rows
	proc_rows = malloc(num_rows * num_cols);

	MPI_Scatter(all_rows, rows_per_proc * num_cols,
				MPI_UNSIGNED_CHAR, &proc_rows[kernel_dim/2],
				rows_per_proc * num_cols,
				MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	return proc_rows;
}

// odd procs send, event procs receive
void exchange_proc_rows_step1(unsigned char *proc_rows, int rank, int num_procs, int rows_per_proc, int num_cols) {
	int offset;

	if (rank & 1) {
		// send upwards
		if (rank > 0)
			MPI_Send(&proc_rows[num_cols], num_cols, MPI_UNSIGNED_CHAR, rank - 1, 0, MPI_COMM_WORLD);

		// send downards
		if (rank < num_procs - 1) {
			offset = rows_per_proc * num_cols;
			MPI_Send(&proc_rows[offset], num_cols, MPI_UNSIGNED_CHAR, rank + 1, 0, MPI_COMM_WORLD);
		}
	} else {
		// receive from downards
		if (rank < num_procs - 1) {
			offset = num_cols + (rows_per_proc * num_cols);
			MPI_Recv(&proc_rows[offset], num_cols, MPI_UNSIGNED_CHAR, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		// receive from upwards
		if (rank > 0)
			MPI_Recv(proc_rows, num_cols, MPI_UNSIGNED_CHAR, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
}

void exchange_proc_rows_step2(unsigned char *proc_rows, int rank, int num_procs, int rows_per_proc, int num_cols) {
	int offset;

	if (rank & 1) {
		// receive from downards
		if (rank < num_procs - 1) {
			offset = num_cols + (rows_per_proc * num_cols);
			MPI_Recv(&proc_rows[offset], num_cols, MPI_UNSIGNED_CHAR, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		// receive from upwards
		if (rank > 0)
			MPI_Recv(proc_rows, num_cols, MPI_UNSIGNED_CHAR, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	} else {
		// send upwards
		if (rank > 0)
			MPI_Send(&proc_rows[num_cols], num_cols, MPI_UNSIGNED_CHAR, rank - 1, 0, MPI_COMM_WORLD);

		// send downards
		if (rank < num_procs - 1) {
			offset = rows_per_proc * num_cols;
			MPI_Send(&proc_rows[offset], num_cols, MPI_UNSIGNED_CHAR, rank + 1, 0, MPI_COMM_WORLD);
		}
	}
}

// TODO Support non-3x3 kernels
void exchange_proc_rows(unsigned char *proc_rows, int rank, int num_procs, int rows_per_proc, int num_cols) {
	exchange_proc_rows_step1(proc_rows, rank, num_procs, rows_per_proc, num_cols);
	exchange_proc_rows_step2(proc_rows, rank, num_procs, rows_per_proc, num_cols);

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

struct bmp_image *aggregate_results(struct bmp_image *out, int rank,
									int num_procs, int rows_per_proc,
									int num_cols, int kernel_dim) {
	unsigned char *result;
	struct bmp_image *output;
	int final_size, send_size;
	int rows, cols;
	int tot_rows, tot_cols;

	if (rank > 0 && rank < num_procs - 1) {
		rows = rows_per_proc;
		cols = num_cols - (2 * (kernel_dim/2));
	} else {
		rows = rows_per_proc - (kernel_dim/2);
		cols = num_cols - (2 * (kernel_dim/2));
	}
	send_size = rows * cols;

	tot_rows = ((num_procs * rows_per_proc) - (2 * (kernel_dim/2)));
	tot_cols = (num_cols - (2 * (kernel_dim/2)));

	final_size =  tot_rows * tot_cols;
	result = malloc(final_size);

	printf("\t%d: snd rows=%d, cols=%d -- tot rows=%d, cols=%d\n", rank, rows, cols, tot_rows, tot_cols);

	MPI_Gather(out->data, send_size, MPI_UNSIGNED_CHAR, result,
				final_size, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	output = malloc(sizeof(struct bmp_image));
	output->data = result;
	output->header.height = (num_procs * rows_per_proc) - (2 * (kernel_dim/2));
	output->header.width = num_cols - (2 * (kernel_dim/2));
	output->header.planes = 1;
	output->header.size = (output->header.width * output->header.height * output->header.planes) + sizeof(struct bmp_header);
	return output;

}

void process_image(const char *filepath, int rank, int num_procs) {
	struct bmp_image *img, *out, *result;
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
	proc_rows = propagate_proc_rows(all_rows, rows_per_proc, num_cols, 3);
	exchange_proc_rows(proc_rows, rank, num_procs, rows_per_proc, num_cols);
	out = convolution(proc_rows, rank, num_procs, rows_per_proc, num_cols);
	result = aggregate_results(out, rank, num_procs, rows_per_proc, num_cols, 3);

	if (rank == 0 && out)
		bmp_describe(out);

	if (rank == 0 && result)
		bmp_describe(result);
/*
	free(out);
	free(proc_rows);
	if (rank == 0) {
		bmp_destroy(img);
		//bmp_destroy(result);
	}
*/
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
