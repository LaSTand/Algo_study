/* 
 *  Simple string quicksort algorithm
 */
#include <stdio.h>
#include <string.h>
// swap the strings
void swap(char * a, char * b)
{
	char tmp[20];
	strcpy(tmp, a);
	strcpy(a, b);
	strcpy(b, tmp);
}
// partition function(Hoare partition)
int partition(char (*arr)[20], int lo, int hi)
{
	int pivot = lo;		// Set pivot as the string on the far left.
	int c = lo-1;		// 'c' is index that low(left) sting.
	int d = hi+1;		// 'd' is index that high(right) string.
	// loop forever
	while(1)
	{
		do{
			c = c+1;	// Search next string.(To right)
		}while(strcmp(arr[pivot], arr[c])>0);	// Find a string that bigger than pivot.

		do{
			d = d-1;	// Serach next string.(To left)
		}while(strcmp(arr[pivot], arr[d])<0);	// Find a string that smaller than pivot.

		if(c >= d)	// This means that 'pL >= pR'
			return d;	// Return pR.
		swap(arr[c], arr[d]);	// swap the strings. Big string to right, small string to left
	}
}
// quicksort function(Hoare Partition)
void quicksort(char (*arr)[20], int lo, int hi)
{
	int p;		// 'p' is a variable that indicate where dividing occur.
	if(lo < hi)
	{	
		p = partition(arr, lo, hi);	// Divide occur.
		quicksort(arr, lo, p);		// Left side.
		quicksort(arr, p+1, hi);	// Right side.
	}
}

int main()
{
	FILE * fp_input = fopen("InputList.txt", "rt");	
	FILE * fp_output = fopen("OutputList.txt", "wt");

	char list[20][20];	// 2-dimension array that save the strings from 'InputList.txt'

	int idx = 0;		// Index. Indicates how many string are saved.
	int i, j, k;		// Variables for loop;
	int high, low;		// Uses as pL, pR. 
	// File open succeed check.
	if(!fp_input)
	{
		printf("No file exist !! \n");
		return -1;
	}
	// Read 'InputList.txt' and write into array 'list'
	while(!feof(fp_input))
	{
		fgets(list[idx], 20, fp_input);
		if(feof(fp_input) != 0)				//There's no 'new line' at the end of file.
		{
			for(i = 0 ; i < 20 ; i++)		//At the end of file, add 'new line' at the end array.
			{
				if(list[idx][i]==NULL)		//At the last array's end.
				{
					list[idx][i] = '\n';	//add 'new line'
					list[idx][i+1] = NULL;	//and add NULL to the next component.
					break;
				}
			}
		}
		idx++;
		if(idx >= 20)						// 'list' have set [20][20], so max num of string is 20!
		{
			printf("Max number of strings is %d !!!\n", idx-1);
			return -1;
		}		
	}

	fclose(fp_input);	// Close file stream.

	printf("==================\n");
	printf("  Before Sorting  \n");
	printf("==================\n");
	// Print saved strings in the array 'list'.
	for(j = 0 ; j < idx ; j++)
	{
		printf("%s", list[j]);
	}
	// Set pL and pR.
	low = 0;
	high = idx-1;	// sub 1. Because array index starts from '0'.
	
	printf("\n==================\n");
	printf("\n\n");
	printf("==================\n");
	printf("   After Sorting  \n");
	printf("==================\n");
	// Execute sorting.
	quicksort(list, low, high);
	// Print sorted strings in the array 'list'.	


	for(j = 0 ; j < idx ; j++)
	{
		printf("%s", list[j]);
	}
	// Print strings of array 'list' to 'OutputList.txt'
	for(k = 0 ; k < idx ; k++)
	{
		fputs(list[k], fp_output);
	}
	fclose(fp_output);	// close file stream.

	printf("==================\n");
	return 0;
}
