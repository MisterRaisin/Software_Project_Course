#include <stdio.h>
#include <math.h>
#include <stdlib.h>



int DEFAULT_ITER = 200; 
int BASE_EXP_START = 3;
double CONVERGENCE_TARGET = 0.001;



double getDistance(double a[],double b[],int dim);

int main(int argc, char **argv)
{
    int i=0,j=0,k, maxIter; /* iterators and input*/
    char ch;
    int scalarCount=0;
    int baseExp=BASE_EXP_START; /* We add char by char, and this keeps track of the floating point*/
    int sign = 1;
    int thereWasPoint=0;
    double currentScalar=0;
    int dim=-1;
    int vector_count=0;
    double **vectors;
    double **prevClusters;
    double **clusters;
    double **sums;
    int *clusterSizes;
    int converged;
    int iter;

    if(argc < 2 || argc > 3){ /* Wrong number of arguments (1 or 2 depending on the iter max) + 1 of the file*/
        printf("An Error Has Occurred\n");
        return 1;
    }
    k = atoi(argv[1]);
    if(k <= 1){ /*Cluster input check*/
        printf("Invalid number of clusters!\n");
        return 1;
    }
    if(argc== 2){ /*If we didn't get no iter max, set the default*/
        maxIter = DEFAULT_ITER;
    }else{
        maxIter = atoi(argv[2]);
        if(maxIter <= 1 || maxIter >= 1000){ /* Iter input check*/
            printf("Invalid maximum iteration!\n");
            return 1;
        }
    }

  
    i=0;
    while ((ch = getc(stdin)) != EOF) { /* Finding the dimension and vector count*/
        if(ch==','){ /* End of scalar*/
            i++;
        }
        if(ch == 10){ /* linebreak (takes into account that theres a linebreak at the end of the last line)*/
            if(dim==-1){ /* first vector*/
                dim = i+1; /*  plus 1 because we dont count the last scalar*/
            }else{
                if(dim != i+1){ /* non uniform dimension*/
                    printf("An Error Has Occurred\n");
                    return 1;
                }
            }
            i=0;
            vector_count++;
        }
        
    }
    rewind(stdin); /* Reset the file pointer*/

    if(vector_count <= k || dim == 0){
        printf("Invalid number of clusters!\n");
        return 1;
    }

    vectors = malloc(vector_count * sizeof(double *));
    for(i = 0; i < vector_count; i++) {
        vectors[i] = malloc(dim * sizeof(double));
    }

    i=0;
    
    while ( i<vector_count  && (ch = getc(stdin)) != EOF) { /* processing all vectors*/
        if(ch==','){ /* comma, end of scalar*/

            if(!thereWasPoint){ /* Incase this is a whole number and we dont see a point, we should fix the floating point location here, see ch=="." case*/
                currentScalar *= pow(10,-baseExp-1);
            }
            vectors[i][scalarCount]=currentScalar*sign; /*Pushing current char*/

            currentScalar = 0; /* resetting helpers*/
            thereWasPoint = 0;
            sign = 1;
            baseExp = BASE_EXP_START;
            scalarCount++;
        }else if(ch=='-'){
            sign=-1;
        }
        else if(ch=='.'){
            currentScalar *= pow(10,-baseExp-1); /* Fixing in hindsight the floating point location*/
            baseExp = -1; /* Base for the next digit after the point*/
            thereWasPoint = 1; 
        }else if(ch == 10){ /* Line break*/
            if(scalarCount!=dim-1 && scalarCount!=0){ /* Non uniform dimension*/
                printf("An Error Has Occurred\n");
                return 1;
            }

            if(!thereWasPoint){ /* Incase this is a whole number and we dont see a point, we should fix the floating point location here, see ch=="." case*/
                currentScalar *= pow(10,-baseExp-1);
            }
            vectors[i][scalarCount]=currentScalar*sign; /* Updataing the last scalar*/

            currentScalar = 0; /* resetting helpers*/
            thereWasPoint = 0;
            sign = 1;
            baseExp = BASE_EXP_START;
            scalarCount=0;

            i++; /* incrementing vector count */
        }else{ /* Normal digit, we sum it to the scalar with the current floating point location*/
            currentScalar += (ch-'0')*pow(10,baseExp);
            baseExp--;
        }
    }



    clusters = malloc(k * sizeof(double *));
    for(i = 0; i < k; i++) {
        clusters[i] = malloc(dim * sizeof(double));
    }

    for(i = 0; i < k; i++) { /* Copy fist 8 vals*/
        for(j = 0; j < dim; j++) {
            clusters[i][j] = vectors[i][j];
        }
    }

    

    


    sums = malloc(k * sizeof(double *));
    for(i = 0; i < k; i++) {
        sums[i] = malloc(dim * sizeof(double));
    }

    clusterSizes = calloc(k, sizeof(int));
    prevClusters = malloc(k * sizeof(double *));
    
    for(i = 0; i < k; i++) {
        prevClusters[i] = malloc(dim * sizeof(double));
    }


    for(iter = 0; iter < maxIter && !converged; iter++) {
        converged = 1;

        /* Reset sums and sizes*/
        for(i = 0; i < k; i++) {
            for(j = 0; j < dim; j++) {
                sums[i][j] = 0;
            }
            clusterSizes[i] = 0;
        }

        /* Assign vectors to clusters*/
        for(i = 0; i < vector_count; i++) {
            int minCluster = 0;
            double minDist = getDistance(vectors[i], clusters[0], dim);

            for(j = 1; j < k; j++) {
                double dist = getDistance(vectors[i], clusters[j], dim);
                if (dist < minDist) {
                    minDist = dist;
                    minCluster = j;
                }
            }

            clusterSizes[minCluster]++;
            for(j = 0; j < dim; j++) {
                sums[minCluster][j] += vectors[i][j];
            }
        }

        /* Update clusters*/
        for(i = 0; i < k; i++) {
            for(j = 0; j < dim; j++) {
                prevClusters[i][j] = clusters[i][j];
                clusters[i][j] = clusterSizes[i] ? sums[i][j] / clusterSizes[i] : clusters[i][j];
            }

            if (getDistance(clusters[i], prevClusters[i], dim) > CONVERGENCE_TARGET) {
                converged = 0;
            }
        }
    }

    /* Print final clusters*/
    for(i = 0; i < k; i++) {
        for(j = 0; j < dim; j++) {
            printf("%.4f", clusters[i][j]);
            if (j < dim - 1) printf(",");
        }
        printf("\n");
    }

    /* Free memory*/
    for(i = 0; i < vector_count; i++) free(vectors[i]);
    free(vectors);

    for(i = 0; i < k; i++) {
        free(clusters[i]);
        free(sums[i]);
        free(prevClusters[i]);
    }
    free(clusters);
    free(sums);
    free(prevClusters);
    free(clusterSizes);

    return 0;
}



double getDistance(double a[],double b[],int dim){
    double sum = 0;
    double temp;
    int i;
    for(i=0;i<dim;i++){
        temp =  a[i]-b[i];
        sum += temp*temp;
    }
    return sqrt(sum);
}