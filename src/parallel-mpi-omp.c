#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/conv.h"
#include "include/image.h"

void propagate_img_dimensions(int *rows_per_proc, int *num_cols) {
	MPI_Bcast(rows_per_proc, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(num_cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

unsigned char *scatter(unsigned char *all_rows, int rows_per_proc, int num_cols, int kernel_dim) {
	int num_rows;
	unsigned char *proc_rows;

	num_rows = rows_per_proc + (2 * (kernel_dim/2));
	proc_rows = calloc(num_rows * num_cols, 1);

	MPI_Scatter(all_rows, rows_per_proc * num_cols,
				MPI_UNSIGNED_CHAR, &proc_rows[(kernel_dim/2)*num_cols],
				rows_per_proc * num_cols,
				MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	return proc_rows;
}

// odd procs send, event procs receive
void exchange_adjacent_rows(unsigned char *proc_rows, int rank, int num_procs, int rows_per_proc, int num_cols, int is_odd_first) {
	int offset;

	if ((rank+is_odd_first) & 1) {
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

struct raw_image *convolution2D(unsigned char *img_rows, int rank, int world_size, int rows_per_proc, int num_cols) {
	int n = 3;
	char kernel[9] = {0, 0, 0, 0, 1, 0, 0, 0, 0};
	struct raw_image *in, *out;

	in = raw_create_empty(num_cols, rows_per_proc + (2*(n/2)), 1);
	in->data = img_rows;

	out = conv_2d_parallel(in, kernel, n);
	return out;
}

unsigned char *gather(struct raw_image *my_result, int rank, int num_procs) {
	int send_size;
	unsigned char *final_rows = NULL;

	send_size = my_result->width * my_result->height * my_result->nchannels;

	if (rank == 0) {
		final_rows = malloc(num_procs * send_size);
	}

	MPI_Gather(my_result->data, send_size, MPI_UNSIGNED_CHAR, final_rows,
				send_size, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	return final_rows;
}

void save_intermediate_results(struct raw_image *my_result, int rank) {
	char filename[100];
	struct bmp_image *tmp_bmp = raw_to_bmp(my_result);
	snprintf(filename, 100, "rank_%d.bmp", rank);
	bmp_save(tmp_bmp, filename);
	bmp_destroy(tmp_bmp);
}

struct raw_image * get_input_image(const char *filepath) {
	struct bmp_image *src_bmp = bmp_load(filepath);
	struct raw_image *src_raw = bmp_to_raw(src_bmp, 0);
	bmp_destroy(src_bmp);
	return src_raw;
}

void process_image(const char *filepath, int rank, int num_procs) {
	int n = 3;
	int num_cols, rows_per_proc;
	unsigned char *my_rows = NULL;
	unsigned char *all_rows = NULL;
	unsigned char *final_rows = NULL;
	struct bmp_image *dst_bmp = NULL;
	struct raw_image *image = NULL;
	struct raw_image *my_result = NULL;
	struct raw_image *final_image = NULL;

	if (rank == 0) {
		image = get_input_image(filepath);
		all_rows = image->data;

		// WARNING: must be multiple of world_size
		rows_per_proc = image->height / num_procs;
		num_cols = image->width;
	}

	propagate_img_dimensions(&rows_per_proc, &num_cols);

	my_rows = scatter(all_rows, rows_per_proc, num_cols, n);

	exchange_adjacent_rows(my_rows, rank, num_procs, rows_per_proc, num_cols, 0);
	exchange_adjacent_rows(my_rows, rank, num_procs, rows_per_proc, num_cols, 1);

	my_result = convolution2D(my_rows, rank, num_procs, rows_per_proc, num_cols);
	save_intermediate_results(my_result, rank);

	final_rows = gather(my_result, rank, num_procs);

	if (rank == 0) {
		final_image = raw_create_empty(num_cols, num_procs * rows_per_proc - 1, 1);
		final_image->data = &final_rows[num_cols];
		dst_bmp = raw_to_bmp(final_image);
		bmp_save(dst_bmp, "result.bmp");
		bmp_destroy(dst_bmp);

		raw_destroy(image);
	}
}

int main(int argc, char *argv[]) {
	int nprocs, rank;

	if (argc != 2) {
    	fprintf(stderr, "Usage: parallel-mpi image.bmp\n");
    	exit(1);
  	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

	process_image(argv[1], rank, nprocs);

	MPI_Barrier(MPI_COMM_WORLD);
  	MPI_Finalize();
}
