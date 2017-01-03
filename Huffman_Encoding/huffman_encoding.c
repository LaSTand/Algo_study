/* 
 * Huffman Encondig
 * Wild character(unicode) encoding 
 */
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

#define CODE_SIZE 32
#define RIGHT 'R'
#define LEFT 'L'
#define MAXNUM 1500
#define INPUTFILE L"Input_ex_Korean.txt"			// Input File name 
#define HUFF_TABLE L"Huffman_Table.txt"				// Huffman table file
#define OUTPUTFILE L"Output_ex_Korean.txt"			// Encoded text(not binary) file
#define OUTPUTFILE2 L"Decoded_ex_Korean.txt"		// Decoded text file
//#define INPUTFILE L"test.txt" // Å×½ºÆ®..

typedef struct _node {
	int freq;					// Frequency
	wchar_t c;					// a character
	wchar_t hcode[CODE_SIZE];	// Huffman code
	struct _node * left;		// Left leaf
	struct _node * right;		// Right leaf
} Node;

Node * arr[MAXNUM];		// Stored origin letters
Node * letters[MAXNUM];	// Array for backup
int idx = 0;			// The number of all letters

/* 
 Encode function 
 Calculate the Huffan code
 */
void encode(Node * node, Node ** letters, char direction, size_t level, wchar_t * code) {
	size_t n;
	
	n = wcslen(code);
	if (node != NULL) {
		if ( n  < level) {
			if (direction == RIGHT)
				wcscat(code, L"1");
			else {
				if (direction == LEFT)
					wcscat(code, L"0");
			}
		}
		else {
			if (n >= level && level != 0) {		// level!=0 (cause.. negative value)
				code[n - (n - level) - 1] = 0;
				if (direction == RIGHT)
					wcscat(code, L"1");
				else {
					if (direction == LEFT)
						wcscat(code, L"0");
				}
			}
		}
		// Save generated code
		if (node->c != NULL) {
			wcscpy(node->hcode, code);
			for (int i = 0; i < idx; i++) {
				if (node->c == letters[i]->c){
					wcscpy(letters[i]->hcode, code);	// the 'arr' will be break.. so backup the tree to 'letters'
					break;
				}
			}
		}
		else wprintf(L"%c", node->c);		// NULL character print..
		encode(node->left, letters, LEFT, level + 1, code);
		encode(node->right, letters, RIGHT, level + 1, code);
	}
}
/*
 Print Huffman Table
 Frequecy, huffman code will be saved in a file
 */
void printTree(Node * node, FILE * OUT) {
	if (node != NULL) {
		if (node->c != NULL)	// The node is not leaf
			fwprintf(OUT, L"%c - frequency: %3d\tencoding: %s\n", node->c, node->freq, node->hcode);
		else
			wprintf(L"%c", node->c);	// Non-leaf node has no character(Null character)
		printTree(node->left, OUT);		// Recursive
		printTree(node->right, OUT);
	}
}
/*
 Check all node's frequency
 Find the smallest 2 node
 Before making huffman tree
 */
void findMinAndSecondMin(Node * arr[], int * min, int * minIndex, int * secondMin, int * secondMinIndex) {
	int i, k;

	k = 0;
	*minIndex = -1;

	while (k < idx && arr[k] == NULL) k++;

	*minIndex = k;
	*min = arr[k]->freq;

	for (i = k; i < idx; i++) {
		if (arr[i] != NULL && arr[i]->freq < *min) {
			*min = arr[i]->freq;
			*minIndex = i;
		}
	}

	k = 0;	// reset 
	*secondMinIndex = -1;

	while ((k < idx && arr[k] == NULL) || (k == *minIndex && arr[k] != NULL)) k++;

	*secondMinIndex = k;
	*secondMin = arr[k]->freq;

	if (k == *minIndex) k++;

	for (i = k; i < idx; i++) {
		if (arr[i] != NULL && arr[i]->freq < *secondMin && i != *minIndex) {
			*secondMin = arr[i]->freq;
			*secondMinIndex = i;
		}
	}
}
/*
 Decode the encoded file
 Origin text file will be restored
 */
void decode(Node * tree, FILE * IN, FILE * OUT) {
	wchar_t ch = NULL;
	if (tree->c == NULL) {	// not leaf
		ch = fgetwc(IN);
		if (ch == L'0') {
			decode(tree->left, IN, OUT);
		}
		else if (ch == L'1') {
			decode(tree->right, IN, OUT);
		}
	}
	else if (tree->c != NULL) {		// leaf
		fwprintf(OUT, L"%c", tree->c);
		return;
	}
	else
		puts(NULL);	// NOP
}

int main()
{
	_wsetlocale(LC_ALL, L"korean");      // Set locale..

	wchar_t str[32];
	wchar_t code_arr[32];
	wint_t fileReadState;
	int i, j;
	int flag;	// flag to find same character in the input file
	int numOfChars = 0;
	int min, min2;
	int minIdx, min2Idx;
	float aft, bef;
	Node * tree;
	size_t n, m;
	FILE * IN;
	FILE * OUT;

	if ((IN = _wfopen(INPUTFILE, L"r,ccs=UNICODE")) == NULL)
	{
		wprintf(L"_wfopen Failed1\n");
		return 0;
	}
	else {
		numOfChars = 0;
		fileReadState = fgetws(str, CODE_SIZE, IN) != NULL;
		while (!feof(IN) || fileReadState) {
			n = wcslen(str);
			//wprintf(L"%s", str);
			for (i = 0; i < n; i++) {
				arr[idx] = NULL;
				//
				flag = 0;
				numOfChars++;
				for (j = 0; j < idx; j++) {
					if (arr[j]->c == str[i]) {
						flag = 1;	// If same character already exist
						break;
					}
					else
						flag = 0;	// First appear
				}
				/* store the characterr into arr */
				if (arr[idx] == NULL && flag == 0) {
					arr[idx] = (Node *)malloc(sizeof(Node));
					arr[idx]->c = str[i];
					arr[idx]->freq = 1;
					arr[idx]->left = NULL;
					arr[idx]->right = NULL;
					idx++;
				}
				else
					arr[j]->freq += 1;		// Already exist character.. freq++
			}
			if (fileReadState) {
				fileReadState = fgetws(str, CODE_SIZE, IN) != NULL;
			}
		}
	}
	fclose(IN);

	// Huffman table backup
	for (i = 0; i < idx; i++) {
		letters[i] = arr[i];
	}

	// Make huffman tree
	j = 1;
	do {
		findMinAndSecondMin(arr, &min, &minIdx, &min2, &min2Idx);	// Find the smallest 2 nodes

		if (minIdx != -1 && min2Idx != -1 && minIdx != min2Idx) {
			Node *temp;
			tree = (Node *)malloc(sizeof(Node));					// Make a node
			tree->freq = arr[minIdx]->freq + arr[min2Idx]->freq;	// Sum the frequency of 2 nodes
			tree->c = NULL;											// Not leaf
			tree->left = arr[minIdx];								// Left side 
			temp = (Node *)malloc(sizeof(Node));					// Make temp node 
			temp->c = arr[min2Idx]->c;								// 2nd node backup
			temp->freq = arr[min2Idx]->freq;
			temp->left = arr[min2Idx]->left;
			temp->right = arr[min2Idx]->right; 
			tree->right = temp;										 

			arr[minIdx] = tree;
			arr[min2Idx] = NULL;			// tree will be broken.						
		}
		j++;
	} while (j < idx);

	OUT = _wfopen(HUFF_TABLE, L"w,ccs=UNICODE");	
	fwprintf(OUT, L"Number of Letters %d\n", idx);
	// Encode value save
	for (i = 0; i < idx; i++) {
		if (arr[i] != NULL) {
			wcscat(code_arr, L"");	// Initialize with NULL
			encode(tree = arr[i], letters, 0, 0, code_arr);	// Encode result --> hcode
			printf("\nSuccessful encoding");
			printTree(arr[i], OUT);
			break;
		}
	}
	fclose(OUT);

	//Encode file
	IN = _wfopen(INPUTFILE, L"r,ccs=UNICODE");
	OUT = _wfopen(OUTPUTFILE, L"w,ccs=UNICODE");
	fileReadState = fgetws(str, CODE_SIZE, IN) != NULL;
	while (!feof(IN) || fileReadState) {
		n = wcslen(str);
		for (i = 0; i < n; i++) {
			// Check the Same char
			for (j = 0; j < idx; j++) {
				if (letters[j]->c == str[i]) {
					fputws(letters[j]->hcode, OUT);
					break;
				}
			}
		}
		if (fileReadState) {
			fileReadState = fgetws(str, CODE_SIZE, IN) != NULL;
		}
	}
	fclose(IN);
	fclose(OUT);

	wprintf(L"\n Input File : %s\n", INPUTFILE);
	wprintf(L"\n Output File : %s\n", OUTPUTFILE);

	printf("\n Input file size(letter : 2 byte) : %4d bits", numOfChars*16);	// a letter 16bit
	bef = numOfChars * 16;

	OUT = _wfopen(OUTPUTFILE, L"r,ccs=UNICODE");
	fileReadState = fgetws(str, CODE_SIZE, OUT) != NULL;
	numOfChars = 0;
	while (!feof(OUT) || fileReadState) {
		numOfChars += wcslen(str);					// Calc the number of 1 and 0
		if (fileReadState) {
			fileReadState = fgetws(str, CODE_SIZE, OUT) != NULL;
		}
	}
	fclose(OUT);

	aft = numOfChars;
	printf("\n Compressed file size(0 and 1 : 1bit) %4d bits", numOfChars);	// Not binary(check result easily in text mode)
	
	printf("\n Compressed Rate : %f\n", aft/bef*100);

	printf("\n\n==== decodeing output file ====\n");

	IN = _wfopen(OUTPUTFILE, L"r,ccs=UNICODE");		// L"test.txt" OUTPUTFILE
	OUT = _wfopen(OUTPUTFILE2, L"w,ccs=UNICODE");
	// Decode
	while (!feof(IN)) {
		decode(tree, IN, OUT);
	}
	fclose(IN);
	fclose(OUT);
	printf("decodeing Succeed!!\n\n");

	return 0;
}