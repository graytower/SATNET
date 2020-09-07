#ifndef __CMD_H__
#define __CMD_H__

#include <iostream>
#include <unistd.h>
#include "common.h"
#include "topo.h"

void run_cmd(char *cmd,const char *attr);

void add_namespace(const char *name, int id);
void del_namespace(const char *name, int id);

void add_bridge(const char *name,int id);
void del_bridge(const char *name,int id);

void addLink_BrBr(
	const char *name1,int id1,int portId1,
	const char *name2,int id2,int portId2);


void delLink_BrBr(const char *name,int id,int portId);

void addLink_NsBr(
	const char *nsName,int nsId,int nsPortId,
	const char *brName,int brId,int brPortId);

void addLink_BrBr(
	const char *name1,int id1,int portId1,
	const char *name2,int id2,int portId2);

void delLink_BrBr(const char *name,int id,int portId);

void addLink_NsNs(
	const char *nsName1,int nsId1,int pId1,
	const char *nsName2,int nsId2,int pId2);

void change_LinkState(
	const char *name,int id,
	int portId,bool state);

void set_VethIp(
	const char *name,int id,int portId,
	const char *addr,const char *brAddr);

void print_LinkState(const char *name,int id,int portId);


void print_VethIp(const char *name,int id,int portId);

void add_node_namespace(
	const char *name,const char *br_name,
	int id,int addr,int maskSize);

void del_node_namespace(const char *name,const char *br_name,int id);

void add_satsat_link(
	const char *sname,int ids[],int pIds[],
	int addrs[],int maskSize,int linkId);

void del_sathost_link(
	const char *name,int id,int portId);

void add_sathost_link(
	const char *name1,int id1,int portId1,
	const char *name2,int id2,int portId2);

void IpStr(char *buf,int addr,int maskSize);
void IpStr(char *buf,int addr);
void BIpStr(char *buf,int addr,int maskSize);



void exchange_up(const char *name,int src_id);

void route_write(
	const char *name,int src_id,int src_portid,NodeAddress dst_addr,NodeAddress gw_addr) ;

void link_route_write(
	const char *name,int src_id,int src_portid,int dst_id,ISDB Int_DB[],NodeAddress gw_addr);

#endif