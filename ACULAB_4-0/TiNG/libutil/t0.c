#include <stdio.h>

static int t0(char *fname)
{
 FILE *ofile = fopen(fname, "w");
 int empty = 1;
 if (!ofile) {
	perror("Error creating file");
	fprintf(stderr, "Cannot create file %s\n", fname);
	return 1;
 }
 for (;;) {
	char buff[1024];
	if (!fgets(buff, sizeof(buff), stdin)) break;
	fputs(buff, stdout);
	if (fputs(buff, ofile) == EOF) {
		perror("Error writing file");
		fprintf(stderr, "File: %s\n", fname);
		return 1;
	}
	empty = 0;
 }
 if (fclose(ofile)) {
	perror("Error closing file");
	fprintf(stderr, "File: %s\n", fname);
	return 1;
 }
 if (empty && unlink(fname)) {
	perror("Error deleting file");
	fprintf(stderr, "File: %s\n", fname);
	return 1;
 }
 return 0;
}

static int args(char **argv, char **p1)
{
 if (!*argv) return 1;
 *p1 = *argv++;
 return !!*argv;
}

int main(int argc, char **argv)
{
 char *fname;
 if (args(++argv, &fname)) {
	fprintf(stderr, "Usage: t0 filename\n");
	return 1;
 }
 return t0(fname);
}
