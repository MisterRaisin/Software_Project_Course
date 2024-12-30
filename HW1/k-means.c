#include <stdio.h>
#include <math.h>
#include <stdlib.h>



int DEFAULT_ITER = 200; 
int BASE_EXP_START = 3;
double CONVERGENCE_TARGET = 0.001;



double getDistance(double a[],double b[],int dim);

int main(int argc, char **argv)
{
    int i=0,j=0,k, maxIter; // iterators and input
    if(argc < 2 || argc > 3){ // Wrong number of arguments (1 or 2 depending on the iter max) + 1 of the file
        printf("An Error Has Occurred\n");
        return 1;
    }
    k = atoi(argv[1]);
    if(k == 0){ // Cluster input check
        printf("Invalid number of clusters!\n");
        return 1;
    }
    if(argc== 2){ // If we didn't get no iter max, set the default
        maxIter = DEFAULT_ITER;
    }else{
        maxIter = atoi(argv[2]);
        if(maxIter < 1){ // Iter input check
            printf("Invalid maximum iteration!\n");
            return 1;
        }
    }

    char ch;
    int dim=-1;
    int vector_count=0;
    i=0;
    while ((ch = getc(stdin)) != EOF) { // Finding the dimension and vector count
        if(ch==','){ // End of scalar
            i++;
        }
        if(ch == 10){ // linebreak (takes into account that theres a linebreak at the end of the last line)
            if(dim==-1){ // first vector
                dim = i+1; //  plus 1 because we dont count the last scalar
            }else{
                if(dim != i+1){ // non uniform dimension
                    printf("An Error Has Occurred\n");
                    return 1;
                }
            }
            i=0;
            vector_count++;
        }
        
    }
    rewind(stdin); // Reset the file pointer

    if(vector_count < k){
        printf("An Error Has Occurred\n");
        return 1;
    }

    double **vectors = malloc(vector_count * sizeof(double *));
    for (int i = 0; i < vector_count; i++) {
        vectors[i] = malloc(dim * sizeof(double));
    }

    i=0;
    int scalarCount=0;
    int baseExp=BASE_EXP_START; // We add char by char, and this keeps track of the floating point
    int sign = 1;
    int thereWasPoint=0;
    double currentScalar=0;
    while ( i<vector_count  && (ch = getc(stdin)) != EOF) { // processing all vectors
        if(ch==','){ // comma, end of scalar

            if(!thereWasPoint){ // Incase this is a whole number and we dont see a point, we should fix the floating point location here, see ch=="." case
                currentScalar *= pow(10,-baseExp-1);
            }
            vectors[i][scalarCount]=currentScalar*sign; //Pushing current char

            currentScalar = 0; // resetting helpers
            thereWasPoint = 0;
            sign = 1;
            baseExp = BASE_EXP_START;
            scalarCount++;
        }else if(ch=='-'){
            sign=-1;
        }
        else if(ch=='.'){
            currentScalar *= pow(10,-baseExp-1); // Fixing in hindsight the floating point location
            baseExp = -1; // Base for the next digit after the point
            thereWasPoint = 1; 
        }else if(ch == 10){ // Line break
            if(scalarCount!=dim-1 && scalarCount!=0){ // Non uniform dimension
                printf("An Error Has Occurred\n");
                return 1;
            }

            if(!thereWasPoint){ // Incase this is a whole number and we dont see a point, we should fix the floating point location here, see ch=="." case
                currentScalar *= pow(10,-baseExp-1);
            }
            vectors[i][scalarCount]=currentScalar*sign; // Updataing the last scalar

            currentScalar = 0; // resetting helpers
            thereWasPoint = 0;
            sign = 1;
            baseExp = BASE_EXP_START;
            scalarCount=0;

            i++; // incrementing vector count
        }else{ // Normal digit, we sum it to the scalar with the current floating point location
            currentScalar += (ch-'0')*pow(10,baseExp);
            baseExp--;
        }
    }



    double **clusters = malloc(k * sizeof(double *));
    for (int i = 0; i < k; i++) {
        clusters[i] = malloc(dim * sizeof(double));
    }

    for (int i = 0; i < k; i++) { // Copy fist 8 vals
        for (int j = 0; j < dim; j++) {
            clusters[i][j] = vectors[i][j];
        }
    }

    

    


    int iterationCount = 0;
    char converged = 0;
    int currentVectorIndex;
    double* currentVector;


    int* clusterSize = malloc(k * sizeof(int));

    double **originalClusters = malloc(k * sizeof(double *));
    for (int i = 0; i < k; i++) {
        originalClusters[i] = malloc(dim * sizeof(double));
    }
   
    for(iterationCount = 0;iterationCount < maxIter && !converged;iterationCount++){
        
        for(i=0;i<k;i++){
            clusterSize[i] = 0;
        }
        for (int i = 0; i < k; i++) { // Copy fist 8 vals
            for (int j = 0; j < dim; j++) {
                originalClusters[i][j] = clusters[i][j];
            }
        }
        for(currentVectorIndex=0;currentVectorIndex < vector_count;currentVectorIndex++){
            currentVector = vectors[currentVectorIndex];

            
            double minDiff = INFINITY;
            int minDiffAt = 0;
            double diff;
            for(i=0;i<k;i++){
                diff = getDistance(originalClusters[i],currentVector,dim);
                if(diff < minDiff){
                    minDiff = diff;
                    minDiffAt = i;
                }
            }
            clusterSize[minDiffAt] ++;
            for(i=0;i<dim;i++){
                clusters[minDiffAt][i] = clusters[minDiffAt][i]*(clusterSize[minDiffAt]-1)+currentVector[i];
                clusters[minDiffAt][i] /= clusterSize[minDiffAt];
            }
        }
        converged = 1;
        for(i=0;i<k;i++){
            if(getDistance(clusters[i],originalClusters[i],dim) > CONVERGENCE_TARGET){
                converged = 0;
            }
        }
    }


    for(i=0;i<k;i++){ // Printing the updated clusters (their centroids)
        for(j=0;j<dim-1;j++){
            printf("%.4f,",clusters[i][j]);
        }
        printf("%.4f\n",clusters[i][j]);
    }
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

 // for(i=0;i<k;i++){
            //     diff = 0;
            //     for(j=0;j<dim;j++){
            //         currentCentroidDifference[j] = (currentVector[j] - clusters[i][j])/vectorCount;
            //         diff += currentCentroidDifference[j]*currentCentroidDifference[j];
            //     }
            //     diff = sqrt(diff);
            //     if(diff<minDiff){
            //         minDiff = diff;
            //         minDiffAt = i;
            //         for(j=0;j<dim;j++){
            //             minCentroidDifference[j] = currentCentroidDifference[j];
            //         }
            //     }
            // }
            // for(i=0;i<k;i++){
            //     clusters[minDiffAt][i] = clusters[minDiffAt][i] + minCentroidDifference[i];
            // }
            

            
            // if(minDiff*vectorCount < convergenceTarget){
            //     if(convergenceArray[minDiffAt] == 0){
            //         convergenceArray[minDiffAt] = 1;
            //         convergenceCount++;
            //     }
            // }

 // int iterationCount = 0;
    // int convergenceCount = 0 ;

    // double* centroidLengths = malloc(k * sizeof(double)); // Keep track of cluster size for each iteration
    // double *oldCentroid = malloc(dim * sizeof(double)); // Helper for finding the difference between the old centroid and the updated one
    // double *clusterReachedConvergenceTarget = malloc(k * sizeof(double)); // Helper for finding the difference between the old centroid and the updated one
    // int minDiffAt; // More helpers
    // double minDiff;
    // double diff;
    // double change;

    // for (iterationCount=0; iterationCount < maxIter || convergenceCount == k;iterationCount++){

    //     convergenceCount = 0;
    //     for(i=0;i<k;i++){
    //         centroidLengths[i]=1; // Starts at 1 because of the init values
    //     }
    //     for(i=0;i<k;i++){
    //         clusterReachedConvergenceTarget[i]=0; // reset
    //     }

       
    //     for(i=0;i<vector_count;i++){

    //         minDiff=INFINITY;
    //         for(j=0;j<k;j++){
    //             diff = getDistance(vectors[i],clusters[j],dim);
    //             if(diff < minDiff){
    //                 minDiff = diff;
    //                 minDiffAt = j;
    //             }
    //         }

    //         for(j=0;j<dim;j++){
    //             oldCentroid[j] = clusters[minDiffAt][j];
    //         }
    //         centroidLengths[minDiffAt]++;
    //         for(j=0;j<dim;j++){
    //             clusters[minDiffAt][j] = clusters[minDiffAt][j]*(centroidLengths[minDiffAt]-1)+vectors[i][j];
    //             clusters[minDiffAt][j] /= centroidLengths[minDiffAt];
    //         }

    //         change = getDistance(oldCentroid,clusters[minDiffAt],dim);
            
    //         if(change <= CONVERGENCE_TARGET && clusterReachedConvergenceTarget[minDiffAt] == 0){
    //             convergenceCount++;
    //             clusterReachedConvergenceTarget[minDiffAt] = 1;
    //         }else if(clusterReachedConvergenceTarget[minDiffAt] == 1){
    //             convergenceCount--;
    //             clusterReachedConvergenceTarget[minDiffAt] = 0;
    //         }
        
    //     }
    // }
