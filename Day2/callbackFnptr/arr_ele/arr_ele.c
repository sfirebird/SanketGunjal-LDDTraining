#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARR_ELE     3

typedef void (*fnPtr)(int *);

void doubleFn(int *eleAdr){
    *eleAdr = *eleAdr + *eleAdr;
    return;
}

void squareFn(int *eleAdr){
     *eleAdr = *eleAdr * *eleAdr;
     return;
}

void callbackFn(int *eleAdr, fnPtr callback){
    return callback(eleAdr);
}

int main(){
    char op[20];
    int arr[ARR_ELE];

    printf("Enter integers in arr: \n");
    for(int start = 0; start < ARR_ELE; start++){
        scanf("%d", &arr[start]);
    }    

    printf("Enter the operations to be performed on the elements: \n");
    for(int start = 0; start < ARR_ELE; ){
        scanf("%s", op);
        if(strcmp("double", op) == 0){
            callbackFn((arr+start), doubleFn);
            start++;
        }
        else if(strcmp("square", op) == 0){
            callbackFn((arr+start), squareFn);
            start++;
        }
        else{
            printf("Usage Error!\n");
        }
    }
    
    printf("Modified array: \n");
    for(int start = 0; start < ARR_ELE; start++){
        printf("%d ", arr[start]);
    }

    printf("\n");
}
