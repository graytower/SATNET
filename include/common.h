#ifndef __COMMON_H__
#define __COMMOM_H__

#include "node.h"


#define ENABLE_PHYSICAL true
#define BUF_STR_SIZE 100
#define DEFAULT_TIMERSTATUS true

extern SatNode sats[SAT_NUM];
extern HostNode hosts[HOST_NUM];
extern StaticTopo G;
extern ISDB Int_DB[SAT_NUM];

#endif