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


int file_size_counter = 0;


int byte_table_frequencies[256];

char *buffer;
char *bi_array;
char *out_array;
    unsigned long fileLen;


/*finds and returns the small sub-tree in the forrest*/


/*builds the huffman tree and returns its address by reference*/



/*function to decompress the input*/
void decompressFile (FILE *input, FILE *output){
    char bit, c, x = 0;
    int *deco_data = malloc(file_size_counter * sizeof(int));
    
   int j=0;
   while ((c=fgetc(input))!=EOF){
        deco_data[j] = c;
        j++;

   }

   for(int a=0; a<j;a=a+2){
     while(deco_data[a]>1){
        fputc(deco_data[a+1], output);
        deco_data[a]--;
     }
   }

    return;
}

const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}





int main(){
    Node *tree;
    int codeTable[256], codeTable2[256];
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

    input = fopen(filename, "rb");
    output = fopen("output.txt","w");

    if (compress==1)
    {

        fseek(input, 0, SEEK_END);
        fileLen=ftell(input);
        fseek(input, 0, SEEK_SET);

        buffer=(char *)malloc(fileLen+1);
        bi_array = (char *)malloc((fileLen+1)*8);
        out_array = (char *)malloc((fileLen+1)*8);
    if (!buffer)
    {
        fprintf(stderr, "Memory error!");
                                fclose(input);
        return 1;
    }

    //Read file contents into buffer
    fread(buffer, fileLen, 1, input);
    fclose(input);



    for (long int i=0;i<fileLen;i++){

        bi_array[i] = byte_to_binary(buffer[i])[0];
        bi_array[i+1] = byte_to_binary(buffer[i])[1];
        bi_array[i+2] = byte_to_binary(buffer[i])[2];
        bi_array[i+3] = byte_to_binary(buffer[i])[3];
        bi_array[i+4] = byte_to_binary(buffer[i])[4];
        bi_array[i+5] = byte_to_binary(buffer[i])[5];
        bi_array[i+6] = byte_to_binary(buffer[i])[6];
        bi_array[i+7] = byte_to_binary(buffer[i])[7];

        //printf("%c\n", bi_array[i]);
    }


    int counter=0;
    int out_iter=0;
    int compressedBits =0;

    for (int i=0;i<fileLen*8;i++){
        //printf("%c\n", bi_array[i]);
        if(bi_array[i]==0){
            counter++;
            //printf("here?\n");
        }
        else{
            //printf("mustbehere\n");
            compressedBits = compressedBits+6;
            out_array[out_iter]=0;
            out_iter++;
            //printf("checkpoint\n");
            out_array[out_iter]=byte_to_binary(counter)[3];//start with 3 since i only need last 5 bits
            out_iter++;
            out_array[out_iter]=byte_to_binary(counter)[4];
            out_iter++;
            out_array[out_iter]=byte_to_binary(counter)[5];
            out_iter++;
            out_array[out_iter]=byte_to_binary(counter)[6];
            out_iter++;
            out_array[out_iter]=byte_to_binary(counter)[7];
            out_iter++;
            counter=0;
        }
        if(counter == 32){
            counter = 0;
            out_array[out_iter]=1;
            out_iter++;
            compressedBits++;
        }

    }

    fprintf(stderr,"Original bits = %lu\n",(fileLen)*8);
    fprintf(stderr,"Compressed bits = %d\n",(compressedBits));
    fprintf(stderr,"Saved %.2f%% of memory\n",((float)compressedBits/(fileLen*8))*100);


        //compressFile(input,output,codeTable2);
        //input_bit_file = OpenInputBitFile(filename);
        //printf("success!\n");
        //unsigned char temp_mask = input_bit_file->mask;
       // printf("%u\n", (unsigned int)temp_mask);
        //FILE * temp_input = input_bit_file->file;




       //get_size(temp_input);
       //printf("size%d\n", file_size_counter);


        /*
        char c;
        while ((c=fgetc(temp_input))!= EOF)
        {
        printf("the c we got is%d\n",c);
        }
        */
        //input_bit_file2 = OpenInputBitFile(filename);
        //FILE * temp_input2 = input_bit_file2->file;
        //char c;

        
        
        //compress_file_basic(temp_input2,output);


    }
    else
    {
        
        decompressFile(input,output);
        printf("success2!\n");
    }

    return 0;
}
