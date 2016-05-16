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


/*finds and returns the small sub-tree in the forrest*/


/*builds the huffman tree and returns its address by reference*/

void get_size(FILE *input){
    char bit, c, x = 0;
    int n,length,bitsLeft = 8;
    int originalBits = 0, compressedBits = 0;
    file_size_counter = 0;
    //fill the frequency table first

    while ((c=fgetc(input))!= EOF){
        //printf("the c we got is%d\n",c);

        file_size_counter++;


    }
    return;
}




/*function to compress the input*/
void compress_file_modified(FILE *input, FILE *output){
    char bit, x = 0;
    int c;
    int n,length,bitsLeft = 8;
    int iter = 0, compressedBits = 0;

    file_size_counter++;

    int *input_data = malloc(file_size_counter * sizeof(int));
    int *output_data = malloc(2 * file_size_counter * sizeof(int));



    while ((c=fgetc(input))!= EOF){
        //printf("the c we got is%d\n",c);


        input_data[iter] = c;
        iter++;
    }

    input_data[iter] = 300;

    int i = 0;
    int iter2 = 0;
    int expand = 0;

    while (i<file_size_counter){
    int counter = 1;
    expand = 0;

        if(input_data[i]>127){
            expand = 1;
        }

        while (input_data[i] == input_data[i+1]){
            counter++;
            i++;
            expand = 1;
        }
        while (counter > 127){
            output_data[iter2] = 255;
            fputc(255,output);
            iter2++;
            output_data[iter2] = input_data[i];
            fputc(input_data[i],output);
            iter2++;
            counter = counter - 128;
        }

        if(expand == 1){
            output_data[iter2] = counter+128;
            fputc(counter+128,output);

            iter2++;
            output_data[iter2] = input_data[i];
            fputc(input_data[i],output);
            iter2++;
            i++;
        }
        else{
            output_data[iter2] = input_data[i];
            fputc(input_data[i],output);
            iter2++;
            i++;
        }
    }





   

    /*print details of compression on the screen*/
    fprintf(stderr,"Original bits = %d\n",(file_size_counter-1)*8);
    fprintf(stderr,"Compressed bits = %d\n",(iter2-1)*8);
    fprintf(stderr,"Saved %.2f%% of memory\n",((float)(iter2-1)/(file_size_counter-1))*100);

    return;
}

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

       get_size(temp_input);
       printf("size%d\n", file_size_counter);


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

        
        
        compress_file_modified(temp_input2,output);


    }
    else
    {
        get_size(input);
        decompressFile(input,output);
        printf("success2!\n");
    }

    return 0;
}
