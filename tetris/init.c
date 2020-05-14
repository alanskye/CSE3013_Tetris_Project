#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    FILE *fp = fopen("genesinit.txt", "w");
    srand((unsigned int)time(NULL));
    for (int i = 0; i < 80; i++) {
        fprintf(fp, "%lf ", 10 * ((double)rand()) / RAND_MAX); // line 
        fprintf(fp, "%lf ", 10 * ((double)rand()) / RAND_MAX); // touch f
        fprintf(fp, "%lf ", 10 * ((double)rand()) / RAND_MAX); // touch w
        fprintf(fp, "%lf ", 10 * ((double)rand()) / RAND_MAX); // touch b
        for (int j = 0; j < 4; j++)
            fprintf(fp, "%lf ", -10 + 20 * ((double)rand()) / RAND_MAX); 
        fprintf(fp, "%lf ", -10 * ((double)rand()) / RAND_MAX); // col max
        fprintf(fp, "%lf ", -10 * ((double)rand()) / RAND_MAX); // col sum
        fprintf(fp, "\n");
    }
    fclose(fp);
    return 0;
}
