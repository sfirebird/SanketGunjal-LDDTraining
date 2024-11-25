#ifndef __OPERATION_H__
#define __OPERATION_H__

/**/
typedef int (*fnPtr)(int, int);

typedef struct dynamicOp{
	int var1_int; 
	int var2_int;
	float var3_fp;
	float var4_fp;
	fnPtr operation;
}DOP;


/*
 * @params
 * @brief
 * @return
 */
int addFn(int x, int y);

/*
 * @params
 * @brief
 * @return
 */
int callbackFn(DOP *dynStr);


#endif
