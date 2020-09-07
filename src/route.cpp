#include "route.h"

int FindMin(
    unsigned int distance[],
    bool mark[]
)
{
    int n=0;
    unsigned int min=MAX_ARC_WEIGHT;
    for(int k=0;k<SAT_NUM;k++)
    {
        if(!mark[k] && distance[k]<min)
        {
            min = distance[k];
            n = k;
        }
    }
    if(min == MAX_ARC_WEIGHT)
        return -1;

    return n;

}

void Dijkstra(
    unsigned int sup_array[][SAT_NUM],
    int src_id,
    unsigned int distance[],
    int path[])
{
    int temp =src_id-1;
    bool mark[SAT_NUM];
    //initialize mark , add it to the set path[]
    for(int i=0;i < SAT_NUM;i++)
    {
        mark[i]=false;
        distance[i]=sup_array[temp][i];
        if(distance[i]!=MAX_ARC_WEIGHT)
            path[i]=temp;
        else
            path[i]=-1;
    }
    //mark the choosed point temp and update its distance
    mark[temp]=true;
    distance[temp]=0;

    for(int i=0;i < SAT_NUM;i++)
    {
        //find the closed point and mark it
        if((temp = FindMin(distance, mark)) == -1)
			return;
        mark[temp]=true;

        for(int j=0;j<SAT_NUM;j++)
        {
            //not marked and temp is the closed point, addit to the set and update the shortest path
            if(!mark[j] && (sup_array[temp][j]+distance[temp]< distance[j]))
            {
                distance[j]= sup_array[temp][j]+distance[temp];
                path[j]=temp;
            }
        }
    }
}

int FindPreNode(
    int path[],
    int src_id,
    int dst_id)
{
    int temp =dst_id-1;
    if(path[temp]==src_id-1)
       return temp;
    else if(path[temp] == -1)
       return -1;
    else 
       return FindPreNode(path,src_id,path[temp]+1);
}