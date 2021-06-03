/*
 @author: Zuhaib Ul Zamann
 Entry Number: 2018MT60798
 Department of Mathematics and Computing, IITD
 Date: 14/04/2021
*/
#include<stdlib.h>
#include<stdio.h>
#include <string.h>
#include<limits.h>
#define DELIM " \n"// Delimiter for tokenization
#define VPN_MASK 0xfffff000// VPN Mask for extracting Virtual page number.
#define OPTIMAL "OPT" // String OPT
#define RANDOM "RANDOM" // String RANDOM
#define CLOCK "CLOCK" // String CLOCK
#define FIRST_IN_FIRST_OUT "FIFO" // String FIFO
#define LEAST_RECENT_USED "LRU" // STRING LRU
//Adding Verbose thingy Now
// For a page table i need
// Frame Number corresponding to a page.
// Valid bit
// Dirty bit

int * FrameNumber;// Page Table
int *Valid;// Valid bit
int * Dirty; // Dirty Bit

void FIFO(char*file, int count, int flag)// First in first out page replacement policy
{
    // printf("%s\n", file);
    FILE * fpointer = fopen(file, "r");// File pointer
    if (fpointer == NULL)// File is not found
    {
        fprintf(stderr, "FILE NOT FOUND\n");
    }
    int frames[count];// Inverted page table
    memset(frames, - 1,sizeof(int)* count);// initially no frame is assigned  as page
    char comp[3];// Comparison purposes. To see if the operation was a read/ write
    strcpy(comp, "W");
    int frame_size =0; // Back of the queue. Insertion
    int number_of_reads = 0;// Total number of inputs
    int number_of_writes = 0;// Number of time evicted page is written on disk
    int number_of_faults = 0;// Total number of page faults
    int number_of_drops = 0;// Total number of evicted pages not written on disk
    char line[15];// Getting line by line input from file.
    while (!feof(fpointer))
    {
        fgets(line,15, fpointer);// Get the next Line
        char **tokens = (char**)malloc(sizeof(char*)* 2);// String Tokenization
        tokens[0] = strtok(line, DELIM);// Address
        tokens[1] = strtok(NULL, DELIM);// Read/Write
        if (!tokens[1])
        {
            continue;// Empty Line encountered or incorrect input format
        }
        long virtual_address = strtol(tokens[0], NULL, 0);// Virtual Memory Address
        int Page_Number = (int)(virtual_address&VPN_MASK)>>12;// Virtual Page Number;
        
        number_of_reads+=1;
        if (Valid[Page_Number] == 1)// The Page is in Physical Frame
        {
            if (strcmp(tokens[1], comp) == 0)// The command is of write
            {
                Dirty[Page_Number] = 1;// Page will now be dirty
            }
        }
        else// Page Fault
        {
            number_of_faults+=1;// Page fault occured
            if (frames[frame_size] == -1)// The frame has not yet been assigned to any page
            {
                // Cold start
                // No page eviction will be done
                frames[frame_size] = Page_Number;// Assign the frame to the page
                frame_size++; // Increase the current position of tail
                frame_size %= count;// In case queue has got full or one complete rotation.
                Valid[Page_Number] = 1; // Set the valid bit
                if (strcmp(tokens[1],comp) ==0)
                {// Command was W
                    Dirty[Page_Number] = 1;// Set Dirty bit to 1
                }
            }
            else
            {
                int page_evic = frames[frame_size];//This page will be evicted -> If dirty bit then writes will increase by 1
                frames[frame_size] = Page_Number;// This page is inserted
                frame_size++;// Explained as above.
                frame_size %= count;// Explained above
                if (Dirty[page_evic] == 1)// Page was dirty. Needed to be written back to the disk.
                {
                    if (flag == 1)// Verbose
                    {
                        printf("Page 0x%05x was read from disk, page 0x%05x was written to the disk.\n",Page_Number, page_evic );
                    }
                    number_of_writes +=1;// The page was dirty and hence its contents need to be written back.
                }
                else// Page is dropped
                {
                    if (flag == 1)// Verbose
                    {
                        printf("Page 0x%05x was read from disk, page 0x%05x was dropped (it was not dirty).\n",Page_Number, page_evic );
                    }
                    number_of_drops+=1;// Number of drops increases by 1
                }
                Dirty[page_evic] = 0;// Set the dirty bit of the evicted page to 0
                Valid[page_evic] = 0;// Set the valid bit of the evicted page to 0;
                Valid[Page_Number] = 1; // Set the valid bit of assigned page to 1
                if (strcmp(tokens[1],comp) == 0)// Command was W
                {
                    Dirty[Page_Number] = 1;// Set the dirty bit to 1.
                }
            }
        }
    }
    fclose(fpointer);// Close the file
    printf("Number of memory accesses: %d\n", number_of_reads);// Total number of memory accesses
    printf("Number of misses: %d\n", number_of_faults);// Total number of page faults
    printf("Number of writes: %d\n", number_of_writes);// Total number of writes
    printf("Number of drops: %d\n", number_of_drops);// Total number of drops
}

int LeftChild(int i)// Left child in heap. Heap starts at 0. So left child of 0 is 1 and right child is 2 and so on and so forth
{
    return  2*i+1;
}
int RightChild(int i)// Right Child in heap
{
    return 2*i+2;
}
int Parent(int i)// The parent of the current node
{
    return (i-1)/2;
}
void ShiftUp(int heap[], int Used_Time[], int heapIndex[], int count, int i)// Bubble up. Shift the current node upwards to its rightful position to restore heap property
{
    int p = Parent(i);// Parent of current node
    while (p>=0 && Used_Time[i]<Used_Time[p])// Node has a parent and its current value is smaller that its parent. This is a min heap so shift up will occur.
    {
        int temp = Used_Time[i];
        Used_Time[i] = Used_Time[p];
        Used_Time[p] = temp;
        temp = heap[i];
        heap[i] = heap[p];
        heap[p] = temp;
        heapIndex[heap[i]] = i;// Setting the selfawareness of nodes. In self awareness we make each node item aware of its index in heap. Hence the name heap index
        heapIndex[heap[p]] = p;
        i = p;
        p = Parent(p);
    }
}
void ShiftDown(int heap[], int Used_Time[], int heapIndex[],  int count, int i)// Bubbling Down
{
    int l = LeftChild(i);// The left child
    int r = RightChild(i);// The right child 
    int minIndex = i;// The minimum of the left and the right and the current node.
    if (l<count && Used_Time[minIndex]>Used_Time[l])
    {
        minIndex = l;
    }
    if (r<count && Used_Time[minIndex]>Used_Time[r])
    {
        minIndex = r;
    }
    if (minIndex!=i)
    {
        int temp = Used_Time[minIndex];// Current node was not minimum. Bubbling Down will occur
        Used_Time[minIndex] = Used_Time[i];
        Used_Time[i] = temp;
        temp = heap[minIndex];
        heap[minIndex] = heap[i];
        heap[i] = temp;
        heapIndex[heap[i]] = i;
        heapIndex[heap[minIndex]] = minIndex;
        ShiftDown(heap, Used_Time, heapIndex,count, minIndex);// Recursive call.
    }
}
void LRU(char *file , int count, int flag)// Least Recently used page replacement policy
{
    FILE * fpointer = fopen(file, "r");
    if (fpointer == NULL)
    {
        fprintf(stderr, "FILE NOT FOUND\n");
    }
    int frames[count];// Page NUmbers assigned to frame i;
    int heap[count];// heap. Min heap
    int Used_Time[count];// Last used time
    int heapIndex[count];// Heap Indices


    for(int i = 0;i<count;i++)
    {
        heap[i] = i;// Initially all frames have same recent used time. So order is immaterial
    }
    for(int i = 0;i<count;i++)
    {
        heapIndex[i] = i;
    }
   
    memset(frames, - 1,sizeof(int)* count);// All frames as empty
    memset(Used_Time, 0, sizeof(Used_Time)); // Last used time as 0. Time here will be command number.


    char comp[3];
    strcpy(comp, "W");


    int number_of_reads = 0;
    int number_of_writes = 0;
    int number_of_faults = 0;
    int number_of_drops = 0;
    int line_count = 0;
    char line[15];
    while (!feof(fpointer))
    {
        line_count++;
        fgets(line,15, fpointer);// Get the next Line
        char **tokens = (char**)malloc(sizeof(char*)* 2);// String Tokenization
        tokens[0] = strtok(line, DELIM);// Address
        tokens[1] = strtok(NULL, DELIM);// Read/Write
        if (!tokens[1])
        {
            continue;
        }
        long virtual_address = strtol(tokens[0], NULL, 0);// Virtual Memory Address
        int Page_Number = (int)(virtual_address&VPN_MASK)>>12;// Virtual Page Number;
        number_of_reads+=1;

        if (Valid[Page_Number] ==   1)// The Page is in Physical Frame
        {
            if (strcmp(tokens[1], comp) == 0)// The command is of write
            {
                Dirty[Page_Number] = 1;// Page will now be dirty
                // number_of_writes+=1;
            }
            int Frame_Number = FrameNumber[Page_Number];
            int index = heapIndex[Frame_Number];
            Used_Time[index] = line_count;
            ShiftDown(heap, Used_Time, heapIndex, count, index);
        }
        else
        {
            number_of_faults+=1;
            int index = heap[0];
            if (frames[index] == -1)
            {
                // Frame is empty
                frames[index] = Page_Number;
                FrameNumber[Page_Number] = index;
                Valid[Page_Number] = 1;
                if (strcmp(tokens[1], comp) == 0)// The command is of write
                {
                    Dirty[Page_Number] = 1;// Page will now be dirty
                    // number_of_writes+=1;
                }
                Used_Time[0] = line_count;
                ShiftDown(heap, Used_Time, heapIndex, count, 0);
            }
            else
            {
                Valid[frames[index]] = 0;
                if (Dirty[frames[index]]==1)
                {
                    if (flag == 1)
                    {
                        printf("Page 0x%05x was read from disk, page 0x%05x was written to the disk.\n",Page_Number, frames[index] );
                    }
                    number_of_writes+=1;
                    Dirty[frames[index]] = 0;
                }
                else
                {
                    if (flag == 1)
                    {
                        printf("Page 0x%05x was read from disk, page 0x%05x was dropped (it was not dirty).\n",Page_Number, frames[index] );
                    }

                    number_of_drops+=1;
                }
                frames[index] = Page_Number;
                FrameNumber[Page_Number] = index;
                Valid[Page_Number] = 1;
                if (strcmp(tokens[1], comp) == 0)// The command is of write
                {
                    Dirty[Page_Number] = 1;// Page will now be dirty
                    // number_of_writes+=1;
                }
                Used_Time[0] = line_count;
                ShiftDown(heap, Used_Time, heapIndex, count, 0);
            }
        }
    }
    fclose(fpointer);
    printf("Number of memory accesses: %d\n", number_of_reads);
    printf("Number of misses: %d\n", number_of_faults);
    printf("Number of writes: %d\n", number_of_writes);
    printf("Number of drops: %d\n", number_of_drops);

}
void Random(char *file, int count, int flag)
{
    // printf("COUNT %d\n", count);
    FILE * fpointer = fopen(file, "r");
    if (fpointer == NULL)
    {
        fprintf(stderr, "FILE NOT FOUND\n");
    }
    int frames[count];
    memset(frames, - 1,sizeof(int)* count);
    char comp[3];
    strcpy(comp, "W");
    char read[3];
    strcpy(read, "R");
    int number_of_reads = 0;
    int number_of_writes = 0;
    int number_of_faults = 0;
    int number_of_drops = 0;
    char line[15];
    int s=0;
    srand(5635);
    int complete = 0;
    while (!feof(fpointer)&& s<count)
    {
        fgets(line,15, fpointer);// Get the next Line
        char **tokens = (char**)malloc(sizeof(char*)* 2);// String Tokenization
        tokens[0] = strtok(line, DELIM);// Address
        tokens[1] = strtok(NULL, DELIM);// Read/Write
        if (!tokens[1])
        {
            complete = 1;
            continue;
        }
        number_of_reads+=1;
        long virtual_address = strtol(tokens[0], NULL, 0);// Virtual Memory Address
        int Page_Number = (int)(virtual_address&VPN_MASK)>>12;// Virtual Page Number;
        if (Valid[Page_Number]==1)
        {
            if (strcmp(comp, tokens[1])==0)
            {
                // number_of_writes+=1;
                Dirty[Page_Number] = 1;
            }

        }
        else
        {
            frames[s] = Page_Number;
            Valid[Page_Number] = 1;
            s++;
            if (strcmp(comp, tokens[1])==0)
            {
                // number_of_writes+=1;
                Dirty[Page_Number] = 1;
            }
            // number_of_reads+=1;
            number_of_faults+=1;
        }

    } 
    if (complete == 0)
    {
        // printf("YES\n");
        char line[15];
        while (!feof(fpointer))
        {
            fgets(line,15, fpointer);// Get the next Line
            // printf("%s\n",line);
            char **tokens = (char**)malloc(sizeof(char*)* 2);// String Tokenization
            tokens[0] = strtok(line, DELIM);// Address
            tokens[1] = strtok(NULL, DELIM);// Read/Write
            if (!tokens[1])
            {
                // printf("YES\n");
                continue;
            }
            // printf("%s,%s\n", tokens[0], tokens[1]);
            number_of_reads+=1;
            long virtual_address = strtol(tokens[0], NULL, 0);// Virtual Memory Address
            int Page_Number = (int)(virtual_address&VPN_MASK)>>12;// Virtual Page Number;
            if (Valid[Page_Number]==1)
            {
                if (strcmp(comp, tokens[1])==0)
                {
                    Dirty[Page_Number] = 1;
                }

            }
            else
            {
                number_of_faults+=1;
                int id = rand();
                // printf("%d\n",id);
                id %= count;
                // printf("%d\n",id);
                int page_evic = frames[id];
                if (Dirty[page_evic])
                {
                    if (flag == 1)
                    {
                        printf("Page 0x%05x was read from disk, page 0x%05x was written to the disk.\n",Page_Number, page_evic );
                    }
                    number_of_writes+=1;
                    Dirty[page_evic] = 0;
                }
                else
                {
                    if (flag == 1)
                    {
                        printf("Page 0x%05x was read from disk, page 0x%05x was dropped (it was not dirty).\n",Page_Number, page_evic );
                    }
                    number_of_drops+=1;
                }
                Valid[page_evic] = 0;
                Valid[Page_Number] = 1;
                frames[id] = Page_Number;
                if (strcmp(comp,tokens[1])==0)
                {
                    Dirty[Page_Number] = 1;
                }
            }
        }

    }
    fclose(fpointer);
    printf("Number of memory accesses: %d\n", number_of_reads);
    printf("Number of misses: %d\n", number_of_faults);
    printf("Number of writes: %d\n", number_of_writes);
    printf("Number of drops: %d\n", number_of_drops);
}
void Clock(char *file, int count, int flag)
{
    int frames[count];
    int Recent[count];
    memset(frames, -1, sizeof(frames));
    memset(Recent, 0, sizeof(Recent));
    FILE * fpointer = fopen(file, "r");
    if (!fpointer)
    {
        printf( "No such file in directory\n");
        return ;
    }
    char comp[3];
    strcpy(comp, "W");
    char read[3];
    strcpy(read, "R");
    int number_of_reads = 0;
    int number_of_writes = 0;
    int number_of_faults = 0;
    int number_of_drops = 0;
    int clkhand = 0;
    char line[15];
    while (!feof(fpointer))
    {
        fgets(line,15, fpointer);// Get the next Line
        char **tokens = (char**)malloc(sizeof(char*)* 2);// String Tokenization
        tokens[0] = strtok(line, DELIM);// Address
        tokens[1] = strtok(NULL, DELIM);// Read/Write
        if (!tokens[1])
        {
            continue;
        }
        long virtual_address = strtol(tokens[0], NULL, 0);// Virtual Memory Address
        int Page_Number = (int)(virtual_address&VPN_MASK)>>12;// Virtual Page Number;
        number_of_reads+=1;
        if (Valid[Page_Number] == 1)// The Page is in Physical Frame
        {
            if (strcmp(tokens[1], comp) == 0)// The command is of write
            {
                Dirty[Page_Number] = 1;// Page will now be dirty
            }
            int frame = FrameNumber[Page_Number];
            Recent[frame] =1;
        }
        else
        {
            number_of_faults+=1;
            while (Recent[clkhand]!=0)
            {
                Recent[clkhand]=0;
                clkhand++;
                clkhand%=count;
            }
            Recent[clkhand]=1;
            if (frames[clkhand]==-1)
            {
                frames[clkhand] = Page_Number;
                FrameNumber[Page_Number] = clkhand;
                Valid[Page_Number] = 1;
                if (strcmp(comp, tokens[1])==0)
                {
                    Dirty[Page_Number] = 1;
                }
                clkhand++;
                clkhand%=count;
            }
            else
            {
                int page_evic = frames[clkhand];
                FrameNumber[Page_Number] = clkhand;
                Valid[Page_Number] = 1;
                if (Dirty[page_evic]==1)
                {
                    if (flag == 1)
                    {
                        printf("Page 0x%05x was read from disk, page 0x%05x was written to the disk.\n",Page_Number, page_evic );
                    }
                    Dirty[page_evic] = 0;
                    number_of_writes+=1;
                }
                else
                {
                    if (flag == 1)
                    {
                        printf("Page 0x%05x was read from disk, page 0x%05x was dropped (it was not dirty).\n",Page_Number, page_evic );
                    }
                    number_of_drops+=1;
                }
                Valid[page_evic] =0;
                frames[clkhand] = Page_Number;
                if (strcmp(comp, tokens[1])==0)
                {
                    Dirty[Page_Number] = 1;
                }
                clkhand++;
                clkhand%=count;
            }
        }
    }
    fclose(fpointer);
    printf("Number of memory accesses: %d\n", number_of_reads);
    printf("Number of misses: %d\n", number_of_faults);
    printf("Number of writes: %d\n", number_of_writes);
    printf("Number of drops: %d\n", number_of_drops);
}
void OPT(char *file, int count, int flag)
{
    // printf("HOW are you\n");
    FILE * fpointer = fopen(file, "r");
    char comp[3];
    strcpy(comp, "W");
    char read[3];
    strcpy(read, "R");
    int number_of_reads = 0;
    int number_of_writes = 0;
    int number_of_faults = 0;
    int number_of_drops = 0;
    int frames[count];
    memset(frames, -1, sizeof(frames));
    int ** Queues = malloc(sizeof(int*)*(1<<20));//future
    int* sizes = (int*)malloc(sizeof(int)*(1<<20));//c
    int* maxSizes = (int*)malloc(sizeof(int)*(1<<20));//future size
    memset(maxSizes, 0, sizeof(int)*(1<<20));
    // printf("%d\n",maxSizes[0]);
    memset(sizes, 0, sizeof(int)*(1<<20));
    char line[15];
    int line_cnt = 0;
    while(!feof(fpointer))
    {
        line_cnt++;
        fgets(line,15, fpointer);// Get the next Line
        char **tokens = (char**)malloc(sizeof(char*)* 2);// String Tokenization
        tokens[0] = strtok(line, DELIM);// Address
        tokens[1] = strtok(NULL, DELIM);// Read/Write
        if (!tokens[1])
        {
            continue;
        }
        long virtual_address = strtol(tokens[0], NULL, 0);// Virtual Memory Address
        int Page_Number = (int)(virtual_address&VPN_MASK)>>12;// Virtual Page Number;
        if (Queues[Page_Number]==NULL)
        {
            // printf("ID is %d and count is %d and size is %d\n",Page_Number, line_cnt, sizes[Page_Number] );
            Queues[Page_Number] = malloc(sizeof(int)*10); 
            maxSizes[Page_Number] = 10;
        }
        else if ( sizes[Page_Number] == maxSizes[Page_Number])
        {
            if (maxSizes[Page_Number]==0)
            {
                printf("error\n");
                return;
            }
            Queues[Page_Number] = realloc(Queues[Page_Number],sizeof(int)*2*maxSizes[Page_Number]);
            maxSizes[Page_Number] = 2*maxSizes[Page_Number];
        }
        Queues[Page_Number][sizes[Page_Number]] = line_cnt;
        sizes[Page_Number]++;
    }
    fclose(fpointer);
    // for(int i = 0;i<1<<20;i++)
    // {
    //     if (Queues[i]!=NULL)
    //     {
    //         for(int j = 0;j<sizes[i];j++)
    //         {
    //             printf("%d, ", Queues[i][j]);
    //         }
    //         printf("\n");
    //     }
    // }
    int *new_sizes = malloc(sizeof(int)*(1<<20));
    memset(new_sizes, 0, sizeof(int)*(1<<20));
    // memset(sizes, 0 , sizeof(int)*(1<<20));
    fpointer = fopen(file, "r");
    int SetLimit = line_cnt++;
    line_cnt = 0;

    int s = 0;
    while (!feof(fpointer))
    {
        line_cnt++;
        fgets(line,15, fpointer);// Get the next Line
        char **tokens = (char**)malloc(sizeof(char*)* 2);// String Tokenization
        tokens[0] = strtok(line, DELIM);// Address
        tokens[1] = strtok(NULL, DELIM);// Read/Write
        if (!tokens[1])
        {
            continue;
        }
        number_of_reads+=1;
        long virtual_address = strtol(tokens[0], NULL, 0);// Virtual Memory Address
        int Page_Number = (int)(virtual_address&VPN_MASK)>>12;// Virtual Page Number;
        new_sizes[Page_Number]++;
        if(Valid[Page_Number]==1)
        {
            if (strcmp(comp, tokens[1])==0)
            {
                Dirty[Page_Number] = 1;
            }
        }
        else
        {
            number_of_faults+=1;
            if (s<count)
            {
                frames[s] = Page_Number;
                Valid[Page_Number] = 1;
                Dirty[Page_Number] = 0;
                if (strcmp(comp, tokens[1])==0)
                {
                    Dirty[Page_Number] = 1;
                }
                s++;
            }
            else
            {
                int bestIndex = 0;
                for(int i = 0;i<count;i++)
                {
                    int c1 = 0;
                    int c2 = 0;
                    int pg_n1 = frames[bestIndex];
                    int pg_n2 = frames[i];
                    if (new_sizes[pg_n1] ==sizes[pg_n1])
                    {
                        c1 = SetLimit;
                    }
                    else
                    {
                        c1 = Queues[pg_n1][new_sizes[pg_n1]];
                    }
                    if (new_sizes[pg_n2] ==sizes[pg_n2])
                    {
                        c2 = SetLimit;
                    }
                    else
                    {
                        c2 = Queues[pg_n2][new_sizes[pg_n2]];
                    }
                    if (c1<c2)
                    {
                        bestIndex = i;
                    }
                    // printf("%d,%d\n",c1, c2);
                }
                int page_evic = frames[bestIndex];
                Valid[page_evic] = 0;
                // printf("%d %d ",page_evic, Dirty[page_evic]);
                if (Dirty[page_evic] == 1)
                {
                    if (flag == 1)
                    {
                        printf("Page 0x%05x was read from disk, page 0x%05x was written to the disk.\n",Page_Number, page_evic );
                    }
                    Dirty[page_evic] = 0;
                    number_of_writes+=1;
                }
                else
                {
                    if (flag == 1)
                    {
                        printf("Page 0x%05x was read from disk, page 0x%05x was dropped (it was not dirty).\n",Page_Number, page_evic );
                    }
                    number_of_drops+=1;
                }
                // printf("%d %d\n",line_cnt, Dirty[page_evic]);
                Dirty[page_evic] = 0;
                Valid[Page_Number] = 1;
                frames[bestIndex] = Page_Number;
                Dirty[Page_Number]=0;
                if (strcmp(comp, tokens[1])==0)
                {
                    Dirty[Page_Number] = 1;
                }
            }
        }
    }
    fclose(fpointer);
    printf("Number of memory accesses: %d\n", number_of_reads);
    printf("Number of misses: %d\n", number_of_faults);
    printf("Number of writes: %d\n", number_of_writes);
    printf("Number of drops: %d\n", number_of_drops);
}
int main(int argv, char **argc)
{
    FrameNumber = (int*)malloc(sizeof(int)*(1<<20));
    Valid = (int*)malloc(sizeof(int)*(1<<20));
    memset(Valid, 0 , sizeof(int)*(1<<20));
    Dirty = (int*)malloc(sizeof(int)*(1<<20));
    memset(Dirty, 0, sizeof(int)*(1<<20));
    char *temp;
    temp = strdup(LEAST_RECENT_USED);
    if (strcmp(argc[3], temp)== 0)
    {
        if (argc[4]!=NULL)
        {
            LRU(argc[1], atoi(argc[2]),1);
        }
        else
        {
            LRU(argc[1], atoi(argc[2]),0);
        }
        return 0;
    }
    temp = strdup(FIRST_IN_FIRST_OUT);
    if (strcmp(argc[3], temp)== 0)
    {
        if (argc[4]!=NULL)
        {
            FIFO(argc[1], atoi(argc[2]),1);
        }
        else
        {
            FIFO(argc[1], atoi(argc[2]),0);
        }
        return 0;
    }
    temp = strdup(CLOCK);
    if (strcmp(argc[3], temp)== 0)
    {
        if (argc[4]!=NULL)
        {
            Clock(argc[1], atoi(argc[2]),1);
        }
        else
        {
            Clock(argc[1], atoi(argc[2]),0);
        }
        return 0;
    }
    temp = strdup(RANDOM);
    if (strcmp(argc[3], temp)== 0)
    {
        if (argc[4]!=NULL)
        {
            Random(argc[1], atoi(argc[2]),1);
        }
        else
        {
            Random(argc[1], atoi(argc[2]),0);
        }
        return 0;
    }
    temp = strdup(OPTIMAL);
    if (strcmp(argc[3], temp)== 0)
    {
        if (argc[4]!=NULL)
        {
            OPT(argc[1], atoi(argc[2]),1);
        }
        else
        {
            OPT(argc[1], atoi(argc[2]),0);
        }
        return 0;
    }
}
