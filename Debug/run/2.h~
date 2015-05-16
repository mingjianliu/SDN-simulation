#include <fstream>

/*------------------------------------------------------------------------

	This part record the node to node cost, save it in pathcost

	Input	:	topology.txt, numberofnodes, pathcost[policy][][]

	Output	:	0 or -1

------------------------------------------------------------------------*/

int PathCost_Input(const char* filename,const int policy, const int number, int[][][] &pathcost) {

    FILE *file;
    int h=number;/*read the first line node and edge numbers*/
    /*read and skip the second line*/
    file=fopen(filename,"r");

    if(file==NULL) {
        cout<<"File doesn't exist."<<endl;
        fclose(file);
        return -1;
    }

    else {
        /*initialize traffic matrix*/

        for(int i=0; i<h; i++) {
            for(int j=0; j<h; j++) {
                if(i==j) {
                    pathcost[policy][i][j]=0;
                }
                else {
                    pathcost[policy][i][j]=99999;
                }
            }
        }
        /*input path cost raw data*/

        char ch=getc(file);
        int x,y,n;

        for(int i=0; ch!=EOF;)
        {
            if(ch =='\t') {
                i++;
                ch=getc(file);
            }
            else if(ch =='\n') {
                i=0;
                ch=getc(file);
            }
            else if(i==1) {
                char str[MAX_NUM]="";
                /*read the string*/
                n=0;
                while(ch!='\t') {
                    str[n]=ch;
                    ch=getc(file);
                    n++;
                }
                x=atoi(str);

            }
            else if(i==2) {
                char str[MAX_NUM]="";
                /*read the string*/
                n=0;
                while(ch!='\t') {
                    str[n]=ch;
                    ch=getc(file);
                    n++;
                }
                y=atoi(str);
            }

            else if(i==3) {
                char str[MAX_NUM]="";
                /*read the string*/
                n=0;
                while(ch!='\t') {
                    str[n]=ch;
                    ch=getc(file);
                    n++;
                }
                pathcost[policy][x][y]=atoi(str);
            }
            else {
                ch=getc(file);
            }
        }
    }
    fclose(file);
    return 0;
}

