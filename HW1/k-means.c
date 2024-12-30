#include <stdio.h>
#include <math.h>
#include <stdlib.h>



int MAX_NUM_LENGTH = 20;
int DEFAULT_ITER = 200; 
int BASE_EXP_START = 3;
double convergenceTarget = 0.00001;



double getDistance(double a[],double b[],int dim);

int main(int argc, char **argv)
{
    int i=0,j=0,k, iter, dim=0; // iterators and dim
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
        iter = DEFAULT_ITER;
    }else{
        iter = atoi(argv[2]);
        if(iter < 1){ // Iter input check
            printf("Invalid maximum iteration!\n");
            return 1;
        }
    }

    char ch;
    while ((ch = getc(stdin)) != EOF && ch != 10) { // Finding the dimension
        if(ch==','){ // End of scalar
            dim++;
        }
        
    }
    dim++; // End of row doesn't get ++'d in the loop
    rewind(stdin); // Reset the file pointer



    
    double clusters[k][dim]; // Our clusters

    int vectorCount=0;
    int scalarCount=0;
    int baseExp=BASE_EXP_START; // We add char by char, and this keeps track of the floating point
    int sign = 1;
    int thereWasPoint=0;
    double currentScalar=0;



    while (vectorCount<k  && (ch = getc(stdin)) != EOF) { // its crucial that the count check is first, otherwise, we steal the first char of the next line
        if(ch==','){ // comma, end of scalar
            if(scalarCount >= dim){// Non uniform dimension
                printf("An Error Has Occurred\n");
                return 1;
            }

            if(!thereWasPoint){ // Incase this is a whole number and we dont see a point, we should fix the floating point location here, see ch=="." case
                currentScalar *= pow(10,-baseExp-1);
            }

            clusters[vectorCount][scalarCount]=currentScalar*sign; //Pushing current char

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

            clusters[vectorCount][scalarCount]=currentScalar*sign; // Updataing the last scalar

            currentScalar = 0; // resetting helpers
            thereWasPoint = 0;
            sign = 1;
            baseExp = BASE_EXP_START;
            scalarCount=0;

            vectorCount++; // incrementing vector count
        }else{ // Normal digit, we sum it to the scalar with the current floating point location
            currentScalar += (ch-'0')*pow(10,baseExp);
            baseExp--;
        }
    }




   


    vectorCount = 0; // we dont include the cluster initialization in the count.
    scalarCount = 0; // resetting helpers
    currentScalar = 0;
    thereWasPoint = 0;
    sign = 1;
    baseExp = BASE_EXP_START;

    int centroidLengths[k]; // Holds the size of each cluster (we dont actually hold an array of them)
    for(i=0;i<k;i++){
        centroidLengths[k]=1;
    }
    int minDiffAt; // Helpers
    double minDiff;
    double diff;
    double currentCentroidDifference[dim];
    double minCentroidDifference[dim];

    double oldCentroid[dim];
    double change;

    double currentVector[dim];


    char convergenceArray[k]; // To keep track of the clusters that have already reached the convergence target
    int convergenceCount=0; // To keep track the number of clusters that have did so


   while ((ch = getc(stdin)) != EOF && vectorCount<iter && convergenceCount < k){
        if(ch == 10){ // Line break
            if(scalarCount!=dim-1 && scalarCount!=0){
                printf("An Error Has Occurred\n");
                return 1;
            }

            if(!thereWasPoint){
                currentScalar *= pow(10,-baseExp-1);
            }

            currentVector[scalarCount]=currentScalar*sign;
            currentScalar = 0;
            sign = 1;
            baseExp = BASE_EXP_START;
            scalarCount=0;
            vectorCount++;
            thereWasPoint = 0;

            minDiff=INFINITY;
            for(i=0;i<k;i++){
                diff = getDistance(currentVector,clusters[i],dim);
                if(diff < minDiff){
                    minDiff = diff;
                    minDiffAt = i;
                }
            }
            printf("%d\n",minDiffAt);

            for(j=0;j<dim;j++){
                oldCentroid[j] = clusters[minDiffAt][j];
            }
            centroidLengths[minDiffAt]++;
            for(j=0;j<dim;j++){
                clusters[minDiffAt][j] = clusters[minDiffAt][j]*(centroidLengths[minDiffAt]-1)+currentVector[j];
                clusters[minDiffAt][j] /= centroidLengths[minDiffAt];
            }

            change = getDistance(oldCentroid,clusters[minDiffAt],dim);
            if(change <= convergenceTarget){
                if(convergenceArray[minDiffAt] == 0){
                    convergenceCount++;
                }
                convergenceArray[minDiffAt] = 1;
            }else{
                if(convergenceArray[minDiffAt] == 1){
                    convergenceCount--;
                }
                convergenceArray[minDiffAt] = 0;
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
        }else if(ch==','){
            if(scalarCount >= dim){
                printf("An Error Has Occurred\n");
                return 1;
            }

            if(!thereWasPoint){
                currentScalar *= pow(10,-baseExp-1);
            }

            currentVector[scalarCount]=currentScalar*sign;

            currentScalar = 0;
            thereWasPoint = 0;
            sign = 1;
            baseExp = BASE_EXP_START;
            scalarCount++;
        }else if(ch=='-'){
            sign=-1;
        }
        else if(ch=='.'){
            currentScalar *= pow(10,-baseExp-1);
            baseExp = -1;
            thereWasPoint = 1;
        }else{
            currentScalar += (ch-'0')*pow(10,baseExp);
            baseExp--;
        }
    }




    printf("dim: %d\n",dim);
    printf("vector count: %d\n",vectorCount);
    printf("convergence count: %d\n",convergenceCount);
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