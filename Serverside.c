//bgserver.c
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<math.h>
#define MAXLINE 20
#define SERV_PORT 5777
char filefortransportlayersender[20];
char filefornetworklayersender[20];
char fileformaclayersender[20];
char filetosendtotheotherend[20];
char filereadytosend[20];
int checkingsum[16];
static int counter = 0;

struct AppLayer
{
char descriptor[2] ;//this is 64 in decimal or 40 in hex
char bytecount[20]; // this is to store the number of bytes in the file
char msg[1000][1000];
char eof[1][2];
int transferablemsg[2000];
};
struct TransportLayer
{
int sourceport[16];
int destport[16];
int seqnum[32];
int acknum[32];
int headerlength[4];
int reserved[6];
int flags[6];
int windowsize[16];
int checksum[16];
int urgentpointer[16];
int options[24];
int padding[8];
int payload[3000];
};
int dataSize=0;
struct format
{
char      ver;
char      ihl ;///header length
char      tos[2];///type of service
char      len[4];///total length
char      id[4];///id no
char      flag[2];
char      offset[2];
char      ttl[2];
char      protocol[2];
char      chksum[4];
char      sourceip[4];
char      destip[4];
int      binchksum[16];
int      data[2000];
};

char *msg;//[]="This is CCN";
int msgsize;
int binheaders[8][16];
char headercombo[32],rcheadercombo[32];
typedef struct format iph;
iph ipheader,rcheader;
struct AppLayer app1;
struct TransportLayer tra1,tra2,tra3;
int currentmessagecount = 0;
struct frame
{
  int preamble[56];
  int sfd[8];
  int destaddress[16];
  int sourceaddress[16];
  char type[16];
  int msg[3000];
  int crc[32];


};
struct frame f1,f2;
int currentlengthofmessage=0;
int bin;
int numappend;
void dec_hex(long int num,int firstdimension,char str[1000][1000])   // Function Definition
{
long int rem[50],length=0,i=0;

int k = 0;

while(num>0)
  {
     rem[i]=num%16;
     num=num/16;
     i++;
     length++;
  }


for(i=length-1;i>=0;i--)
 {
   switch(rem[i])
   {
     case 10:
         str[firstdimension][k++] = 'A';
         break;
     case 11:
         str[firstdimension][k++] = 'B';
         break;
     case 12:
         str[firstdimension][k++] = 'C';
         break;
     case 13:
         str[firstdimension][k++] = 'D';
         break;
     case 14:
         str[firstdimension][k++] = 'E';
         break;
     case 15:
         str[firstdimension][k++] = 'F';
         break;
     default :
        str[firstdimension][k++] = (char)(rem[i]+48);
   }


 }
str[firstdimension][k] = '\0';
}
void dec_hex1(long int num,char str[20])   // Function Definition
{
long int rem[50],length=0,i=0;

int k = 0;

while(num>0)
  {
     rem[i]=num%16;
     num=num/16;
     i++;
     length++;
  }


for(i=length-1;i>=0;i--)
 {
   switch(rem[i])
   {
     case 10:
         str[k++] = 'A';
         break;
     case 11:
         str[k++] = 'B';
         break;
     case 12:
         str[k++] = 'C';
         break;
     case 13:
         str[k++] = 'D';
         break;
     case 14:
         str[k++] = 'E';
         break;
     case 15:
         str[k++] = 'F';
         break;
     default :
        str[k++] = (char)(rem[i]+48);
   }


}
str[k] = '\0';
}


void displaymsg(int i)
{
   int k;
   for(k=0;k<i;k++)
   printf("%s",app1.msg[k]);
   printf("\n");
}
void initializeappheader(int i)
{
int num = i;
app1.descriptor[0] = '4';
app1.descriptor[1]= '0';
sprintf(app1.bytecount,"%d",num);
dec_hex1(num,app1.bytecount);
app1.eof[0][0] = '0';
app1.eof[0][1] ='4';
}
void displayappheader(int i)
{
   printf("%s",app1.bytecount);
   displaymsg(i);
   printf(" %s",app1.eof);
   printf("\n");
}
char *applayersender()
{
   char str[100];
   char str1[100];
   char ch;
   int asciiequivalent;
   int i,j,k,l;
   FILE *fp;
   i = 0;




  printf("\nEnter the name of file you wish to see\n");
  scanf("%s",str);

  fp = fopen(str,"r"); // read mode

  if( fp == NULL )
  {
     perror("\nError while opening the file.\n");
     exit(EXIT_FAILURE);
  }


  i = 0;
  while( ( ch = fgetc(fp) ) != EOF )
     {
       asciiequivalent = (int)ch;
       dec_hex(asciiequivalent,i,app1.msg);
       i++;
     }
fclose(fp);
initializeappheader(i-1);
printf("\nThe entire block \n");
displayappheader(i-1);
strcpy(str,"copied.txt");

fp = fopen(str,"w");
   if ( (fp = fopen(str,"w") ) == NULL)
   {
         printf("\nSorry, cannot open the file %s",str);
         exit(1);
   }
   else
  {
       if ( (fp = fopen(str,"w") ) != NULL)
            {
               fprintf(fp,"%s ","40");
               if((i-1)>9)
               fprintf(fp,"%s ",app1.bytecount);
               else
               {
                 fprintf(fp,"%c%s ",'0',app1.bytecount);
               
               }
               for(k=0;k<i-1;k++)
                {
                  fprintf(fp, "%s ", app1.msg[k]);
                }
                fprintf(fp, "%s", app1.eof);
            }
   }
fclose(fp);
//printf("\n Up %s\n",str);
strcpy(filefortransportlayersender,str);
return str;
}

char * hex_binary(char hexa[])   /* Function to convert hexadecimal to binary. */
{   //printf(" \n enterered the hex to binary converter \n");
   char binarynum[3000]="";
   long int i = 0;
   while (hexa[i])
   {
       switch (hexa[i])
       {
       case '0':
       strcat(binarynum,"0000");
break;
       case '1':
       strcat(binarynum,"0001");
break;
       case '2':
       strcat(binarynum,"0010");
break;
       case '3':
          strcat(binarynum,"0011");
break;
       case '4':
       strcat(binarynum,"0100");
break;
       case '5':
       strcat(binarynum,"0101");
break;
       case '6':
       strcat(binarynum,"0110");
break;
       case '7':
       strcat(binarynum,"0111");
break;
       case '8':
       strcat(binarynum,"1000");
break;
       case '9':
       strcat(binarynum,"1001");
break;
       case 'A':
       strcat(binarynum,"1010");
break;
       case 'B':
       strcat(binarynum,"1011");
break;
       case 'C':
       strcat(binarynum,"1100");
break;
       case 'D':
       strcat(binarynum,"1101");
break;
       case 'E':
       strcat(binarynum,"1110");
break;
       case 'F':
       strcat(binarynum,"1111");
break;


       }
       i++;
   }

binarynum[4*i] = '\0';
//\n converted\n");
return binarynum;
}

void initializetransportheader()
{

 int sourceport[16] = {0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,0};
 int destport[16] = {0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,0};
 int headerlength[4] = {0,1,0,1};
 int reserved[6] = {0,0,0,0,0,0};
 int windowsize[16] = {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0};
 int urgentpointer[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 int i ;
 tra1.flags[0] = 0;
 tra1.flags[1] = 0;
 tra1.flags[2] = 0;
 tra1.flags[3] = 0;
 tra1.flags[4]=  1;
 tra1.flags[5] = 0;
 int option[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 int padding[8]={0,0,0,0,0,0,0,0};
 int checksum[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

 for(i=0;i<16;i++)
 {
     tra1.sourceport[i] = sourceport[i];
     tra1.destport[i] = destport[i];
     tra1.windowsize[i] = windowsize[i];
     tra1.urgentpointer[i] = urgentpointer[i];
     tra1.checksum[i] = checksum[i]=0;
 }
 for(i=0;i<24;i++)
  tra1.options[i] = option[i];
 for(i=0;i<8;i++)
  tra1.padding[i] = padding[i];
 for(i=0;i<4;i++)
   tra1.headerlength[i] = headerlength[i];
 for(i=0;i<6;i++)
   tra1.reserved[i] = reserved[i];
}

void seqnum(int i)
{
   int j = 0;
   for(j=0;j<32;j++)
   {
       tra1.seqnum[j] = 0;
   }
   tra1.seqnum[26] = i;
   for(j=0;j<32;j++)
    tra1.acknum[j] = 0;
}

void findchecksum()
{ int i;
 int k = 0;
 int combo1[16],combo2[16];
 int currlengthofcombo = 0;
 for(i=0;i<16;i++)
  tra1.checksum[i] = tra1.sourceport[i]^tra1.destport[i];
 for(i=0;i<16;i++)
  tra1.checksum[i] = tra1.checksum[i]^tra1.seqnum[i]^tra1.acknum[i];
 i = 16;
 for(k=0;k<16;k++)
 {
     tra1.checksum[k] = tra1.checksum[k]^tra1.seqnum[i]^tra1.acknum[i];
     i++;
 }

 k = 0;
 while(currlengthofcombo<4)
 {
    combo1[currlengthofcombo++] = tra1.headerlength[k++];
 }
 k=0;
 while(currlengthofcombo<10)
 {
    combo1[currlengthofcombo++] = tra1.reserved[k++];
 }
 k=0;
 while(currlengthofcombo<16)
 {
    combo1[currlengthofcombo++] = tra1.flags[0];
    combo1[currlengthofcombo++] = tra1.flags[1];
    combo1[currlengthofcombo++] = tra1.flags[2];
    combo1[currlengthofcombo++] = tra1.flags[3];
    combo1[currlengthofcombo++] = tra1.flags[4];
    combo1[currlengthofcombo++] = tra1.flags[5];
 }
 for(i=0;i<16;i++)
 tra1.checksum[i] = tra1.checksum[i]^combo1[i]^tra1.windowsize[i]^tra1.urgentpointer[i]^tra1.options[i];
 i = 16;
 k = 0;
 while(i<24)
 {
     combo2[k++]=tra1.options[i++];
 }
 for(i=0;i<8;i++)
 {
     combo2[k++] = tra1.padding[i];
 }
 for(i=0;i<16;i++)
 {
     tra1.checksum[i] = tra1.checksum[i]^combo2[i];
 }

 //for(i=0;i<16;i++)
 //tra1.checksum[i] = !tra1.checksum[i];


printf("\ncheck sum is \n");
 for(i=0;i<16;i++)
  printf("%d",tra1.checksum[i]);
}
void findchecksum1()
{ int i;
 int k = 0;
 int combo1[16],combo2[16];
 int currlengthofcombo = 0;
 for(i=0;i<16;i++)
  tra2.checksum[i] = tra2.sourceport[i]^tra2.destport[i];
 for(i=0;i<16;i++)
  tra2.checksum[i] = tra2.checksum[i]^tra2.seqnum[i]^tra2.acknum[i];
 i = 16;
 for(k=0;k<16;k++)
 {
     tra2.checksum[k] = tra2.checksum[k]^tra2.seqnum[i]^tra2.acknum[i];
     i++;
 }

 k = 0;
 while(currlengthofcombo<4)
 {
    combo1[currlengthofcombo++] = tra2.headerlength[k++];
 }
 k=0;
 while(currlengthofcombo<10)
 {
    combo1[currlengthofcombo++] = tra2.reserved[k++];
 }
 k=0;
 while(currlengthofcombo<16)
 {
    combo1[currlengthofcombo++] = tra2.flags[0];
    combo1[currlengthofcombo++] = tra2.flags[1];
    combo1[currlengthofcombo++] = tra2.flags[2];
    combo1[currlengthofcombo++] = tra2.flags[3];
    combo1[currlengthofcombo++] = tra2.flags[4];
    combo1[currlengthofcombo++] = tra2.flags[5];
 }
 for(i=0;i<16;i++)
 tra2.checksum[i] = tra2.checksum[i]^combo1[i]^tra2.windowsize[i]^tra2.urgentpointer[i]^tra2.options[i];
 i = 16;
 k = 0;
 while(i<24)
 {
     combo2[k++]=tra2.options[i++];
 }
 for(i=0;i<8;i++)
 {
     combo2[k++] = tra2.padding[i];
 }
 for(i=0;i<16;i++)
 {
     tra2.checksum[i] = tra2.checksum[i]^combo2[i];
 }

 //for(i=0;i<16;i++)
 //tra1.checksum[i] = !tra1.checksum[i];


printf("\ncheck sum pee is \n");
 for(i=0;i<16;i++)
  printf("%d",tra2.checksum[i]);
}
void findchecksum2()
{ 
 int i;
 int k = 0;
 int combo1[16],combo2[16];
 int currlengthofcombo = 0;
 for(i=0;i<16;i++)
  tra3.checksum[i] = tra3.sourceport[i]^tra3.destport[i];
 for(i=0;i<16;i++)
  tra3.checksum[i] = tra3.checksum[i]^tra3.seqnum[i]^tra3.acknum[i];
 i = 16;
 for(k=0;k<16;k++)
 {
     tra3.checksum[k] = tra3.checksum[k]^tra3.seqnum[i]^tra3.acknum[i];
     i++;
 }

 k = 0;
 while(currlengthofcombo<4)
 {
    combo1[currlengthofcombo++] = tra3.headerlength[k++];
 }
 k=0;
 while(currlengthofcombo<10)
 {
    combo1[currlengthofcombo++] = tra3.reserved[k++];
 }
 k=0;
 while(currlengthofcombo<16)
 {
    combo1[currlengthofcombo++] = tra3.flags[0];
    combo1[currlengthofcombo++] = tra3.flags[1];
    combo1[currlengthofcombo++] = tra3.flags[2];
    combo1[currlengthofcombo++] = tra3.flags[3];
    combo1[currlengthofcombo++] = tra3.flags[4];
    combo1[currlengthofcombo++] = tra3.flags[5];
 }
 for(i=0;i<16;i++)
 tra3.checksum[i] = tra3.checksum[i]^combo1[i]^tra3.windowsize[i]^tra3.urgentpointer[i]^tra3.options[i];
 i = 16;
 k = 0;
 while(i<24)
 {
     combo2[k++]=tra3.options[i++];
 }
 for(i=0;i<8;i++)
 {
     combo2[k++] = tra3.padding[i];
 }
 for(i=0;i<16;i++)
 {
     tra3.checksum[i] = tra3.checksum[i]^combo2[i];
 }

 //for(i=0;i<16;i++)
 //tra1.checksum[i] = !tra1.checksum[i];


printf("\ncheck sum pee is \n");
 for(i=0;i<16;i++)
 printf("%d",tra3.checksum[i]);
}
char * transportlayersender(char str[])
{
int i,length,trackofseqnum=1;
int msginbin[3000];
char c[3000];
char binary[3000];
FILE *fptr;
printf("\n%s\n",str);
  if ((fptr=fopen(str,"r"))==NULL)
  {
      printf("Error! opening file");
      exit(1);         /* Program exits if file pointer returns NULL. */
  }
  fscanf(fptr,"%[^\n]",c);
  printf("Data from file:\n%s",c);
  fclose(fptr);

strcpy(binary,hex_binary(c));
//printf("\n the binary equivalent is \n");
//puts(binary);
length = strlen(binary);
printf("Integer array representation \n");
for(i=0;i<length;i++)
tra1.payload[i] = (int)binary[i]-48;
for(i=0;i<length;i++)
printf("%d",tra1.payload[i]);
initializetransportheader();
// Here is where I'll generate the sequence number randomly
seqnum(trackofseqnum);
//function to find the checksum of the header(note that this does not include the data
findchecksum();
//now to write the entire transport header to a file and keep it ready for sending across to the network layer

strcpy(str,"synpacket.txt");
fptr = fopen(str,"w");
   if ( (fptr = fopen(str,"w") ) == NULL)
   {
         printf("Sorry, cannot open the file %s",str);
         exit(1);
   }
   else
  {
       if ( (fptr = fopen(str,"w") ) != NULL)
            {
                for(i=0;i<16;i++)
                fprintf(fptr, "%d", tra1.sourceport[i]);
                fprintf(fptr,"\n");
                for(i=0;i<16;i++)
                fprintf(fptr, "%d", tra1.destport[i]);
                fprintf(fptr,"\n");
                for(i=0;i<32;i++)
                fprintf(fptr, "%d", tra1.seqnum[i]);
                fprintf(fptr,"\n");
                for(i=0;i<32;i++)
                fprintf(fptr, "%d", tra1.acknum[i]);
                fprintf(fptr,"\n");
                for(i=0;i<4;i++)
                fprintf(fptr, "%d", tra1.headerlength[i]);
                fprintf(fptr,"\n");
                for(i=0;i<6;i++)
                fprintf(fptr, "%d", tra1.reserved[i]);
                fprintf(fptr,"\n");
               
                fprintf(fptr, "%d", tra1.flags[0]);
                fprintf(fptr, "%d", tra1.flags[1]);
                fprintf(fptr, "%d", tra1.flags[2]);
                fprintf(fptr, "%d", tra1.flags[3]);
                fprintf(fptr, "%d", tra1.flags[4]);
                fprintf(fptr, "%d", tra1.flags[5]);
                fprintf(fptr,"\n");
                for(i=0;i<16;i++)
                fprintf(fptr, "%d", tra1.windowsize[i]);
                fprintf(fptr,"\n");
                for(i=0;i<16;i++)
                fprintf(fptr, "%d", tra1.checksum[i]);
                fprintf(fptr,"\n");
                for(i=0;i<16;i++)
                fprintf(fptr, "%d", tra1.urgentpointer[i]);
                fprintf(fptr,"\n");
                for(i=0;i<24;i++)
                fprintf(fptr, "%d", tra1.options[i]);
                fprintf(fptr,"\n");
                for(i=0;i<8;i++)
                fprintf(fptr, "%d", tra1.padding[i]);
                fprintf(fptr,"\n");


            }
   }

fclose(fptr);
  strcpy(filefornetworklayersender,str);
return str;
}


char * transportlayersender1(char str[])
{
int i,length,trackofseqnum=1;
int msginbin[2000];
char c[2000];
char binary[2000];
char str1[30];
printf("\nENTERED THE FUNCTION\n");
FILE *fptr;
printf("\n%s\n",str);
  if ((fptr=fopen(str,"r"))==NULL)
  {
      printf("Error! opening file");
      exit(1);         /* Program exits if file pointer returns NULL. */
  }
  fscanf(fptr,"%[^\n]",c);
  printf("Data from file:\n%s",c);
  fclose(fptr);

strcpy(binary,hex_binary(c));
//printf("\n the binary equivalent is \n");
//puts(binary);
length = strlen(binary);
//printf("%d",length);
//printf("Inteer array representation \n");
//for(i=0;i<length;i++)
//tra3.payload[i] = (int)binary[i]-48;
//for(i=0;i<length;i++)
//printf("%d",tra3.payload[i]);
//initializetransportheader();

 int sourceport[16] = {0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,0};
 int destport[16] = {0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,0};
 int headerlength[4] = {0,1,0,1};
 int reserved[6] = {0,0,0,0,0,0};
 int windowsize[16] = {0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0};
 int urgentpointer[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 
 tra3.flags[0] = 0;
 tra3.flags[1] = 0;
 tra3.flags[2] = 0;
 tra3.flags[3] = 0;
 tra3.flags[4]=  0;
 tra3.flags[5] = 0;
 int option[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 int padding[8]={0,0,0,0,0,0,0,0};
 int checksum[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 int seqnum[32];
 int acknum[32];
 
 for(i=0;i<32;i++)
   {
     tra3.seqnum[i]=0;
     tra3.acknum[i]=0;
   }
 
  tra3.seqnum[25]=1;
  tra3.seqnum[31]=1;
     
       
 for(i=0;i<16;i++)
 {
     tra3.sourceport[i] = sourceport[i];
     tra3.destport[i] = destport[i];
     tra3.windowsize[i] = windowsize[i];
     tra3.urgentpointer[i] = urgentpointer[i];
     tra3.checksum[i] = checksum[i]=0;
 }

 for(i=0;i<24;i++)
  tra3.options[i] = option[i];
 for(i=0;i<8;i++)
  tra3.padding[i] = padding[i];
 for(i=0;i<4;i++)
   tra3.headerlength[i] = headerlength[i];
 for(i=0;i<6;i++)
   tra3.reserved[i] = reserved[i];


findchecksum2();


 

//now to write the entire transport header to a file and keep it ready for sending across to the network layer
printf("writing to synpacket.txt again \n");
strcpy(str1,"synpacket.txt");
fptr = fopen(str1,"w");
   if ( (fptr = fopen(str1,"w") ) == NULL)
   {
         printf("Sorry, cannot open the file %s",str);
         exit(1);
   }
   else
  {
       if ( (fptr = fopen(str1,"w") ) != NULL)
            {
                for(i=0;i<16;i++)
                fprintf(fptr, "%d", tra3.sourceport[i]);
                printf("1\n");
                fprintf(fptr,"\n");
                for(i=0;i<16;i++)
                fprintf(fptr, "%d", tra3.destport[i]);
                fprintf(fptr,"\n");
                for(i=0;i<32;i++)
                fprintf(fptr, "%d", tra3.seqnum[i]);
                fprintf(fptr,"\n");
                for(i=0;i<32;i++)
                fprintf(fptr, "%d", tra3.acknum[i]);
                fprintf(fptr,"\n");
                for(i=0;i<4;i++)
                fprintf(fptr, "%d", tra3.headerlength[i]);
                fprintf(fptr,"\n");
                for(i=0;i<6;i++)
                fprintf(fptr, "%d", tra3.reserved[i]);
                fprintf(fptr,"\n");
               
                fprintf(fptr, "%d", tra3.flags[0]);
                fprintf(fptr, "%d", tra3.flags[1]);
                fprintf(fptr, "%d", tra3.flags[2]);
                fprintf(fptr, "%d", tra3.flags[3]);
                fprintf(fptr, "%d", tra3.flags[4]);
                fprintf(fptr, "%d", tra3.flags[5]);
                fprintf(fptr,"\n");
                for(i=0;i<16;i++)
                fprintf(fptr, "%d", tra3.windowsize[i]);
                fprintf(fptr,"\n");
                for(i=0;i<16;i++)
                fprintf(fptr, "%d", tra3.checksum[i]);
                fprintf(fptr,"\n");
                for(i=0;i<16;i++)
                fprintf(fptr, "%d", tra3.urgentpointer[i]);
                fprintf(fptr,"\n");
                for(i=0;i<24;i++)
                fprintf(fptr, "%d", tra3.options[i]);
                fprintf(fptr,"\n");
                for(i=0;i<8;i++)
                fprintf(fptr, "%d", tra3.padding[i]);
                fprintf(fptr,"\n");

                fprintf(fptr, "%s", binary);
               // fprintf(fptr,"\n");
                printf("1\n");
            }
   }

fclose(fptr);
strcpy(filefornetworklayersender,str1);
return str1;
}




void init(char str1[100])
{

   int i=0;
   FILE *fp;char ch;
   char str[2000];
   fp = fopen(str1,"r");
   if( fp == NULL )
   {
     perror("Error while opening the file.\n");
     exit(EXIT_FAILURE);
   }
   while( ( ch = fgetc(fp) ) != EOF )
   {
       if(ch=='\n')
       {
         continue;
       }
       if(ch=='0')
           ipheader.data[i]=0;
       if(ch=='1')
           ipheader.data[i]=1;
       str[i]=ch;
       ++i;
   }
   //j=i;
   dataSize=i;

   msg=(char*)malloc((i+1)*sizeof(char));
   str[i]='\0';
   i=0;
   while(str[i]!='\0')
   {
       msg[i]=str[i];
       i++;
   }
  ipheader.ver='4';
  ipheader.ihl='5';
  ipheader.tos[0]='0';
  ipheader.tos[1]='0';
  ipheader.len[0]='0';
  ipheader.len[1]='0';
  ipheader.len[2]='0';
  ipheader.len[3]='0';
  ipheader.id[0]='1';
  ipheader.id[1]='c';
  ipheader.id[2]='4';
  ipheader.id[3]='6';
  ipheader.flag[0]='4';
  ipheader.flag[1]='0';
  ipheader.offset[0]='0';
  ipheader.offset[1]='0';
  ipheader.ttl[0]='4';
  ipheader.ttl[1]='0';
  ipheader.chksum[0]='0';
  ipheader.chksum[1]='0';
  ipheader.chksum[2]='0';
  ipheader.chksum[3]='0';
  ipheader.protocol[0]='0';
  ipheader.protocol[1]='6';
  ipheader.sourceip[0]='0';
  ipheader.sourceip[1]='0';
  ipheader.sourceip[2]='2';
  ipheader.sourceip[3]='2';
  ipheader.destip[0]='0';
  ipheader.destip[1]='0';
  ipheader.destip[2]='2';
  ipheader.destip[3]='2';

}

void dec_hexa(long int num)
{
long int rem[50],i=0,length=0;
int r=0;
char temp;
char buff[]="0000";
while(num>0)
  {
     rem[i]=num%16;
     num=num/16;
     i++;
     length++;
  }

printf("\nHexadecimal number : ");
for(i=length-1;i>=0;i--)
 {
   switch(rem[i])
   {
     case 10:
         temp='a';
         break;
     case 11:
         temp='b';
         break;
     case 12:
         temp='c';
         break;
     case 13:
         temp='d';
         break;
     case 14:
         temp='e';
         break;
     case 15:
         temp='f';
         break;
     default :
        temp=(char)(((int)'0')+rem[i]);
   }
   buff[r]=temp;
   r++;
 }
 for(i=r-1;i>=0;i--)
     ipheader.len[i+1]=buff[r-i-1];
}

void putLen()
{
   int res,i;
   res=msgsize/4+20;
   dec_hexa(res);
   printf("\n");
}

void calMsgLen()
{
   int i=0;
   int count=0;
   while(msg[i]!='\0')
   {
       i++;
       count++;
   }
   msgsize=count;
   putLen();
}

void display()
{
   int i;
   for(i=0;i<32;i++)
   {
       if(i%4==0)
           printf(" ");
       printf("%c",headercombo[i]);
   }
printf("\n");
}

void feedHeader()
{
           headercombo[0]=ipheader.ver;
           headercombo[1]=ipheader.ihl;
           headercombo[2]=ipheader.tos[0];
           headercombo[3]=ipheader.tos[1];
           headercombo[4]=ipheader.len[0];
           headercombo[5]=ipheader.len[1];
           headercombo[6]=ipheader.len[2];
           headercombo[7]=ipheader.len[3];
           headercombo[8]=ipheader.id[0];
           headercombo[9]=ipheader.id[1];
           headercombo[10]=ipheader.id[2];
           headercombo[11]=ipheader.id[3];
           headercombo[12]=ipheader.flag[0];
           headercombo[13]=ipheader.flag[1];
           headercombo[14]=ipheader.offset[0];
           headercombo[15]=ipheader.offset[1];
           headercombo[16]=ipheader.ttl[0];
           headercombo[17]=ipheader.ttl[1];
           headercombo[18]=ipheader.protocol[0];
           headercombo[19]=ipheader.protocol[1];
           headercombo[20]=ipheader.chksum[0];
           headercombo[21]=ipheader.chksum[1];
           headercombo[22]=ipheader.chksum[2];
           headercombo[23]=ipheader.chksum[3];
           headercombo[24]=ipheader.sourceip[0];
           headercombo[25]=ipheader.sourceip[1];
           headercombo[26]=ipheader.sourceip[2];
           headercombo[27]=ipheader.sourceip[3];
           headercombo[28]=ipheader.destip[0];
           headercombo[29]=ipheader.destip[1];
           headercombo[30]=ipheader.destip[2];
           headercombo[31]=ipheader.destip[3];

           display();

}



char *feedBin()
{
   int contiguos[128];
   int i,j,eq[4],k;
   k=0;
   int temp[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   printf("\n");
   FILE *fptr;

   for(i=0;i<32;i++)
   {
        switch(headercombo[i])
       {
       case 'a':
           eq[0]=1;
           eq[1]=0;
           eq[2]=1;
           eq[3]=0;
           break;
       case 'b':
           eq[0]=1;
           eq[1]=0;
           eq[2]=1;
           eq[3]=1;
           break;
     case 'c':
           eq[0]=1;
           eq[1]=1;
           eq[2]=0;
           eq[3]=0;
           break;
     case 'd':

           eq[0]=1;
           eq[1]=1;
           eq[2]=0;
           eq[3]=1;
           break;
     case 'e':
           eq[0]=1;
           eq[1]=1;
           eq[2]=1;
           eq[3]=0;
           break;
     case 'f':
           eq[0]=1;
           eq[1]=1;
           eq[2]=1;
           eq[3]=1;
           break;
     case '0':

       eq[0]=0;
       eq[1]=0;
       eq[2]=0;
       eq[3]=0;break;
     case '1':
       eq[0]=0;
       eq[1]=0;
       eq[2]=0;
       eq[3]=1;break;
     case '2':
       eq[0]=0;
       eq[1]=0;
       eq[2]=1;
       eq[3]=0;break;
     case '3':
       eq[0]=0;
       eq[1]=0;
       eq[2]=1;
       eq[3]=1;break;
     case '4':
       eq[0]=0;
       eq[1]=1;
       eq[2]=0;
       eq[3]=0;break;
     case '5':
       eq[0]=0;
       eq[1]=1;
       eq[2]=0;
       eq[3]=1;break;
     case '6':

       eq[0]=0;
       eq[1]=1;
       eq[2]=1;
       eq[3]=0;break;
     case '7':

       eq[0]=0;
       eq[1]=1;
       eq[2]=1;
       eq[3]=1;break;
     case '8':

       eq[0]=1;
       eq[1]=0;
       eq[2]=0;
       eq[3]=0;break;
     case '9':

       eq[0]=1;
       eq[1]=0;
       eq[2]=0;
       eq[3]=1;break;

   }

       contiguos[k]=eq[0];
       contiguos[k+1]=eq[1];
       contiguos[k+2]=eq[2];
       contiguos[k+3]=eq[3];


       k+=4;

   }


   printf("\n\n");
   for(i=0;i<16;i++)
   {
       temp[i]=contiguos[i];
   }
   for(i=0;i<128;i++)
   {
       if(i%16==0&&i>15)
       {
               temp[0]=(temp[0]+contiguos[i])%2;
               temp[1]=(temp[1]+contiguos[i+1])%2;
               temp[2]=(temp[2]+contiguos[i+2])%2;
               temp[3]=(temp[3]+contiguos[i+3])%2;
               temp[4]=(temp[4]+contiguos[i+4])%2;
               temp[5]=(temp[5]+contiguos[i+5])%2;
               temp[6]=(temp[6]+contiguos[i+6])%2;
               temp[7]=(temp[7]+contiguos[i+7])%2;
               temp[8]=(temp[8]+contiguos[i+8])%2;
               temp[9]=(temp[9]+contiguos[i+9])%2;
               temp[10]=(temp[10]+contiguos[i+10])%2;
               temp[11]=(temp[11]+contiguos[i+11])%2;
               temp[12]=(temp[12]+contiguos[i+12])%2;
               temp[13]=(temp[13]+contiguos[i+13])%2;
               temp[14]=(temp[14]+contiguos[i+14])%2;
               temp[15]=(temp[15]+contiguos[i+15])%2;

               printf("\n");


       }
       printf("%d",contiguos[i]);
   }
   printf("\nSUM\n");
   for(i=0;i<16;i++)
   {
       printf("%d",temp[i]);
       ipheader.binchksum[i]=temp[i];
   }

   char str[100];
   //printf("\nEnter file to write to \n");
   //scanf("%s",str);
  strcpy(str,"maclayerinput.txt");
   fptr = fopen(str,"w");
   if ( (fptr = fopen(str,"w") ) == NULL)
   {
         printf("Sorry, cannot open the file %s",str);
         exit(1);
   }
   else
  {
       /*if ( (fptr = fopen(str,"w") ) != NULL)
            {*/
                for(i=0;i<129;i++)
                {
                     if(i>127)
                     {
                       fprintf(fptr,"\n");
                       for(j=0;j<dataSize;j++)
                           fprintf(fptr,"%d",ipheader.data[j]);
                           continue;
                     }

                    if(i==80)
                    {
                        fprintf(fptr,"\n");
                        for(j=0;j<16;j++)
                           fprintf(fptr, "%d", ipheader.binchksum[j]);

                        i+=16;
                    }
                   if(i%16==0&&i!=0)
                   fprintf(fptr,"\n");
                   fprintf(fptr, "%d", contiguos[i]);

                }
            //}
   }

fclose(fptr);




   char q;
   for(i=0;i<16;i+=4)
   {
       k=temp[i]*8+temp[i+1]*4+temp[i+2]*2+temp[i+3]*1;
       switch(k)
       {
           case 10:
               q='a';
               break;
           case 11:
               q='b';
               break;
           case 12:
               q='c';
               break;
           case 13:
               q='d';
               break;
           case 14:
               q='e';
               break;
           case 15:
               q='f';
               break;
           default:
               q=(char)(((int)'0')+k);
               break;
       }
       if(i==0)
           ipheader.chksum[0]=q;
       if(i==4)
           ipheader.chksum[1]=q;
       if(i==8)
           ipheader.chksum[2]=q;
       if(i==12)
           ipheader.chksum[3]=q;

   }
printf("\nCHECKSUM\n");
for(i=0;i<4;i++)
printf("%c",ipheader.chksum[i]);
//printf("FLAG1");
strcpy(fileformaclayersender,str);
}
char * networklayersender(char str[])
{   char file[30];
   init(str);
   ///Senders Side
   calMsgLen();
   feedHeader();
   feedBin();
return file;

}
int  crccheck(struct frame f2)
{
int temp1,temp2;
int crc[32]={0};
int lower[]={0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1,1,0,1,1,1};
int countin = 32;
//int count=0;
//int i;
//int l;
//int k;
/*
numappend+=32;
while(count<numappend)
 {

   temp2 = crc[0];
   crc[0] = f2.msg[count];


   if(crc[31]==0)
    { i=0;
      countin = 32;
      while(countin>1)
      {
         temp1 = temp2;
         temp2 = crc[i+1];
         crc[i+1] = temp1;
         countin--;
         i++;

      }

    }
   else
    {
      i=0;
      countin = 32;
      while(countin>1)
      {
         temp1 = temp2;
         temp2 = crc[i+1];
         crc[i+1] = temp1;
         countin--;
         i++;

      }
     l = 0;
     for(k=31;k>=0;k--)
     {
      crc[k] = crc[k]^lowerorder[l];
      l++;
     }


    }


 count++;


}

printf("remainder in this case is  \n");
for(i=31;i>=0;i--)
{
  printf("%d",crc[i]);
}
for(i=0;i<32;i++)
{
   if(crc[i]==1)
       {
           printf("error in transmission \n");
           break;
       }
}
*/

int count=0;
int ck=0;
int i;
int pos;
numappend+=32; 
for(i=0;i<32;i++)
   crc[i]=0;
while(count<numappend)
   {
       if(crc[31]==0)
       {
           for(i=30;i>=0;i--)
            crc[i+1]=crc[i];

         crc[0]=f2.msg[count];
       }
       else
       {
           for(i=30;i>=0;i--)
            crc[i+1]=crc[i];

           crc[0]=f2.msg[count];
           pos=0;
           for(i=31;i>=0;i--)
           {
               crc[i]=crc[i]^lower[pos];
               ++pos;
           }
       }
     count++;
   }
 printf("\nCRC Checked\n");
 for(i=31;i>=0;i--)
  {
      printf("%d",crc[i]);
      if(crc[i]==0)
      ++ck;
  }

  if(ck!=32)
   {
    printf("\nError\n");
    exit(1);
   }
  else
   printf("\nNo Error\n");

return 1;
}
void initializepreamble()
{
 int i = 0;
 int sadd,dadd;
 for(i=0;i<56;i++)
 {
     if(i%2==0)
       f1.preamble[i] = 1;
     else
       f1.preamble[i]=0;

 }
 f1.sfd[0]=1;
 f1.sfd[1]=0;
 f1.sfd[2]=1;
 f1.sfd[3]=0;
 f1.sfd[4]=1;
 f1.sfd[5]=0;
 f1.sfd[6]=1;
 f1.sfd[7]=1;
 f1.type[0] = '0';
 f1.type[1] = '0';
 f1.type[2] = '0';
 f1.type[3] = '0';
 f1.type[4] = '1';
 f1.type[5] = '0';
 f1.type[6] = '0';
 f1.type[7] = '0';
 f1.type[8] = '0';
 f1.type[9] = '0';
 f1.type[10] = '0';
 f1.type[11] = '0';
 f1.type[12] = '0';
 f1.type[13] = '0';
 f1.type[14] = '0';
 f1.type[15] = '0';

 for(i=0;i<16;i++)
 { f1.sourceaddress[i] = tra1.sourceport[i];
   f1.destaddress[i] = tra1.destport[i];
 }
 
 

 printf("\n Preamble \n");
 for(i=0;i<56;i++)
 printf("%d",f1.preamble[i]);
 printf(" \n");
 printf("\n Start of Delimiter \n");
 for(i=0;i<8;i++)
 printf("%d",f1.sfd[i]);
 printf(" \n");
 
 printf("Type/Length %s \n",f1.type);

}

void maclayersender(char str[])
{
//printf("\n Mac%s\n",str);
int num,n,ret;
 int bin[80];

 int g[33]={1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1,1,0,1,1,1};
 int crc[32]={0};
 int lowerorder[]={0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,1,0,0,0,1,1,1,0,1,1,0,1,1,0,1,1,1};
 int i,j,k,l,x,rem;
 int count = 0;
 char ch;
 int temp1,temp2;int countin=32;
 char str1[30];
 /*printf("Enter your message: ");
   scanf("%s",str);
   printf("String is %s",str);*/
 i=0;
 FILE *fp;
 FILE *fptr;
 initializepreamble();
 fp = fopen(str,"r"); // read mode

  if( fp == NULL )
  {
     perror("Error while opening the file.\n");
     exit(EXIT_FAILURE);
  }


  currentlengthofmessage=0;
  while( ( ch = fgetc(fp) ) != EOF )
     {

         x = (int)ch-48;
         if(x!=-38)
         f1.msg[currentlengthofmessage++] = x;

    /*num=x;

    int k=0;
    while(num>0)
   {
    rem = num%2;
    num = num/2;
    bin[k]=rem;
    k++;
   }
 k = k-1;*/
 /* while(k>=0)
  {
    f1.msg[currentlengthofmessage++] = bin[k];
    bin[k] = 0;
    k--;

  }*/
}

  fclose(fp);


printf("\ncurrent message \n");
for(i=0;i<currentlengthofmessage;i++)
 {
  printf("%d",f1.msg[i]);
 }
printf("\n");





numappend = currentlengthofmessage-1;

 for(i=0;i<32;i++)
 {
    f1.msg[currentlengthofmessage++]=0;
 }


i=0;
 while(count<currentlengthofmessage)
 {

   temp2 = crc[0];
   crc[0] = f1.msg[count];


   if(crc[31]==0)
    { i=0;
      countin = 32;
      while(countin>1)
      {
         temp1 = temp2;
         temp2 = crc[i+1];
         crc[i+1] = temp1;
         countin--;
         i++;

      }

    }
   else
    {
      i=0;
      countin = 32;
      while(countin>1)
      {
         temp1 = temp2;
         temp2 = crc[i+1];
         crc[i+1] = temp1;
         countin--;
         i++;

      }
     l = 0;
     for(k=31;k>=0;k--)
     {
      crc[k] = crc[k]^lowerorder[l];
      l++;
     }


    }


 count++;

 }
printf("\nCRC is\n");
k = 0;
for(i=31;i>=0;i--)
 {

     f1.msg[numappend+32-i]=crc[i];
     f1.crc[k]=crc[i];
     printf("%d",f1.crc[k]);
     k++;

 }
 numappend+=32;

printf("\n sent message \n");

for(i=0;i<=numappend;i++)
{
 printf("%d",f1.msg[i]);
}
printf("\n");

//printf("\n Enter the file to write to for sending \n");
strcpy(str1,"readytosend.txt");
fptr = fopen(str1,"w");
   if ( (fptr = fopen(str1,"w") ) == NULL)
   {
         printf("Sorry, cannot open the file %s",str1);
         exit(1);
   }
   else
  {
       if ( (fptr = fopen(str1,"w") ) != NULL)
            {
                for(i=0;i<56;i++)
                fprintf(fptr, "%d",f1.preamble[i]);
                fprintf(fptr,"\n");
                for(i=0;i<8;i++)
                fprintf(fptr, "%d",f1.sfd[i]);
                fprintf(fptr,"\n");
                for(i=0;i<16;i++)
                fprintf(fptr, "%d",f1.sourceaddress[i]);
                fprintf(fptr,"\n");
                for(i=0;i<16;i++)
                fprintf(fptr, "%d",f1.destaddress[i]);
                fprintf(fptr,"\n");
                for(i=0;i<16;i++)
                fprintf(fptr, "%c",f1.type[i]);
                fprintf(fptr,"\n");
                for(i=0;i<=numappend;i++)
                fprintf(fptr, "%d",f1.msg[i]);
                fprintf(fptr,"\n");
                for(i=0;i<32;i++)
                fprintf(fptr, "%d", f1.crc[i]);
                fprintf(fptr,"\n");
               }
   }

fclose(fptr);
strcpy(filereadytosend,str1);


//crccheck();

}

void receivemaclayer(char str[])
{
  FILE *fp;
  char ch,str1[30];
  int count = 0,x,i;
  int check;
  fp = fopen(str,"r"); // read mode
  if( fp == NULL )
  {
     perror("Error while opening the file.\n");
     exit(EXIT_FAILURE);
  }


  
  while( ( ch = fgetc(fp) ) != EOF )
     {
        x = (int)ch-48;
        if(x!=-38)
      {
        if(count<56)
        f2.preamble[count] = x;   
        else if(count>=56 && count<64)
        f2.sfd[count-56] = x;
        else if(count>=64 && count<80)
        f2.sourceaddress[count-64] = x;
        else if(count>=80 && count<96)
        f2.destaddress[count-80] = x;
        else if(count>=96 &&count<112)
        f2.type[count-96] = ch;
        else if(count>=112 && count<=112+numappend)
        f2.msg[count-112] = x;
        else if(count>112+numappend&&count<=112+numappend+32)
        f2.crc[count-112-numappend-1] = x;

      
     count++;
     }
     
     }

fclose(fp);


               for(i=0;i<56;i++)
               printf("%d",f2.preamble[i]);
               printf("\n");
               for(i=0;i<8;i++)
               printf("%d",f2.sfd[i]);
               printf("\n");
               for(i=0;i<16;i++)
               printf("%d",f2.sourceaddress[i]);
               printf("\n");
               for(i=0;i<16;i++)
               printf("%d",f2.destaddress[i]);
               printf("\n");
               for(i=0;i<16;i++)
                printf( "%c",f2.type[i]);
                printf("\n");
                for(i=0;i<=numappend;i++)
                printf("%d",f2.msg[i]);
                printf("\n");
                for(i=0;i<32;i++)
                printf("%d", f2.crc[i]);
                printf("\n");
    
check = crccheck(f2);
if(check == 1)
{
// printf("Enter the file you wish to give to the network layer \n");
 //scanf("%s",str1);
strcpy(str1,"passtonetworklayer.txt");
 fp = fopen(str1,"w"); //write mode
  if( fp == NULL )
  {
     perror("Error while opening the file.\n");
     exit(EXIT_FAILURE);
  }
for(i=0;i<=numappend-32;i++)
fprintf(fp, "%d",f2.msg[i]);
fprintf(fp,"\n");
                
     
fclose(fp); 
}
}

void chksumerror(int chksum[128])
{
int i=0;
int temp[16];int flag=0;
for(i=0;i<16;i++)
temp[i]=chksum[i];

for(i=0;i<128;i++)
    {
        if(i%16==0&&i>15)
        {
                temp[0]=(temp[0]+chksum[i])%2;
                temp[1]=(temp[1]+chksum[i+1])%2;
                temp[2]=(temp[2]+chksum[i+2])%2;
                temp[3]=(temp[3]+chksum[i+3])%2;
                temp[4]=(temp[4]+chksum[i+4])%2;
                temp[5]=(temp[5]+chksum[i+5])%2;
                temp[6]=(temp[6]+chksum[i+6])%2;
                temp[7]=(temp[7]+chksum[i+7])%2;
                temp[8]=(temp[8]+chksum[i+8])%2;
                temp[9]=(temp[9]+chksum[i+9])%2;
                temp[10]=(temp[10]+chksum[i+10])%2;
                temp[11]=(temp[11]+chksum[i+11])%2;
                temp[12]=(temp[12]+chksum[i+12])%2;
                temp[13]=(temp[13]+chksum[i+13])%2;
                temp[14]=(temp[14]+chksum[i+14])%2;
                temp[15]=(temp[15]+chksum[i+15])%2;

                printf("\n");
        }
printf("%d",chksum[i]);
    }
printf("\n\nCHECKSUM ON RECEIVERS SIDE\n\n");
for(i=0;i<16;i++)
{
if(temp[i]!=0)flag=1;
printf("%d",temp[i]);
    }

if(flag)
printf("\nERROR IN TRANSMISSION\n");
else
printf("\nNO ERROR\n");


}


void writetotpLayer(int arr[],int size)
{
int i=0;FILE *fptr;
char filename[100];
//printf("\nEnter file to write to \n");
//scanf("%s",filename);
        strcpy(filename,"passtotransportlayer.txt");  
fptr = fopen(filename,"w");
   	if ( (fptr = fopen(filename,"w") ) == NULL)
  {
         printf("Sorry, cannot open the file %s",filename);
         exit(1);
   	}
for(i=1;i<size;i++)
{
  if(i<=16)
  {
fprintf(fptr,"%d",arr[i]);
continue;
  }        
  
  if(i==17)
  {
       fprintf(fptr,"\n");
fprintf(fptr,"%d",arr[16]);continue;
  }
  
  if(i>17&&i<33)
  {
fprintf(fptr,"%d",arr[i]);
continue;
  }

  if(i==33)
  {    
fprintf(fptr,"\n");
fprintf(fptr,"%d",arr[32]);continue;
  }
  if(i>33&&i<65)
  {
fprintf(fptr,"%d",arr[i]);
continue;
  }

 if(i==65)
     	 { 
fprintf(fptr,"\n");
fprintf(fptr,"%d",arr[65]);continue;
  }

  if(i>65&&i<97)
  {
fprintf(fptr,"%d",arr[i]);
continue;
  }

 if(i==97)
     	 {
fprintf(fptr,"\n");
fprintf(fptr,"%d",arr[97]);continue;
 }

  if(i>97&&i<101)
  {
fprintf(fptr,"%d",arr[i]);
continue;
  }
  if(i==101)
  {
fprintf(fptr,"\n");
fprintf(fptr,"%d",arr[101]);continue;
  }

  if(i>101&&i<107)
  {
fprintf(fptr,"%d",arr[i]);
continue;
  }
  if(i==107)
  {
fprintf(fptr,"\n");
fprintf(fptr,"%d",arr[107]);continue;
  }
  if(i>107&&i<113)
  {
fprintf(fptr,"%d",arr[i]);
continue;
  }
  if(i==113)
  {
fprintf(fptr,"\n");
fprintf(fptr,"%d",arr[113]);continue;
  }
  if(i>113&&i<129)
  {
fprintf(fptr,"%d",arr[i]);
continue;
  }

  if(i==129)
  {
fprintf(fptr,"\n");
fprintf(fptr,"%d",arr[129]);continue;
  }

 if(i>129&&i<145)
  {
fprintf(fptr,"%d",arr[i]);
continue;
  }

  if(i==145)
  {
fprintf(fptr,"\n");
fprintf(fptr,"%d",arr[145]);continue;
  }
  
           if(i>145&&i<161)
  {
fprintf(fptr,"%d",arr[i]);
continue;
  }

  if(i==161)
  {
fprintf(fptr,"\n");
fprintf(fptr,"%d",arr[161]);continue;
  }

  if(i>161&&i<185)
  {
fprintf(fptr,"%d",arr[i]);
continue;
  }

  if(i==185)
  {	
fprintf(fptr,"\n");
fprintf(fptr,"%d",arr[185]);continue;
  }

  if(i>185&&i<193)
  {
fprintf(fptr,"%d",arr[i]);
continue;
  }
   }
fclose(fptr); 
}


void readFileForNWLayer()
{
   FILE *fp;
   FILE *fptr;
   char ch;
   char filename[100];//="passtonetworklayer.txt";
   char str1[30];
   int i=0,k=0;int sizeOfReceivedData;
   char *receivedData;
   int frchksum[128];
   int *frtplayer;
   int sizeofdatatobesent=0;
   //printf("\nEnter Filename\n");
   //scanf("%s",filename);
   strcpy(filename,"passtonetworklayer.txt");
puts(filename);
  fp = fopen(filename,"r");
if( fp == NULL )
    {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
    }
    while( ( ch = fgetc(fp) )!=EOF )
    {
i++;
    }
    sizeOfReceivedData=i;
    fclose(fp);
    receivedData=(char*)malloc((i+1)*sizeof(char));
    i=0;    
    fp = fopen(filename,"r");
    if( fp == NULL )
    {
      perror("Error while opening the file.\n");
      exit(EXIT_FAILURE);
    }
    while( ( ch = fgetc(fp) )!=EOF )
    {
i++;
        receivedData[i]=ch;
    }
    fclose(fp);
    receivedData[i]='\0';
    frtplayer=(int*)malloc((sizeOfReceivedData-128)*sizeof(int));	
    for(i=0;i<128;i++)
    {
if(receivedData[i]=='0')
          frchksum[i]=0;
        if(receivedData[i]=='1')
 frchksum[i]=1;	
    }
//i--;
    for(;i<sizeOfReceivedData;i++)
    {
if(receivedData[i]=='0')
           frtplayer[k]=0;
        if(receivedData[i]=='1')
           frtplayer[k]=1;
        k++;
    }
sizeofdatatobesent=k;
printf("\nDATA TO BE SENT TO TRANSPORT LAYER\n");
for(i=0;i<k;i++)
printf("%d",frtplayer[i]);
printf("\n");  
chksumerror(frchksum);

writetotpLayer(frtplayer,sizeOfReceivedData);
 

}
void verifychecksum()
{ int i;
 int k = 0;
 int combo1[16],combo2[16];
 int currlengthofcombo = 0;
 for(i=0;i<16;i++)
  tra2.checksum[i] = tra2.sourceport[i]^tra2.destport[i];
 for(i=0;i<16;i++)
  tra2.checksum[i] = tra2.checksum[i]^tra2.seqnum[i]^tra2.acknum[i];
 i = 16;
 for(k=0;k<16;k++)
 {
     tra2.checksum[k] = tra2.checksum[k]^tra2.seqnum[i]^tra2.acknum[i];
     i++;
 }

 k = 0;
 while(currlengthofcombo<4)
 {
    combo1[currlengthofcombo++] = tra2.headerlength[k++];
 }
 k=0;
 while(currlengthofcombo<10)
 {
    combo1[currlengthofcombo++] = tra2.reserved[k++];
 }
 k=0;
 while(currlengthofcombo<16)
 {
    combo1[currlengthofcombo++] = tra2.flags[0];
    combo1[currlengthofcombo++] = tra2.flags[1];
    combo1[currlengthofcombo++] = tra2.flags[2];
    combo1[currlengthofcombo++] = tra2.flags[3];
    combo1[currlengthofcombo++] = tra2.flags[4];
    combo1[currlengthofcombo++] = tra2.flags[5];
 }
 for(i=0;i<16;i++)
 tra2.checksum[i] = tra2.checksum[i]^combo1[i]^tra2.windowsize[i]^tra2.urgentpointer[i]^tra2.options[i];
 i = 16;
 k = 0;
 while(i<24)
 {
     combo2[k++]=tra2.options[i++];
 }
 for(i=0;i<8;i++)
 {
     combo2[k++] = tra2.padding[i];
 }
 for(i=0;i<16;i++)
 {
     tra2.checksum[i] = tra2.checksum[i]^combo2[i];
 }

 //for(i=0;i<16;i++)
 //tra2.checksum[i] = !tra2.checksum[i];


printf("\ncheck sum is \n");
for(i=0;i<16;i++)
printf("%d",tra2.checksum[i]);

for(i=0;i<16;i++)
{
  checkingsum[i] = checkingsum[i]^tra2.checksum[i];
}

for(i=0;i<16;i++)
{
 if(checkingsum[i]==1)
{
   printf("\n Error in transmission \n");
   exit(1);
}
}


printf(" \n checksum verified \n");

}




void receivetransportlayer()
{

    FILE *f = fopen("passtotransportlayer.txt","r");
    char ch;
    int i=0;
    int count=0;
    while( ( ch = fgetc(f) ) != EOF )
    {
       if(ch=='\n')
       {
           ++count;
           i=0;
       }
      else
     {


       switch(count)
       {
         case 0:  tra2.sourceport[i]=ch-48;
                  ++i;
                  break;

         case 1:  tra2.destport[i]=ch-48;
                  ++i;
                  break;

         case 2:  tra2.seqnum[i]=ch-48;
                  ++i;
                  break;

         case 3:  tra2.acknum[i]=ch-48;
                  ++i;
                  break;

         case 4:  tra2.headerlength[i]=ch-48;
                  ++i;
                  break;

         case 5:  tra2.reserved[i]=ch-48;
                  ++i;
                  break;

         case 6:  tra2.flags[i]=ch-48;
                  ++i;
                  break;

         case 7:  tra2.windowsize[i]=ch-48;
                  ++i;
                  break;

         case 8:  tra2.checksum[i]=ch-48;
                  ++i;
                  break;

         case 9:  tra2.urgentpointer[i]=ch-48;
                  ++i;
                  break;

         case 10:  tra2.options[i]=ch-48;
                  ++i;
                  break;

         case 11: tra2.padding[i]=ch-48;
                  ++i;
                  break;

       }


     }// END OF ELSE

    }
fclose(f);

 
   


for(i=0;i<16;i++)
checkingsum[i] = tra2.checksum[i];
  
verifychecksum();
changeseqnoandflags(counter);
}
void changeseqnoandflags(int counter)
{

    tra2.seqnum[23]=1;
    tra2.seqnum[26]=0;
    tra2.seqnum[31]=0;
    //tra2.seqnum[26]=0;
    tra2.acknum[26]=0;
    tra2.acknum[23] = 1;
    tra2.acknum[31]=1;
    tra2.flags[1]=1;
    tra2.flags[4] = 0;
    findchecksum1();
    
   
    FILE *f = fopen("synpacket.txt", "w+");
    int i=0;
    for(i=0;i<16;i++)
    fprintf(f,"%d",tra2.sourceport[i]);
    fprintf(f,"\n");

    for(i=0;i<16;i++)
    fprintf(f,"%d",tra2.destport[i]);
    fprintf(f,"\n");

    for(i=0;i<32;i++)
    fprintf(f,"%d",tra2.seqnum[i]);
    fprintf(f,"\n");

    for(i=0;i<32;i++)
    fprintf(f,"%d",tra2.acknum[i]);
    fprintf(f,"\n");

    for(i=0;i<4;i++)
    fprintf(f,"%d",tra2.headerlength[i]);
    fprintf(f,"\n");

    for(i=0;i<6;i++)
    fprintf(f,"%d",tra2.reserved[i]);
    fprintf(f,"\n");

    for(i=0;i<6;i++)
    fprintf(f,"%d",tra2.flags[i]);
    fprintf(f,"\n");

    for(i=0;i<16;i++)
    fprintf(f,"%d",tra2.windowsize[i]);
    fprintf(f,"\n");

    for(i=0;i<16;i++)
    fprintf(f,"%d",tra2.checksum[i]);
    fprintf(f,"\n");

    for(i=0;i<16;i++)
    fprintf(f,"%d",tra2.urgentpointer[i]);
    fprintf(f,"\n");

    for(i=0;i<24;i++)
    fprintf(f,"%d",tra2.options[i]);
    fprintf(f,"\n");

    for(i=0;i<8;i++)
    fprintf(f,"%d",tra2.padding[i]);
    fprintf(f,"\n");

    fclose(f);
   
  // WHEN

}
void main()
{
  char sendingstring[3000],ch;
  char sendnumappend[10];
  char recv[3000]; 
  int listenfd,connfd,clilen;
  struct sockaddr_in servaddr,cliaddr;
  listenfd=socket(AF_INET,SOCK_STREAM,0);
  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family=AF_INET;
  servaddr.sin_port=htons(SERV_PORT);
  bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
  listen(listenfd,5);
  clilen=sizeof(cliaddr);
  connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);
  
  
  applayersender();
  transportlayersender(filefortransportlayersender);
  networklayersender(filefornetworklayersender);
  maclayersender(fileformaclayersender);  
  FILE *fp=fopen("readytosend.txt","r");
  int i=0;
  while( ( ch = fgetc(fp) ) != EOF )
  {
    sendingstring[i]=ch;
    ++i;
    //printf("%c",ch);
   }
  fclose(fp);
  
  sendingstring[1800]=1;
  sendingstring[1842]=1;
  sendingstring[i]=EOF;
  printf("\n SENT SYN PACKET \n");
  
  sprintf(sendnumappend,"%d",numappend);
  write(connfd,sendnumappend,sizeof(sendnumappend));
  write(connfd,sendingstring,sizeof(sendingstring));
  
  

  read(connfd,recv,sizeof(recv)); //READING SYN+ACK
  FILE *f3;
  f3=fopen("NEWF.txt","w+");
  i=0;
  while(recv[i]!=EOF)
  {
   fputc(recv[i],f3);
   ++i;
  }
  fclose(f3);
  
   receivemaclayer("NEWF.txt");
   readFileForNWLayer();
   receivetransportlayer(); 
printf("\n RECEIVED SYN + ACK \n");

  // SENDING ACK BACK
printf("\n SENDING ACK \n");

  sleep(2); 
  networklayersender("synpacket.txt");
  maclayersender(fileformaclayersender);  
  char str3[30];
   fp=fopen("readytosend.txt","r");
   i=0;
 
  while( ( ch = fgetc(fp) ) != EOF )
  {
    sendingstring[i]=ch;
    ++i;
    //printf("%c",ch);
   }
  fclose(fp);
  sendingstring[i]=EOF;
  
  sprintf(sendnumappend,"%d",numappend);
  

  write(connfd,sendnumappend,sizeof(sendnumappend));
  write(connfd,sendingstring,sizeof(sendingstring));
  
  read(connfd,str3,sizeof(str3));
  printf("\n%s\n",str3);
  //sleep(3);

  printf("\nCONNECTION ESTABLISHED\nSENDING DATA\n");
  sleep(2);
  transportlayersender1("copied.txt");   
  networklayersender(filefornetworklayersender);
  maclayersender(fileformaclayersender);  
  sprintf(sendnumappend,"%d",numappend);
  fp=fopen("readytosend.txt","r");
  //printf("qwert\n");
  i=0;
  while( ( ch = fgetc(fp) ) != EOF )
  {
    sendingstring[i]=ch;
    ++i;
    //printf("%c",ch);
  }
  fclose(fp);
  sendingstring[i]=EOF;
  write(connfd,sendnumappend,sizeof(sendnumappend));
  write(connfd,sendingstring,sizeof(sendingstring));  
  
 char strfin[40];
 read(connfd,strfin,sizeof(strfin));
  printf("\n%s RECEIEVED\n",strfin);

  close(listenfd);

  
  
}

