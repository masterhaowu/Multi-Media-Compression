/****************************************************************************/
/*                A simple implementation of the Huffman coding             */
/*                author: danielscocco@gmail.com                            */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bitio.h"


#define len(x) ((int)log10(x)+1)

uint64_t test;

/* Node of the huffman tree */
struct node{
    int value;
    int letter;
    struct node *left,*right;
};

typedef struct node Node;

BIT_FILE * input_bit_file;
BIT_FILE * input_bit_file2;

/* 81 = 8.1%, 128 = 12.8% and so on. The 27th frequency is the space. Source is Wikipedia */
int englishLetterFrequencies [27] = {81, 15, 28, 43, 128, 23, 20, 61, 71, 2, 1, 40, 24, 69, 76, 20, 1, 61, 64, 91, 28, 10, 24, 1, 20, 1, 130};

int file_size_counter = 0;


int byte_table_frequencies[256];
/*finds and returns the small sub-tree in the forrest*/
int findSmaller (Node *array[], int differentFrom){
    int smaller;
    int i = 0;

    while (array[i]->value==-1)
        i++;
    smaller=i;
    if (i==differentFrom){
        i++;
        while (array[i]->value==-1)
            i++;
        smaller=i;
    }

    for (i=1;i<256;i++){
        if (array[i]->value==-1)
            continue;
        if (i==differentFrom)
            continue;
        if (array[i]->value<array[smaller]->value)
            smaller = i;
    }

    return smaller;
}

/*builds the huffman tree and returns its address by reference*/
void buildHuffmanTree(Node **tree){
    Node *temp;
    Node *array[27];
    int i, subTrees = 27;
    int smallOne,smallTwo;

    for (i=0;i<27;i++){
        array[i] = malloc(sizeof(Node));
        array[i]->value = englishLetterFrequencies[i];
        array[i]->letter = i;
        array[i]->left = NULL;
        array[i]->right = NULL;
    }

    while (subTrees>1){
        smallOne=findSmaller(array,-1);
        smallTwo=findSmaller(array,smallOne);
        temp = array[smallOne];
        array[smallOne] = malloc(sizeof(Node));
        array[smallOne]->value=temp->value+array[smallTwo]->value;
        array[smallOne]->letter=127;
        array[smallOne]->left=array[smallTwo];
        array[smallOne]->right=temp;
        array[smallTwo]->value=-1;
        subTrees--;
    }

    *tree = array[smallOne];

return;
}

void clear_byte_table_frequency() 
{
    for (int i=0;i<256;i++)
    {
        byte_table_frequencies[i] = 0;
    }

}

void build_huffman_table_bytes_global(Node **tree, FILE *input)
{

    clear_byte_table_frequency();
    int bit, c, x = 0;
    int n,length,bitsLeft = 8;
    int originalBits = 0, compressedBits = 0;

    //fill the frequency table first

    while ((c=fgetc(input))!= EOF){
        //printf("the c we got is%d\n",c);

        file_size_counter++;
        
        byte_table_frequencies[c]++;

    }
    printf("filling success!\n");
    printf("random number is %d\n", byte_table_frequencies[200]);
    printf("total number is %d\n", file_size_counter);



    //now build the table
    Node *temp;
    Node *array[256];
    int i, subTrees = 256;
    int smallOne,smallTwo;

    for (i=0;i<256;i++){
        
        
        array[i] = malloc(sizeof(Node));
        array[i]->value = byte_table_frequencies[i];
        if(byte_table_frequencies[i]==0){
            //printf("000000000000\n");
            subTrees--;
            array[i]->value = -1;
        }

        array[i]->letter = i;
        array[i]->left = NULL;
        array[i]->right = NULL;
        
    }

    printf("filled all the nodes in the array!\n");

     while (subTrees>1){
        smallOne=findSmaller(array,-1);
        smallTwo=findSmaller(array,smallOne);
        temp = array[smallOne];
        array[smallOne] = malloc(sizeof(Node));
        array[smallOne]->value=temp->value+array[smallTwo]->value;
        array[smallOne]->letter=356;
        array[smallOne]->left=array[smallTwo];
        array[smallOne]->right=temp;
        array[smallTwo]->value=-1;
        subTrees--;
    }

    *tree = array[smallOne];

    printf("tree building success!\n");


    return;

}

/* builds the table with the bits for each letter. 1 stands for binary 0 and 2 for binary 1 (used to facilitate arithmetic)*/
void fillTable(uint64_t codeTable[], Node *tree, uint64_t Code){
            
    //printf("here?\n");
    if (tree->letter<256){
        //printf("go into the tree!\n");
        //printf("code assigned%llu\n", Code);
        codeTable[(int)tree->letter] = Code;
    }
    else{
        fillTable(codeTable, tree->left, Code*10+1);
        fillTable(codeTable, tree->right, Code*10+2);
    }
   // printf("return\n");
    return;
}

/*function to compress the input*/
void compress_file_huffman_global(FILE *input, FILE *output, uint64_t codeTable[]){
    int bit, c, x = 0;
    int n,length,bitsLeft = 8;
    int originalBits = 0, compressedBits = 0;
    double temp_length;
    double total_length;
    for (int i=0;i<256;i++){

        temp_length=len(codeTable[i]);
        total_length=temp_length*byte_table_frequencies[i] + total_length;

    }
    total_length = total_length/file_size_counter;

    while ((c=fgetc(input))!= EOF){
        //printf("the c we got is%d\n",c);
        originalBits++;
        
            length=len(codeTable[c]);
            n = codeTable[c];
        

        while (length>0){
            compressedBits++;
            bit = n % 10 - 1;
            n /= 10;
            x = x | bit;
            bitsLeft--;
            length--;
            if (bitsLeft==0){
                fputc(x,output);
                x = 0;
                bitsLeft = 8;
            }
            x = x << 1;
        }
    }

    if (bitsLeft!=8){
        x = x << (bitsLeft-1);
        fputc(x,output);
    }

    int total_len=0;

  

    double temp_double = (double)compressedBits/originalBits;

    /*print details of compression on the screen*/
    fprintf(stderr,"Average bits = %f\n",total_length);
    fprintf(stderr,"Original bits = %d\n",originalBits*8);
    fprintf(stderr,"Compressed bits = %d\n",compressedBits);
    fprintf(stderr,"Saved %.2f%% of memory\n",((float)compressedBits/(originalBits*8))*100);

    return;
}

/*function to decompress the input*/
void decompressFile (FILE *input, FILE *output, Node *tree){
    Node *current = tree;
    int c,bit;
    char mask = 1 << 7;
    int i;

    while ((c=fgetc(input))!=EOF){

        for (i=0;i<8;i++){
            bit = c & mask;
            c = c << 1;
            if (bit==0){
                current = current->left;
                if (current->letter!=127){
                    if (current->letter==26)
                        fputc(32, output);
                    else
                        fputc(current->letter+97,output);
                    current = tree;
                }
            }

            else{
                current = current->right;
                if (current->letter!=127){
                    if (current->letter==26)
                        fputc(32, output);
                    else
                        fputc(current->letter+97,output);
                    current = tree;
                }
            }
        }
    }

    return;
}

/*invert the codes in codeTable2 so they can be used with mod operator by compressFile function*/
void invertCodes(uint64_t codeTable[],uint64_t codeTable2[]){
    int i;
    uint64_t n, copy;

    for (i=0;i<256;i++){
        n = codeTable[i];
        copy = 0;
        while (n>0){
            copy = copy * 10 + n %10;
            n /= 10;
        }
        codeTable2[i]=copy;
    }

return;
}
/*
void compress_byte_file(FILE *input, FILE *output, int codeTable[]){
    char bit, c, x = 0;
    int n,length,bitsLeft = 8;
    int originalBits = 0, compressedBits = 0;

    while ((c=fgetc(input))!= EOF){
        printf("the c we got is%d\n",c);
        originalBits++;
        if (c==32){
            length = len(codeTable[26]);
            n = codeTable[26];
        }
        else{
            length=len(codeTable[c-97]);
            n = codeTable[c-97];
        }

        while (length>0){
            compressedBits++;
            bit = n % 10 - 1;
            n /= 10;
            x = x | bit;
            bitsLeft--;
            length--;
            if (bitsLeft==0){
                fputc(x,output);
                x = 0;
                bitsLeft = 8;
            }
            x = x << 1;
        }
    }

    if (bitsLeft!=8){
        x = x << (bitsLeft-1);
        fputc(x,output);
    }

    print details of compression on the screen
    fprintf(stderr,"Original bits = %d\n",originalBits*8);
    fprintf(stderr,"Compressed bits = %d\n",compressedBits);
    fprintf(stderr,"Saved %.2f%% of memory\n",((float)compressedBits/(originalBits*8))*100);

    return;
}
*/

int main(){
    Node *tree;
    uint64_t codeTable[256], codeTable2[256];

    for (int i=0;i<256;i++){
        codeTable[i] = 0;
        codeTable2[i] = 0;
    }
    int compress;
    char filename[20];
    FILE *input, *output;

    test=1;

    //buildHuffmanTree(&tree);

    //fillTable(codeTable, tree, 0);

    //invertCodes(codeTable,codeTable2);

    /*get input details from user*/
    printf("Type the name of the file to process:\n");
    scanf("%s",filename);
    printf("Type 1 to compress and 2 to decompress:\n");
    scanf("%d",&compress);

    input = fopen(filename, "r");
    output = fopen("output.txt","w");

    if (compress==1)
    {
        //compressFile(input,output,codeTable2);
        input_bit_file = OpenInputBitFile(filename);
        printf("success!\n");
        unsigned char temp_mask = input_bit_file->mask;
        printf("%u\n", (unsigned int)temp_mask);
        FILE * temp_input = input_bit_file->file;

        build_huffman_table_bytes_global(&tree, temp_input);

        fillTable(codeTable, tree, 0);

        invertCodes(codeTable,codeTable2);
        /*
        char c;
        while ((c=fgetc(temp_input))!= EOF)
        {
        printf("the c we got is%d\n",c);
        }
        */
        input_bit_file2 = OpenInputBitFile(filename);
        FILE * temp_input2 = input_bit_file2->file;
        char c;

       // printf("0checking%d\n", codeTable2[0]);
        //printf("1checking%d\n", codeTable2[1]);
        //printf("2checking%d\n", codeTable2[2]);
        
        compress_file_huffman_global(temp_input2,output,codeTable2);
        //printf("testing%llu\n", test);

    }
    else
    {
        decompressFile(input,output, tree);
        printf("success2!\n");
    }

    return 0;
}
