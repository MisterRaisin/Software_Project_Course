#include <stdlib.h>
#include <stdio.h>
#include <math.h>


int MAX_NUM_LENGTH = 20;
int DEFAULT_ITER = 200; 
int BASE_EXP_START = 3;
double convergenceTarget = 0.00001;



double getDistance(double a[],double b[],int dim);

int main(int argc, char **argv)
{
    int i=0,j=0,k, iter, dim=0;
    if(argc < 2 || argc > 3){
        printf("An Error Has Occurred");
        return 1;
    }
    k = atoi(argv[1]);
    if(k == 0){
        printf("Invalid number of clusters!");
        return 1;
    }
    if(argc== 2){
        iter = DEFAULT_ITER;
    }else{
        iter = atoi(argv[2]);
        if(iter < 1){
            printf("Invalid maximum iteration!");
            return 1;
        }
    }

    char ch;
    while ((ch = getc(stdin)) != EOF && ch != 10) { // Finding the dimension
        if(ch==','){
            dim++;
        }
        
    }
    dim++;
    // printf("dim: %d\n",dim);

    rewind(stdin);



    
    double clusters[k][dim];

    int vectorCount=0;
    int scalarCount=0;
    int baseExp=BASE_EXP_START;
    int sign = 1;


  


    double currentScalar=0;



    while ((ch = getc(stdin)) != EOF && vectorCount<k) {
        if(ch == 10){ // Line break
            if(scalarCount!=dim-1 && scalarCount!=0){
                 printf("\nPlease provide a valid file inwhich all vectors are of the same dimension. At k vector number %d",vectorCount);
                return 1;
            }
            clusters[vectorCount][scalarCount]=currentScalar*sign;

            for(j=0;j<dim-1;j++){
                printf("%lf,", clusters[vectorCount][j]);
            }
            printf("%lf\n", clusters[vectorCount][j]);



            currentScalar = 0;
            sign = 1;
            baseExp = BASE_EXP_START;
            scalarCount=0;
            vectorCount++;
        }else if(ch==','){
            if(scalarCount >= dim){
                printf("\nPlease provide a valid file inwhich all vectors are of the same dimension. At k vector number %d",vectorCount);
                return 1;
            }
            clusters[vectorCount][scalarCount]=currentScalar*sign;
            currentScalar = 0;
            sign = 1;
            baseExp = BASE_EXP_START;
            scalarCount++;
        }else if(ch=='-'){
            sign=-1;
        }
        else if(ch=='.'){
            currentScalar *= pow(10,-baseExp-1);
            baseExp = -1;
        }else{
            currentScalar += (ch-'0')*pow(10,baseExp);
            baseExp--;
        }
    }




   


    vectorCount = 0; // we dont include the cluster initialization in the count.
    scalarCount = 0;
    currentScalar = 0;
    sign = 0;
    baseExp = BASE_EXP_START;


    int minDiffAt;
    double minDiff=INFINITY;
    double diff;
    double currentCentroidDifference[dim];
    double minCentroidDifference[dim];

    double oldCentroid[dim];
    double change;

    double currentVector[dim];


    char convergenceArray[k];
    int convergenceCount=0;
    for(i=0;i<k;i++){
        convergenceArray[i] = 0;
    }


   while ((ch = getc(stdin)) != EOF && vectorCount<iter && convergenceCount < k) {
        if(ch == 10){ // Line break
            if(scalarCount!=dim-1 && scalarCount!=0){
                printf("\nPlease provide a valid file inwhich all vectors are of the same dimension. At vector number %d",vectorCount);
                return 1;
            }
            currentVector[scalarCount]=currentScalar*sign;
            currentScalar = 0;
            sign = 1;
            baseExp = BASE_EXP_START;
            scalarCount=0;
            vectorCount++;

            for(j=0;j<dim-1;j++){
                printf("%lf,",currentVector[j]);
            }
            printf("%lf\n",currentVector[j]);



            for(i=0;i<k;i++){
                diff = getDistance(currentVector,clusters[i],dim);
                if(diff < minDiff){
                    minDiff = diff;
                    minDiffAt = i;
                }
            }

            for(j=0;j<dim;j++){
                oldCentroid[j] = clusters[minDiffAt][j];
            }

            for(j=0;j<dim;j++){
                clusters[minDiffAt][j] = clusters[minDiffAt][j]*(vectorCount-1)+currentVector[j];
                clusters[minDiffAt][j] /= vectorCount;
            }

            if(!convergenceArray[minDiffAt]){
                change = getDistance(oldCentroid,clusters[minDiffAt],dim);
                if(change <= convergenceTarget){
                    convergenceArray[minDiffAt] = 1;
                    convergenceCount++;
                }
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
                printf("\nPlease provide a valid file inwhich all vectors are of the same dimension. At vector number %d",vectorCount);
                return 1;
            }
            currentVector[scalarCount]=currentScalar*sign;
            currentScalar = 0;
            sign = 1;
            baseExp = BASE_EXP_START;
            scalarCount++;
        }else if(ch=='-'){
            sign=-1;
        }
        else if(ch=='.'){
            currentScalar *= pow(10,-baseExp-1);
            baseExp = -1;
        }else{
            currentScalar += (ch-'0')*pow(10,baseExp);
            baseExp--;
        }
    }

    printf("vector count: %d\n",vectorCount);
    printf("convergence count: %d\n",convergenceCount);
    for(i=0;i<k;i++){
        for(j=0;j<dim-1;j++){
            printf("%lf,",clusters[i][j]);
        }
        printf("%lf\n",clusters[i][j]);
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