#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    FILE *fp = fopen("genesinit.txt", "w");
    srand((unsigned int)time(NULL));
    for (int i = 0; i < 128; i++) {
        fprintf(fp, "%lf ", 100 * ((double)rand()) / RAND_MAX); // line 
        fprintf(fp, "%lf ", 100 * ((double)rand()) / RAND_MAX); // touch f
        fprintf(fp, "%lf ", 100 * ((double)rand()) / RAND_MAX); // touch w
        fprintf(fp, "%lf ", 100 * ((double)rand()) / RAND_MAX); // touch b
        fprintf(fp, "%lf ", -100 + 200 * ((double)rand()) / RAND_MAX); // max diff
        fprintf(fp, "%lf ", -100 + 200 * ((double)rand()) / RAND_MAX); // roughness
        fprintf(fp, "%lf ", -100 + 200 * ((double)rand()) / RAND_MAX); // stdev
        fprintf(fp, "%lf ", -100 + 200 * ((double)rand()) / RAND_MAX); // abs max min
        fprintf(fp, "%lf ", -100 * ((double)rand()) / RAND_MAX); // col max
        fprintf(fp, "%lf ", -100 * ((double)rand()) / RAND_MAX); // col sum
        fprintf(fp, "%lf ", -100 * ((double)rand()) / RAND_MAX); // hole after
        fprintf(fp, "\n");
    }
    fclose(fp);
    return 0;
}
