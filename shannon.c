/****************************************************************************/
/*                A simple implementation of the Huffman coding             */
/*                author: danielscocco@gmail.com                            */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "bitio.h"


#define len(x) ((int)log10(x)+1)

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

int first_size = 256;

int byte_table_frequencies[256];
int sorted_array[256];
double entropy = 0;
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

int findBigger (Node *array[], int differentFrom){
    int bigger;
    int i = 0;

    while (array[i]->value==-1)
        i++;
    bigger=i;
    if (i==differentFrom){
        i++;
        while (array[i]->value==-1)
            i++;
        bigger=i;
    }

    for (i=1;i<256;i++){
        if (array[i]->value==-1)
            continue;
        if (i==differentFrom)
            continue;
        if (array[i]->value>array[bigger]->value)
            bigger = i;
    }

    return bigger;
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
        sorted_array[i] = i;
    }

}

void build_shannon_table_bytes_global(Node **tree, FILE *input)
{

    clear_byte_table_frequency();
    int bit, c, x = 0;
    int n,length,bitsLeft = 8;
    int originalBits = 0, compressedBits = 0;
    file_size_counter = 0;
    //fill the frequency table first

    while ((c=fgetc(input))!= EOF){
        //printf("the c we got is%d\n",c);

        file_size_counter++;
        
        byte_table_frequencies[c]++;

    }
    //printf("filling success!\n");
    //printf("random number is %d\n", byte_table_frequencies[200]);
    //printf("total number is %d\n", file_size_counter);


    //calculate entropy
  double temp_prob = 0;
    for (int i = 0; i < 256; ++i)
    {
        //printf("byte_table_frequencies%d\n", byte_table_frequencies[i]);
        
        temp_prob = (double)byte_table_frequencies[i]/(double)file_size_counter;
        if(temp_prob>0){
            entropy = entropy - temp_prob*(log10(temp_prob)/log10(2));
            //printf(" test%f\n", entropy);
        }
    }

    printf("the entropy is: %f\n", entropy);

    //now build the table
    

    //first sort the array in descending order
    int temp_a;
    for (int i = 0; i < 256; ++i)
    {
        for (int j = i + 1; j < 256; ++j)
        {
            if (byte_table_frequencies[i] < byte_table_frequencies[j])
            {
                temp_a =  sorted_array[i];
                sorted_array[i] = sorted_array[j];
                sorted_array[j] = temp_a;
            }
        }
    }

    for (int i = 0; i < 256; ++i) {
        if (byte_table_frequencies[i]==0){
            first_size--;
        }
    }


    /*
    for (int i=0;i<20;i++)
    {
        printf("sorted_array results'%d'\n", sorted_array[i]);
    }
    */




    return;

}


void divide_fill(uint64_t codeTable[], int sorting_array[], int size, uint64_t Code){

    
   if(size == 1){
        codeTable[sorting_array[0]] = Code;
        //printf("code assigned%llu\n", Code);
   }

   else
   {


    int goal = 0;
    for (int i=0;i<size;i++){
        goal = goal + byte_table_frequencies[sorting_array[i]];
    }

    goal = goal/2;
    int min_diff = goal;
    int target_loc = 0;
    int diff = 0;
    int total = 0;

    for (int i=0;i<size-1;i++){
        total = total + byte_table_frequencies[sorting_array[i]];
        diff = abs(total - goal);
        if(diff<min_diff){
            min_diff = diff;
            target_loc = i;
        }
    }

    int size1 = target_loc + 1;
    int size2 = size - size1;

    int *firstHalf = malloc(size1 * sizeof(int));

    int *secondHalf = malloc(size2 * sizeof(int));

    memcpy(firstHalf, sorting_array, size1 * sizeof(int));
    memcpy(secondHalf, sorting_array + size1, size2 * sizeof(int));

    divide_fill(codeTable, firstHalf, size1, Code*10+1);
    divide_fill(codeTable, secondHalf, size2, Code*10+2);



    }


    return;
}

/* builds the table with the bits for each letter. 1 stands for binary 0 and 2 for binary 1 (used to facilitate arithmetic)*/

/*function to compress the input*/
void compress_file_shannon_global(FILE *input, FILE *output, uint64_t codeTable[]){
    int bit, c, x = 0;
    int length,bitsLeft = 8;
    uint64_t n;
    int originalBits = 0, compressedBits = 0;

    double temp_length;
    double total_length;
    for (int i=0;i<256;i++){

        temp_length=len(codeTable[i]);
        //printf("the temp_length we got is%f\n",temp_length);
        if(temp_length>0){
        total_length=temp_length*byte_table_frequencies[i] + total_length;
        }

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
    double temp_double = ((double)compressedBits)/((double)originalBits);

    /*print details of compression on the screen*/
    printf("Average bits = %f\n",total_length);
    /*print details of compression on the screen*/
    fprintf(stderr,"Original bits = %d\n",originalBits*8);
    fprintf(stderr,"Compressed bits = %d\n",compressedBits);
    fprintf(stderr,"Saved %.2f%% of memory\n",((float)compressedBits/(originalBits*8))*100);

    return;
}

/*function to decompress the input*/
void decompressFile (FILE *input, FILE *output, Node *tree){
    Node *current = tree;
    char c,bit;
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
    uint64_t i, n, copy;

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



int main(){
    Node *tree;
    uint64_t codeTable[256], codeTable2[256];
    int compress;
    char filename[20];
    FILE *input, *output;

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

        build_shannon_table_bytes_global(&tree, temp_input);

        //fillTable(codeTable, tree, 0);

        divide_fill(codeTable, sorted_array, first_size, 0);

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

        
        
        compress_file_shannon_global(temp_input2,output,codeTable2);


    }
    else
    {
        decompressFile(input,output, tree);
        printf("success2!\n");
    }

    return 0;
}
