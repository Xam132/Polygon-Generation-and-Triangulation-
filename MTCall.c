#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define RAD 50
#define MAX 1000000

int cmp(const void *a, const void *b){
    float c = (*(float *)a - *(float *)b);
    return (int)(c*100);
}

float min(float a,float b){
    return (a > b)?b:a;
}

float max(float a,float b){
    return (a > b)?a:b;
}

typedef struct point{
    int x;int y;
}point;

float dist(point p1,point p2){
    return sqrt(pow(p2.x-p1.x,2)+pow(p2.y-p1.y,2));
}

float cost(point p1,point p2,point p3){
    return dist(p1,p2) + dist(p2,p3) + dist(p3,p1);
}

void convex_poly(point arr[],int n,int rad){
    for(int i = 0;i < n;i++){
        arr[i].x = round(rad*cos(2*3.14*i/n));
        arr[i].y = round(rad*sin(2*3.14*i/n));
    }
}

void printarr(point arr[],int n){
    for(int i = 0;i < n;i++){
        if(i == n-1){
            printf("%d",arr[i].x);
        }
        else {
            printf("%d, ", arr[i].x);
        }
    }
    printf("\n");
    for(int i = 0;i < n;i++){
        if(i == n-1){
            printf("%d",arr[i].y);
        }
        else {
            printf("%d, ", arr[i].y);
        }
    }
    printf("\n");
}

int search(float **arr,float d,int i,int j,int l,int k){
    if(arr[i][j] == d){
        if((i == l && j == k) || (i == k && j == l)){return 0;}
        else if((i == l || j == k) || (i == k || j == l)) {
            return 1;
        }
        else if((i < min(k,l))&&(j < min(k,l))){
            return 1;
        }
        else if((i > max(k,l))&&(j > max(k,l))){
            return 1;
        }
        else if((i > min(k,l) && i < max(k,l))
              &&(j > min(k,l) && j < max(k,l))){
            return 1;
        }
        else if((i < min(k,l) && j > max(k,l))||(i > max(k,l) && j < min(k,l))){
            return 1;
        }
        else{return 0;}
    }
    return 0;
}

int com_search(float **arr,float d,int i,int j,int n,int **crr){
    if(n == 0){return 1;}
    int c;
    for(int l = 0;l < n;l++){
        c = search(arr,d,i,j,crr[l][0],crr[l][1]);
        if(c == 0){return 0;}
    }
    return 1;
}

void savearr(FILE *fp,point arr[],int n){
    for(int j = 0;j < n;j++) {
       if (j == n - 1) {
            fprintf(fp, "%d \n",arr[j].x);
        }
        else{
            fprintf(fp, "%d, ",arr[j].x);
        }
    }
    for(int j = 0;j < n;j++) {
        if (j == n - 1) {
            fprintf(fp, "%d \n",arr[j].y);
        }
        else{
            fprintf(fp, "%d, ",arr[j].y);
        }
    }
    fprintf(fp,"\n");
}

void printarray(float arr[],int n){
    for(int i = 0;i < n;i++){
        printf("%.2f ",arr[i]);
    }
    printf("\n");
}

float perimeter(point arr[],int n){
    float a = dist(arr[0],arr[n-1]);
    for(int i = 0;i < n-1;i++){
        a = a + dist(arr[i],arr[i+1]);
    }
    return a;
}

float bruteMTC(point arr[],int l,int h){
    if(h < l+2){return 0;}
    float res = MAX;
    for(int i = l+1;i < h;i++){
        res = min(res, bruteMTC(arr,l,i)+bruteMTC(arr,i,h)
                       + cost(arr[l],arr[i],arr[h]));
    }
    return res;
}

float dynamicMTC(point arr[],int n){
    float **dp = malloc(sizeof(float*)*n);
    for(int i = 0;i < n;i++){
        dp[i] = malloc(sizeof(float)*n);
    }
    for(int i = 0;i < n;i++) {
        for (int j = 0; j < n; j++) {
            if (j < i+2) { dp[i][j] = 0; }
            else{ dp[i][j] = MAX; }
        }
    }
    int m = 2,b;
    while(m < n) {
        for (int i = 0; i < n; i++) {
            b = min(i + m, n);
            for (int k = i + 1; k < b; k++) {
                dp[i][b] = min(dp[i][b], dp[i][k] + dp[k][b]
                                         + cost(arr[i], arr[k], arr[b]));
            }
        }
        m++;
    }
    float z = dp[0][n-1];
    for(int i = 0;i < n;i++){
        free(dp[i]);
    }
    free(dp);
    return z;
}

float greedyMTC(point arr[],int n){
    float **diag = malloc(sizeof(float*)*n);
    float *brr = malloc(sizeof(float)*n*(n-3)/2);
    int l = 0;
    for(int i = 0;i < n;i++){
        diag[i] = malloc(sizeof(float)*n);
        for(int j = 0;j < n;j++) {
            if (j < i + 2) {
                diag[i][j] = 0;
            }
            else {
                if (i == 0 && j == n - 1) {
                    diag[i][j] = 0;
                }
                else {
                    diag[i][j] = dist(arr[i], arr[j]);
                    brr[l++] = diag[i][j];
                }
            }
        }
    }
    qsort(brr,l,sizeof(float),cmp);
    int **crr = malloc(sizeof(int*)*(n-3));
    int z = 0,y;
    float d = perimeter(arr,n);
    for(int i = 0;i < n-3;i++) {
        crr[i] = malloc(sizeof(int)*2);
        y = 0;
        while(y != 1) {
            for (int m = 0; m < n; m++) {
                for (int k = m + 2; k < n; k++) {
                    if (com_search(diag, brr[z], m, k, i, crr)) {
                        crr[i][0] = m;
                        crr[i][1] = k;
                        y = 1;
                        d = d + 2*dist(arr[m],arr[k]);
                        break;
                    }
                }
                if(y == 1){break;}
            }
            if(z >= l){break;}
            else{z++;}
        }
    }
    free(brr);
    for(int i = 0;i < n;i++) { free(diag[i]); }
    for(int i = 0;i < n-3;i++) { free(crr[i]); }
    free(diag); free(crr);
    return d;
}

int main(){
    int n;
    while(1) {
        printf("Enter num of sides :");
        scanf("%d", &n);
        if(n == 0){break;}
        point *arr = malloc(sizeof(point) * n);
        convex_poly(arr, n, RAD);
        printarr(arr, n);
        float a = dynamicMTC(arr, n);
        printf("Dynamic : %.4f\n", a);
        float b = bruteMTC(arr, 0, n - 1);
        printf("Brute : %.4f\n", b);
        float c = greedyMTC(arr, n);
        printf("Greedy : %.4f\n", c);
        free(arr);
    }
    return 0;
}
