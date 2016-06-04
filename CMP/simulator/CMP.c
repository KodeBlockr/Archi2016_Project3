#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
FILE *writeSnapshot;
FILE *writeReport;
void add();
void addu();
void sub();
void and();
void or();
void xor();
void nor();
void nand();
void slt();
void sll();
void srl();
void sra();
void jr();
void addi();
void addiu();
void lw();
void lh();
void lhu();
void lb();
void lbu();
void sw();
void sh();
void sb();
void lui();
void andi();
void ori();
void nori();
void slti();
void beq();
void bne();
void bgtz(); // <--------------------------------------
void j();
void jal();
void printRegister();
int toBigEndian(unsigned int);
unsigned int iMemory[5000],Caddress,tmpp;
int reg[100],dMemory[5000],num;
int i=0,l,k,halt=0,tmp,tmpSigned,a,b,c,d,pc,sp,iNum,dNum,opcode,funct,rs,rt,rd,iCycle=0,dCycle=0,cycle=0,Cshamt,CimmediateUnsigned;
short Cimmediate;

/*-------------------------------------------------------*/
int parameter[11];
int par = 0;
void config_ICMP(int I_MEM_SIZE, int I_MEM_PAGE_SIZE, int I_CACHE_T_SIZE, int I_CACHE_BLOCK_SIZE, int I_CACHE_SET);
void config_DCMP(int D_MEM_SIZE, int D_MEM_PAGE_SIZE, int D_CACHE_T_SIZE, int D_CACHE_BLOCK_SIZE, int D_CACHE_SET);
void checkIMEM(int vAddress);
void checkDMEM(int vAddress);
typedef struct TLB{
    int VPN;
    int PPN;
    int lastCycleUsed;
    int valid;
}TLB;
typedef struct PT{
    int valid;
    int PPN;
}PT;
typedef struct Cache{
    int tag;
    int valid;
    int MRU;
} Cache;
typedef struct Memory{
    int lastCycleUsed;
    int valid;
} Memory;

TLB ITLB[1024], DTLB[1024];
PT IPT[1024], DPT[1024];
Cache ICA[1024][1024], DCA[1024][1024];
Memory IMEM[1024], DMEM[1024];
int iTLB_hit, iTLB_miss, iPT_hit, iPT_miss, iCA_miss, iCA_hit;
int iMemSize, iMemPageSize, iCacheTSize, iCacheBlockSize, iCacheSet;
int I_page_table_entries, I_TLB_entries, I_cache_entries, I_mem_entries;
int dTLB_hit, dTLB_miss, dPT_hit, dPT_miss, dCA_miss, dCA_hit;
int dMemSize, dMemPageSize, dCacheTSize, dCacheBlockSize, dCacheSet;
int D_page_table_entries, D_TLB_entries, D_cache_entries, D_mem_entries;

int main(int argc, char *argv[])
{
    memset(iMemory,0,5000);
    memset(dMemory,0,5000);
    memset(reg,0,100);
    FILE *iimage;
    FILE *dimage;
    iimage=fopen( "iimage.bin","rb");
    dimage=fopen( "dimage.bin","rb");
    writeSnapshot=fopen("snapshot.rpt","w");
    writeReport=fopen("report.rpt","w");
    if(argc == 11){
        for(par = 1 ; par < 11; par++){
            parameter[par] = atoi(argv[par]);
        }
	}else{
        parameter[1] = 64;
        parameter[2] = 32;
        parameter[3] = 8;
        parameter[4] = 16;
        parameter[5] = 16;
        parameter[6] = 4;
        parameter[7] = 4;
        parameter[8] = 16;
        parameter[9] = 4;
        parameter[10] = 1;
	}
	config_ICMP(parameter[1], parameter[3], parameter[5], parameter[6], parameter[7]);
	config_DCMP(parameter[2], parameter[4], parameter[8], parameter[9], parameter[10]);
    fread(&pc,sizeof(int),1,iimage);
    pc=toBigEndian(pc);
    fread(&iNum,sizeof(int),1,iimage);
    iNum=toBigEndian(iNum);
    iCycle=pc;
    i=pc;
    num=0;
    while(num!=iNum){
        fread(&iMemory[iCycle],sizeof(int),1,iimage);
        iCycle+=4;
        num++;
    }
    fread(&reg[29],sizeof(int),1,dimage);
    reg[29]=toBigEndian(reg[29]);
    sp=reg[29];
    fread(&dNum,sizeof(int),1,dimage);
    dNum=toBigEndian(dNum);
    while(dCycle!=dNum){
        fread(&dMemory[dCycle],sizeof(int),1,dimage);
        dCycle++;
    }
    fclose(iimage);
    fclose(dimage);
    //Little-Endian to Big-Endian
    printRegister();
    for(i=pc;i<iCycle;i+=4){
        iMemory[i]=toBigEndian(iMemory[i]);
    }
    for(i=0;i<dCycle;i++){
        dMemory[i]=toBigEndian(dMemory[i]);
    }
    for(i=pc;i<iCycle;){
        cycle++;
        opcode=iMemory[i]>>26;
        funct=(iMemory[i]<<26)>>26;
        rs=(iMemory[i]<<6)>>27;
        rt=(iMemory[i]<<11)>>27;
        rd=(iMemory[i]<<16)>>27;
        Cshamt=(iMemory[i]<<21)>>27;
        Cimmediate=((iMemory[i]<<16)>>16);
        CimmediateUnsigned=Cimmediate&0x0000FFFF;
        Caddress=(iMemory[i]<<6)>>6;
        checkIMEM(i);
        i+=4;
        if(opcode==0x00){
            if(funct==0x20)add();
            else if(funct==0x21)addu(); // <-------------------
            else if(funct==0x22)sub();
            else if(funct==0x24)and();
            else if(funct==0x25)or();
            else if(funct==0x26)xor();
            else if(funct==0x27)nor();
            else if(funct==0x28)nand();
            else if(funct==0x2A)slt();
            else if(funct==0x00)sll();
            else if(funct==0x02)srl();
            else if(funct==0x03)sra();
            else if(funct==0x08)jr();
        }
        else if(opcode==0x08)addi();
        else if(opcode==0x09)addiu(); // <-----------------
        else if(opcode==0x23)lw();
        else if(opcode==0x21)lh();
        else if(opcode==0x25)lhu();
        else if(opcode==0x20)lb();
        else if(opcode==0x24)lbu();
        else if(opcode==0x2B)sw();
        else if(opcode==0x29)sh();
        else if(opcode==0x28)sb();
        else if(opcode==0x0F)lui();
        else if(opcode==0x0C)andi();
        else if(opcode==0x0D)ori();
        else if(opcode==0x0E)nori();
        else if(opcode==0x0A)slti();
        else if(opcode==0x04)beq();
        else if(opcode==0x05)bne();
        else if(opcode==0x07)bgtz(); // <-----------
        else if(opcode==0x02)j();
        else if(opcode==0x03)jal();
        else if(opcode==0x3F)break;
        if(halt==1)break;
        printRegister();
    }
    fclose(writeSnapshot);
    fprintf(writeReport,"ICache :\n");
    fprintf(writeReport,"# hits: %d\n# misses: %d\n\n",iCA_hit,iCA_miss);
    fprintf(writeReport,"DCache :\n");
    fprintf(writeReport,"# hits: %d\n# misses: %d\n\n",dCA_hit,dCA_miss);
    fprintf(writeReport,"ITLB :\n");
    fprintf(writeReport,"# hits: %d\n# misses: %d\n\n",iTLB_hit,iTLB_miss);
    fprintf(writeReport,"DTLB :\n");
    fprintf(writeReport,"# hits: %d\n# misses: %d\n\n",dTLB_hit,dTLB_miss);
    fprintf(writeReport,"IPageTable :\n");
    fprintf(writeReport,"# hits: %d\n# misses: %d\n\n",iPT_hit,iPT_miss);
    fprintf(writeReport,"DPageTable :\n");
    fprintf(writeReport,"# hits: %d\n# misses: %d\n\n",dPT_hit,dPT_miss);
    fclose(writeReport);
}

void add(){
    if(rd==0){

    }
    int regTmp=reg[rs]+reg[rt];
    int regTmpTmp=regTmp>>31;
    int rsTmp=reg[rs]>>31;
    int rtTmp=reg[rt]>>31;
    if((rsTmp==rtTmp)&&(regTmpTmp!=rsTmp)){

    }
    reg[rd]=regTmp;
    if(rd==0){
        reg[rd]=0;
        return;
    }
}
void addu(){ // <----------------------
    if(rd==0){

    }
    int regTmp=reg[rs]+reg[rt];
    reg[rd]=regTmp;
    if(rd==0){
        reg[rd]  = 0;
        return;
    }
}
void sub(){
    if(rd==0){

    }
    int regTmp=reg[rs]-reg[rt];
    int regTmpTmp=regTmp>>31;
    int rsTmp=reg[rs]>>31;
    int rtTmp=(0-reg[rt])>>31;
    if((rsTmp==rtTmp)&&(regTmpTmp!=rsTmp)){

    }
    reg[rd]=regTmp;
    if(rd==0){
        reg[rd]=0;
        return;
    }
}
void and(){
    if(rd==0){

        return;
    }
    reg[rd]=reg[rs]&reg[rt];
}
void or(){
    if(rd==0){

        return;
    }
    reg[rd]=reg[rs]|reg[rt];
}
void xor(){
    if(rd==0){

        return;
    }
    reg[rd]=reg[rs]^reg[rt];
}
void nor(){
    if(rd==0){

        return;
    }
    reg[rd]=~(reg[rs]|reg[rt]);
}
void nand(){
    if(rd==0){

        return;
    }
    reg[rd]=~(reg[rs]&reg[rt]);
}
void slt(){
    if(rd==0){

        return;
    }
    reg[rd]=(reg[rs]<reg[rt]);
}
void sll(){
    if(rd==0){
        if(!(rt==0&&Cshamt==0)){

            return;
        }
    }
    reg[rd]=reg[rt]<<Cshamt;
}
void srl(){
    if(rd==0){

        return;
    }
    reg[rd]=reg[rt];
    for(l=0;l<Cshamt;l++){
        reg[rd]=(reg[rd]>>1)&0x7FFFFFFF;
    }
}
void sra(){
    if(rd==0){

        return;
    }
    reg[rd]=reg[rt]>>Cshamt;
}
void jr(){
    i=reg[rs];
}
void addi(){
    if(rt==0){

    }
    int regTmp=reg[rs]+Cimmediate;
    int regTmpTmp=regTmp>>31;
    int rsTmp=reg[rs]>>31;
    int cTmp=Cimmediate>>15;
    if((rsTmp==cTmp)&&(regTmpTmp!=rsTmp)){

    }
    reg[rt]=regTmp;
    if(rt==0){
        reg[rt]=0;
        return;
    }
}
void addiu(){ // <------------
    if(rt==0){

    }
    int regTmp=reg[rs]+Cimmediate;
    int regTmpTmp=regTmp>>31;
    int rsTmp=reg[rs]>>31;
    int cTmp=Cimmediate>>15;
    /*if((rsTmp==cTmp)&&(regTmpTmp!=rsTmp)){
        fprintf(writeError, "In cycle %d: Number Overflow\n", cycle);
    }*/
    reg[rt]=regTmp;
    if(rt==0){
        reg[rt]=0;
        return;
    }
}
void lw(){
    if(rt==0){

    }
    tmp=reg[rs]+Cimmediate;
    checkDMEM(tmp);
    if((reg[rs]>0&&Cimmediate>0&&tmp<0)||(reg[rs]<0&&Cimmediate<0&&tmp>0)){

    }
    if(tmp<0||(tmp+3<0)||(tmp>1023)||(tmp+3)>1023){

        halt=1;
    }
    if(tmp%4!=0){

        halt=1;
    }
    if(halt==1)return;
    reg[rt]=dMemory[tmp/4];
    if(rt==0){
        reg[rt]=0;
        return;
    }
}
void lh(){
    if(rt==0){

    }
    tmp=reg[rs]+Cimmediate;
    checkDMEM(tmp);
    int tmpTmp=tmp>>31;
    int rsTmp=reg[rs]>>31;
    int cTmp=Cimmediate>>15;
    if((rsTmp==cTmp)&&(tmpTmp!=rsTmp)){

    }
    if(tmp<0||(tmp+1<0)||(tmp>1023)||(tmp+1)>1023){

        halt=1;
    }
    if(tmp%2!=0){

        halt=1;
    }
    if(halt==1)return;
    if(rt==0){
        reg[rt]=0;
        return;
    }
    if(tmp%4==0)tmpSigned=dMemory[tmp/4]>>16;
    else if(tmp%4==2)tmpSigned=(dMemory[tmp/4]<<16)>>16;
    if(tmpSigned>>15==1)reg[rt]=tmpSigned|0xFFFF0000;
    else reg[rt]=tmpSigned;

}
void lhu(){
    if(rt==0){

    }
    tmp=reg[rs]+Cimmediate;
    checkDMEM(tmp);
    int tmpTmp=tmp>>31;
    int rsTmp=reg[rs]>>31;
    int cTmp=Cimmediate>>15;
    if((rsTmp==cTmp)&&(tmpTmp!=rsTmp)){

    }
    if(tmp<0||(tmp+1<0)||(tmp>1023)||(tmp+1)>1023){

        halt=1;
    }
    if(tmp%2!=0){

        halt=1;
    }
    if(halt==1)return;
    if(rt==0){
        reg[rt]=0;
        return;
    }
    tmpp=dMemory[tmp/4];
    if(tmp%4==0)tmpp=tmpp>>16;
    else if(tmp%4==2)tmpp=(tmpp<<16)>>16;
    reg[rt]=tmpp;
}
void lb(){
    if(rt==0){

    }
    tmp=reg[rs]+Cimmediate;
    checkDMEM(tmp);
    int tmpTmp=tmp>>31;
    int rsTmp=reg[rs]>>31;
    int cTmp=Cimmediate>>15;
    if((rsTmp==cTmp)&&(tmpTmp!=rsTmp)){

    }
    if(tmp<0||tmp>1023){

        halt=1;
    }
    if(halt==1)return;
    if(rt==0){
        reg[rt]=0;
        return;
    }
    if(tmp%4==0)tmpSigned=dMemory[tmp/4]>>24;
    else if(tmp%4==1)tmpSigned=(dMemory[tmp/4]<<8)>>24;
    else if(tmp%4==2)tmpSigned=(dMemory[tmp/4]<<16)>>24;
    else if(tmp%4==3)tmpSigned=(dMemory[tmp/4]<<24)>>24;
    if(tmpSigned>>7==1)reg[rt]=tmpSigned|0xFFFFFF00;
    else reg[rt]=tmpSigned;
}
void lbu(){
    if(rt==0){

    }
    tmp=reg[rs]+Cimmediate;
    checkDMEM(tmp);
    int tmpTmp=tmp>>31;
    int rsTmp=reg[rs]>>31;
    int cTmp=Cimmediate>>15;
    if((rsTmp==cTmp)&&(tmpTmp!=rsTmp)){

    }
    if(tmp<0||tmp>1023){

        halt=1;
    }
    if(halt==1)return;
    if(rt==0){
        reg[rt]=0;
        return;
    }
    tmpp=dMemory[tmp/4];
    if(tmp%4==0)tmpp=tmpp>>24;
    else if(tmp%4==1)tmpp=(tmpp<<8)>>24;
    else if(tmp%4==2)tmpp=(tmpp<<16)>>24;
    else if(tmp%4==3)tmpp=(tmpp<<24)>>24;
    reg[rt]=tmpp;
}
void sw(){
    tmp=reg[rs]+Cimmediate;
    checkDMEM(tmp);
    int tmpTmp=tmp>>31;
    int rsTmp=reg[rs]>>31;
    int cTmp=Cimmediate>>15;
    if((rsTmp==cTmp)&&(tmpTmp!=rsTmp)){

    }
    if(tmp<0||(tmp+3<0)||(tmp>1023)||(tmp+3)>1023){

        halt=1;
    }
    if(tmp%4!=0){

        halt=1;
    }
    if(halt==1)return;
    dMemory[tmp/4]=reg[rt];
}
void sh(){
    tmp=reg[rs]+Cimmediate;
    checkDMEM(tmp);
    int tmpTmp=tmp>>31;
    int rsTmp=reg[rs]>>31;
    int cTmp=Cimmediate>>15;
    if((rsTmp==cTmp)&&(tmpTmp!=rsTmp)){

    }
    if(tmp<0||(tmp+1<0)||(tmp>1023)||(tmp+1)>1023){

        halt=1;
    }
    if(tmp%2!=0){

        halt=1;
    }
    if(halt==1)return;
    tmpSigned=reg[rt]&0x0000FFFF;
    if(tmp%4==0)dMemory[tmp/4]=(dMemory[tmp/4]&0x0000FFFF)+(tmpSigned<<16);
    else if(tmp%4==2)dMemory[tmp/4]=(dMemory[tmp/4]&0xFFFF0000)+tmpSigned;
}
void sb(){
    tmp=reg[rs]+Cimmediate;
    checkDMEM(tmp);
    int tmpTmp=tmp>>31;
    int rsTmp=reg[rs]>>31;
    int cTmp=Cimmediate>>15;
    if((rsTmp==cTmp)&&(tmpTmp!=rsTmp)){

    }
    if(tmp<0||tmp>1023){

        halt=1;
    }
    if(halt==1)return;
    tmpSigned=reg[rt]&0x000000FF;
    if(tmp%4==0)dMemory[tmp/4]=(dMemory[tmp/4]&0x00FFFFFF)+(tmpSigned<<24);
    else if(tmp%4==1)dMemory[tmp/4]=(dMemory[tmp/4]&0xFF00FFFF)+(tmpSigned<<16);
    else if(tmp%4==2)dMemory[tmp/4]=(dMemory[tmp/4]&0xFFFF00FF)+(tmpSigned<<8);
    else if(tmp%4==3)dMemory[tmp/4]=(dMemory[tmp/4]&0xFFFFFF00)+tmpSigned;
}
void lui(){
    if(rt==0){

    }
    else reg[rt]=(int)(CimmediateUnsigned<<16);
}
void andi(){
    if(rt==0){

    }
    else reg[rt]=reg[rs]&CimmediateUnsigned;
}
void ori(){
    if(rt==0){

    }
    else reg[rt]=reg[rs]|CimmediateUnsigned;
}
void nori(){
    if(rt==0){

    }
    else reg[rt]=~(reg[rs]|CimmediateUnsigned);
}
void slti(){
    if(rt==0){

    }
    else reg[rt]=(reg[rs]<Cimmediate);
}
void beq(){
    int Cimmediate4=Cimmediate*4;
    int pcTmp=i+Cimmediate4;
    if((Cimmediate>0&&Cimmediate4<=0)||(Cimmediate<0&&Cimmediate4>=0)||(i>0&&Cimmediate4>0&&pcTmp<0)||(i<0&&Cimmediate4<0&&pcTmp>0)){

    }
    if(reg[rs]==reg[rt])i=pcTmp;
}
void bne(){
    int Cimmediate4=Cimmediate*4;
    int pcTmp=i+Cimmediate4;
    if((Cimmediate>0&&Cimmediate4<=0)||(Cimmediate<0&&Cimmediate4>=0)||(i>0&&Cimmediate4>0&&pcTmp<0)||(i<0&&Cimmediate4<0&&pcTmp>0)){

    }
    if(reg[rs]!=reg[rt])i=pcTmp;
}
void bgtz(){
    int Cimmediate4=Cimmediate*4;
    int pcTmp=i+Cimmediate4;
    if((Cimmediate>0&&Cimmediate4<=0)||(Cimmediate<0&&Cimmediate4>=0)||(i>0&&Cimmediate4>0&&pcTmp<0)||(i<0&&Cimmediate4<0&&pcTmp>0)){

    }
    if(reg[rs]>0)i=pcTmp;
}
void j(){
    i=((i>>28)<<28)+Caddress*4;
}
void jal(){
    reg[31]=i;
    i=((i>>28)<<28)+Caddress*4;
}
void printRegister(){
    fprintf(writeSnapshot, "cycle %d\n", cycle);
    for(k=0;k<32;k++){
        fprintf(writeSnapshot, "$%02d: 0x%08X\n",k,reg[k]);
    }
    fprintf(writeSnapshot, "PC: 0x%08X\n\n\n", i);
}
int toBigEndian(unsigned int k){
    a=k>>24;
    b=((k<<8)>>24)<<8;
    c=((k>>8)<<24)>>8;
    d=k<<24;
    k=a+b+c+d;
    return k;
}

void config_ICMP(int I_MEM_SIZE, int I_MEM_PAGE_SIZE, int I_CACHE_T_SIZE, int I_CACHE_BLOCK_SIZE, int I_CACHE_SET){
    iMemSize = I_MEM_SIZE;
    iMemPageSize = I_MEM_PAGE_SIZE;
    iCacheTSize = I_CACHE_T_SIZE;
    iCacheBlockSize = I_CACHE_BLOCK_SIZE;
    iCacheSet = I_CACHE_SET;
    iTLB_hit = 0;
    iTLB_miss = 0;
    iPT_hit = 0;
    iPT_miss = 0;
    I_page_table_entries = 1024/I_MEM_PAGE_SIZE;
    I_TLB_entries = I_page_table_entries/4;
    I_cache_entries = I_CACHE_T_SIZE/I_CACHE_SET/I_CACHE_BLOCK_SIZE;
    I_mem_entries = I_MEM_SIZE/I_MEM_PAGE_SIZE;
    int i;
    for( i = 0; i < I_page_table_entries; i++){
        IPT[i].PPN = 0;
        IPT[i].valid = 0;
    }
    for( i = 0; i < I_page_table_entries; i++){
        ITLB[i].PPN = 0;
        ITLB[i].lastCycleUsed = 0;
        ITLB[i].valid = 0;
        ITLB[i].VPN = 0;
    }
    for(i = 0; i < I_mem_entries; i++){
        IMEM[i].lastCycleUsed = 0;
        IMEM[i].valid = 0;
    }
    int j ;
    for(i = 0; i < I_cache_entries; i++){
        for(j = 0; j < I_CACHE_SET; j++){
            ICA[i][j].MRU = 0;
            ICA[i][j].tag = 0;
            ICA[i][j].valid = 0;
        }
    }

}

void config_DCMP(int D_MEM_SIZE, int D_MEM_PAGE_SIZE, int D_CACHE_T_SIZE, int D_CACHE_BLOCK_SIZE, int D_CACHE_SET){
    dMemSize = D_MEM_SIZE;
    dMemPageSize = D_MEM_PAGE_SIZE;
    dCacheTSize = D_CACHE_T_SIZE;
    dCacheBlockSize = D_CACHE_BLOCK_SIZE;
    dCacheSet = D_CACHE_SET;
    dTLB_hit = 0;
    dTLB_miss = 0;
    dPT_hit = 0;
    dPT_miss = 0;
    D_page_table_entries = 1024/D_MEM_PAGE_SIZE;
    D_TLB_entries = D_page_table_entries/4;
    D_cache_entries = D_CACHE_T_SIZE/D_CACHE_SET/D_CACHE_BLOCK_SIZE;
    D_mem_entries = D_MEM_SIZE/D_MEM_PAGE_SIZE;
    int i;
    for( i = 0; i < D_page_table_entries; i++){
        DPT[i].PPN = 0;
        DPT[i].valid = 0;
    }
    for( i = 0; i < D_page_table_entries; i++){
        DTLB[i].PPN = 0;
        DTLB[i].lastCycleUsed = 0;
        DTLB[i].valid = 0;
        DTLB[i].VPN = 0;
    }
    for(i = 0; i < D_mem_entries; i++){
        DMEM[i].lastCycleUsed = 0;
        DMEM[i].valid = 0;
    }
    int j ;
    for(i = 0; i < D_cache_entries; i++){
        for(j = 0; j < D_CACHE_SET; j++){
            DCA[i][j].MRU = 0;
            DCA[i][j].tag = 0;
            DCA[i][j].valid = 0;
        }
    }

}

void checkIMEM(int vAddress){
    int vpn = vAddress/iMemPageSize;
    int page_offset = vAddress%iMemPageSize;
    int ppn, Iptppn, Ippn;
    bool checkTLB = false; //find VPN in TLB
    bool checkPT = false;
    bool checkCa = false;
    int i, j;
    for(i = 0; i < I_TLB_entries ; i++){   // find TLB
        if(ITLB[i].VPN == vpn && ITLB[i].valid == 1){
            ITLB[i].lastCycleUsed = cycle;
            ppn = ITLB[i].PPN;
            checkTLB = true;
            break;
        }
    }

    if(checkTLB==false){ //TLB MISS!! Find PageTable
        iTLB_miss++;
        //Find PageTable
        if(IPT[vpn].valid==1){
            IMEM[IPT[vpn].PPN].lastCycleUsed= cycle;
            Iptppn = IPT[vpn].PPN; //Hit
            checkPT = true;
        }else checkPT = false;

        if(checkPT==false){  // PT miss!!
            iPT_miss++;
            int MIN = 500001;
            int temp_ppn = 0;
            bool check = false;
            for(i = 0; i < I_mem_entries; i++){
                if(IMEM[i].valid==0){
                    temp_ppn = i;
                    check = true;
                    break;
                }else{
                    if(IMEM[i].lastCycleUsed<MIN){
                        MIN = IMEM[i].lastCycleUsed;
                        temp_ppn = i;
                    }
                }
            }
            IMEM[temp_ppn].lastCycleUsed = cycle;
            IMEM[temp_ppn].valid = 1;

            if(check){
                IPT[vpn].PPN = temp_ppn;
                IPT[vpn].valid = 1;
            }else{
                for(i = 0; i < I_page_table_entries; i++){
                    if(IPT[i].PPN == temp_ppn){
                        IPT[i].valid = 0;
                    }
                }
                IPT[vpn].PPN = temp_ppn;
                IPT[vpn].valid = 1;

                for(i = 0; i < I_TLB_entries; i++){
                    if(ITLB[i].PPN == temp_ppn){
                        ITLB[i].valid = 0;
                    }
                }

                for(j = 0; j < iMemPageSize; j++){
                    int pAddress = temp_ppn * iMemPageSize + j;
                    int pAddressBlock = pAddress/iCacheBlockSize;
                    int block = pAddressBlock % I_cache_entries;
                    int tag = pAddressBlock / I_cache_entries;
                    if(iCacheSet == 1){
                        if(ICA[block][0].tag == tag){
                            ICA[block][0].valid = 0;
                        }
                    }else{
                        for( i = 0; i < iCacheSet; i ++){
                            if(ICA[block][i].tag == tag && ICA[block][i].valid == 1){
                                ICA[block][i].valid = 0;
                                ICA[block][i].MRU = 0;
                            }
                        }
                    }
                }
            }

            MIN = 500001;
            int temp_address = 0;
            for(i = 0; i < I_TLB_entries; i++){
                if(ITLB[i].valid==0){
                    temp_address = i;
                    break;
                }else if(ITLB[i].lastCycleUsed<MIN){
                    MIN = ITLB[i].lastCycleUsed;
                    temp_address = i;
                }
            }

            ITLB[temp_address].lastCycleUsed = cycle;
            ITLB[temp_address].valid = 1;
            ITLB[temp_address].PPN = temp_ppn;
            ITLB[temp_address].VPN = vpn;


        }else{ //PT Hit!!
            iPT_hit++;
            int TLBMIN = 500001;
            int TLB_temp_Address = 0;
            int TLB_temp_ppn;
            TLB_temp_ppn = IPT[vpn].PPN;
            for(i = 0; i < I_TLB_entries; i++){
                if(ITLB[i].valid==0){
                    TLB_temp_Address = i;
                    break;
                }else if(ITLB[i].lastCycleUsed<TLBMIN){
                    TLBMIN = ITLB[i].lastCycleUsed;
                    TLB_temp_Address = i;
                }
            }
            ITLB[TLB_temp_Address].lastCycleUsed = cycle;
            ITLB[TLB_temp_Address].valid = 1;
            ITLB[TLB_temp_Address].PPN = TLB_temp_ppn;
            ITLB[TLB_temp_Address].VPN = vpn;
            IMEM[TLB_temp_ppn].lastCycleUsed = cycle;
        }
    }else{  //TLB Hit!!
        iTLB_hit++;
    }
    //TLB HIT and get PPN
    // then check Cache Use CheckCA
    for(i = 0; i < I_TLB_entries; i++){
        if(ITLB[i].VPN == vpn && ITLB[i].valid == 1){
            ITLB[i].lastCycleUsed = cycle;
            Ippn = ITLB[i].PPN;
            break;
        }
    }
    checkCa = false ;
    //check Cache
    int CAPA = Ippn*iMemPageSize+page_offset;
    int CAPAB = CAPA/iCacheBlockSize;
    int block = CAPAB%I_cache_entries;
    int tag = CAPAB/I_cache_entries;
    int check = 0;
    int temp_j = 0;

    if(iCacheSet==1){
        if(tag==ICA[block][0].tag && ICA[block][0].valid==1){
            checkCa = true;
        }
    }else{
        for(i = 0; i<iCacheSet; i++){
            if(tag==ICA[block][i].tag && ICA[block][i].valid == 1){
                for( j = 0; j < iCacheSet; j++){
                    if(ICA[block][j].MRU==0){
                        if(check == 0){
                            temp_j = j;
                            check = 1;
                        }else{
                            check = 0;
                            break;
                        }
                    }
                }
                if(check == 1 && temp_j == i){
                    for(j = 0; j < iCacheSet; j++){
                        ICA[block][j].MRU = 0;
                    }
                }
                ICA[block][i].MRU = 1;
                checkCa = true;
            }
        }
    }
    if(checkCa==false){
        iCA_miss++;
        // Cache miss
        CAPA = Ippn*iMemPageSize+page_offset;
        CAPAB = CAPA/iCacheBlockSize;
        block = CAPAB%I_cache_entries;
        tag = CAPAB/I_cache_entries;
        check = 0;
        temp_j = 0;
        int checkM = 0;
        int get = 0;
        int temp_i = 0;
        if(iCacheSet==1){
            ICA[block][0].MRU = 0;
            ICA[block][0].tag = tag;
            ICA[block][0].valid = 1;
        }else{
            for(i = 0; i < iCacheSet; i++){
                if(ICA[block][i].valid==0){
                    temp_i = i;
                    check = 1;
                    break;
                }
            }
            for(i = 0; i < iCacheSet; i++){
                if(ICA[block][i].MRU ==0){
                    if(checkM==0){
                       get = i;
                       checkM = 1;
                    }else{
                        checkM = -1;
                        break;
                    }
                }
            }
            if(check==1){
                if(checkM==1&& temp_i ==  get){
                    for(i=0; i < iCacheSet; i++){
                        ICA[block][i].MRU = 0;
                    }
                }
                ICA[block][temp_i].MRU = 1;
                ICA[block][temp_i].tag = tag;
                ICA[block][temp_i].valid = 1;
            }else{
                if(checkM==1){
                    for(i = 0; i < iCacheSet; i++){
                        ICA[block][i].MRU = 0;
                    }
                }
                ICA[block][get].MRU = 1;
                ICA[block][get].tag = tag;
                ICA[block][get].valid = 1;
            }
        }


    }else{
        iCA_hit++;
    }


}
void checkDMEM(int vAddress){
    int vpn = vAddress/dMemPageSize;
    int page_offset = vAddress%dMemPageSize;
    int ppn, Dptppn, Dppn;
    bool checkTLB = false; //find VPN in TLB
    bool checkPT = false;
    bool checkCa = false;
    int i, j;
    for(i = 0; i < D_TLB_entries ; i++){   // find TLB
        if(DTLB[i].VPN == vpn && DTLB[i].valid == 1){
            DTLB[i].lastCycleUsed = cycle;
            ppn = DTLB[i].PPN;
            checkTLB = true;
            break;
        }
    }

    if(checkTLB==false){ //TLB MISS!! Find PageTable
        dTLB_miss++;
        //Find PageTable
        if(DPT[vpn].valid==1){
            DMEM[DPT[vpn].PPN].lastCycleUsed= cycle;
            Dptppn = DPT[vpn].PPN; //Hit
            checkPT = true;
        }else checkPT = false;

        if(checkPT==false){  // PT miss!!
            dPT_miss++;
            int MIN = 500001;
            int temp_ppn = 0;
            bool check = false;
            for(i = 0; i < D_mem_entries; i++){
                if(DMEM[i].valid==0){
                    temp_ppn = i;
                    check = true;
                    break;
                }else{
                    if(DMEM[i].lastCycleUsed<MIN){
                        MIN = DMEM[i].lastCycleUsed;
                        temp_ppn = i;
                    }
                }
            }
            DMEM[temp_ppn].lastCycleUsed = cycle;
            DMEM[temp_ppn].valid = 1;

            if(check){
                DPT[vpn].PPN = temp_ppn;
                DPT[vpn].valid = 1;
            }else{
                for(i = 0; i < D_page_table_entries; i++){
                    if(DPT[i].PPN == temp_ppn){
                        DPT[i].valid = 0;
                    }
                }
                DPT[vpn].PPN = temp_ppn;
                DPT[vpn].valid = 1;

                for(i = 0; i < D_TLB_entries; i++){
                    if(DTLB[i].PPN == temp_ppn){
                        DTLB[i].valid = 0;
                    }
                }

                for(j = 0; j < dMemPageSize; j++){
                    int pAddress = temp_ppn * dMemPageSize + j;
                    int pAddressBlock = pAddress/dCacheBlockSize;
                    int block = pAddressBlock % D_cache_entries;
                    int tag = pAddressBlock / D_cache_entries;
                    if(dCacheSet == 1){
                        if(DCA[block][0].tag == tag){
                            DCA[block][0].valid = 0;
                        }
                    }else{
                        for( i = 0; i < dCacheSet; i ++){
                            if(DCA[block][i].tag == tag && DCA[block][i].valid == 1){
                                DCA[block][i].valid = 0;
                                DCA[block][i].MRU = 0;
                            }
                        }
                    }
                }
            }

            MIN = 500001;
            int temp_address = 0;
            for(i = 0; i < D_TLB_entries; i++){
                if(DTLB[i].valid==0){
                    temp_address = i;
                    break;
                }else if(DTLB[i].lastCycleUsed<MIN){
                    MIN = DTLB[i].lastCycleUsed;
                    temp_address = i;
                }
            }

            DTLB[temp_address].lastCycleUsed = cycle;
            DTLB[temp_address].valid = 1;
            DTLB[temp_address].PPN = temp_ppn;
            DTLB[temp_address].VPN = vpn;


        }else{ //PT Hit!!
            dPT_hit++;
            int DLBMIN = 500001;
            int DLB_temp_Address = 0;
            int DLB_temp_ppn;
            DLB_temp_ppn = DPT[vpn].PPN;
            for(i = 0; i < D_TLB_entries; i++){
                if(DTLB[i].valid==0){
                    DLB_temp_Address = i;
                    break;
                }else if(DTLB[i].lastCycleUsed<DLBMIN){
                    DLBMIN = DTLB[i].lastCycleUsed;
                    DLB_temp_Address = i;
                }
            }
            DTLB[DLB_temp_Address].lastCycleUsed = cycle;
            DTLB[DLB_temp_Address].valid = 1;
            DTLB[DLB_temp_Address].PPN = DLB_temp_ppn;
            DTLB[DLB_temp_Address].VPN = vpn;
        }
    }else{  //TLB Hit!!
        dTLB_hit++;
    }
    //TLB HIT and get PPN
    // then check Cache Use CheckCA
    for(i = 0; i < D_TLB_entries; i++){
        if(DTLB[i].VPN == vpn && DTLB[i].valid == 1){
            DTLB[i].lastCycleUsed = cycle;
            Dppn = DTLB[i].PPN;
            break;
        }
    }
    checkCa = false ;
    //check Cache
    int CAPA = Dppn*dMemPageSize+page_offset;
    int CAPAB = CAPA/dCacheBlockSize;
    int block = CAPAB%D_cache_entries;
    int tag = CAPAB/D_cache_entries;
    int check = 0;
    int temp_j = 0;

    if(dCacheSet==1){
        if(tag==DCA[block][0].tag && DCA[block][0].valid==1){
            checkCa = true;
        }
    }else{
        for(i = 0; i<dCacheSet; i++){
            if(tag==DCA[block][i].tag && DCA[block][i].valid == 1){
                for( j = 0; j < dCacheSet; j++){
                    if(DCA[block][j].MRU==0){
                        if(check == 0){
                            temp_j = j;
                            check = 1;
                        }else{
                            check = -1;
                            break;
                        }
                    }
                }
                if(check == 1 && temp_j == i){
                    for(j = 0; j < dCacheSet; j++){
                        DCA[block][j].MRU = 0;
                    }
                }
                DCA[block][i].MRU = 1;
                checkCa = true;
            }
        }
    }
    if(checkCa==false){
        dCA_miss++;
        // Cache miss
        CAPA = Dppn*dMemPageSize+page_offset;
        CAPAB = CAPA/dCacheBlockSize;
        block = CAPAB%D_cache_entries;
        tag = CAPAB/D_cache_entries;
        check = 0;
        temp_j = 0;
        int checkM = 0;
        int get = 0;
        int temp_i = 0;
        if(dCacheSet==1){
            DCA[block][0].MRU = 0;
            DCA[block][0].tag = tag;
            DCA[block][0].valid = 1;
        }else{
            for(i = 0; i < dCacheSet; i++){
                if(DCA[block][i].valid==0){
                    temp_i = i;
                    check = 1;
                    break;
                }
            }
            for(i = 0; i < dCacheSet; i++){
                if(DCA[block][i].MRU ==0){
                    if(checkM==0){
                       get = i;
                       checkM = 1;
                    }else{
                        checkM = -1;
                        break;
                    }
                }
            }
            if(check==1){
                if(checkM==1&& temp_i ==  get){
                    for(i=0; i < dCacheSet; i++){
                        DCA[block][i].MRU = 0;
                    }
                }
                DCA[block][temp_i].MRU = 1;
                DCA[block][temp_i].tag = tag;
                DCA[block][temp_i].valid = 1;
            }else{
                if(checkM==1){
                    for(i = 0; i < dCacheSet; i++){
                        DCA[block][i].MRU = 0;
                    }
                }
                DCA[block][get].MRU = 1;
                DCA[block][get].tag = tag;
                DCA[block][get].valid = 1;
            }
        }


    }else{
        dCA_hit++;
    }


}
