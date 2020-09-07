#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "topo.h"
#include "cmd.h"




int FindMin(
    unsigned int distance[],
    bool mark[]
);

void Dijkstra(
    unsigned int sup_array[][SAT_NUM],
    int src_id,
    unsigned int distance[],
    int path[]
);

int FindPreNode(
    int path[],
    int src_id,
    int dst_id
);
#endif
