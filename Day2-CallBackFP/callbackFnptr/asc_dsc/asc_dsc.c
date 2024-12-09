#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARR_ELE		5

typedef void (*fnPtr)(int *arrPtr, int size);

void ascend(int *arrPtr, int size);
void descend(int *arrPtr, int size);
void callbackFn(int *arrPtr, int size,  fnPtr callback);

int main(int argc, char **argv){
	int arr[ARR_ELE];

	printf("Enter the array elements: \n");
	for(int start = 0; start < ARR_ELE; start++){
		scanf("%d", &arr[start]);
	}

	// decided which fucnt to call depending upon the user
	if(strcmp("ascend", argv[1]) == 0){
		callbackFn(arr, (int)ARR_ELE, ascend);	
	}
	else if(strcmp("descend", argv[1]) == 0){
		callbackFn(arr, ARR_ELE, descend);
	}
	else{
		printf("Usage error!\n");
		return 0;
	}

	printf("The arr: \n");
	for(int start = 0; start < ARR_ELE; start++){
		printf("%d ", arr[start]);
	}
}

void ascend(int *arrPtr, int size){
	
	int start;
	for(start = 0; start < size; start++){
		int loop;
		for(loop = start + 1; loop < size; loop++){
			if(arrPtr[start] > arrPtr[loop]){
				// swap
				int temp = arrPtr[start];
				arrPtr[start] = arrPtr[loop];
				arrPtr[loop] = temp;
			}
		}
	}	
	return;
}

void descend(int *arrPtr, int size){
	
	int start;
	for(start = 0; start < size; start++){
		int loop;
		for(loop = start + 1; loop < size; loop++){
			if(arrPtr[start] < arrPtr[loop]){
				int temp = arrPtr[start];
				arrPtr[start] = arrPtr[loop];
				arrPtr[loop] = temp;
			}
		}
	}	
	return;
}

void callbackFn(int *arrPtr, int size,  fnPtr callback){
	return callback(arrPtr, size);
}


