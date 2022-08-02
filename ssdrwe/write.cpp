#include <time.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <liblightnvm.h>
#include <liblightnvm_spec.h>
#include <liblightnvm_cli.h>
using namespace std;

int sync_ex01_ewr_prp(struct nvm_bp *bp)
{
	const size_t nchunks = bp->naddrs;
	struct nvm_addr chunk_addrs[nchunks];
	size_t ws_opt = nvm_dev_get_ws_min(bp->dev);
	size_t diff;
	int err;
	//struct nvm_addr chunk_addrs[0] = chunk_addrs[0];


	 uint64_t Pagedata[2050]; 
	 for(uint64_t i=1;i<2049;i++)
	 {
		Pagedata[i-1]=i;
	 }
	 char * tem =  (char *)Pagedata;
	 for(int i=0;i<2048*8;i++)
	 {
		bp->bufs->write[i]=tem[i];
	 }
	
	cout<<endl;
	printf("Pages: %lu blocks: %lu Sectors: %lu Planes: %lu\n", chunk_addrs[0].g.pg, chunk_addrs[0].g.blk,chunk_addrs[0].g.sec,chunk_addrs[0].g.pl);
	printf("chunk: %lu pgrup: %lu Punit: %lu Sectors: %lu  %lu %lu\n", chunk_addrs[0].l.chunk, chunk_addrs[0].l.pugrp,chunk_addrs[0].l.punit,chunk_addrs[0].l.sectr, chunk_addrs[0].ppa, chunk_addrs[0].val);
	cout<<endl;
	chunk_addrs[0] = nvm_addr_dev2gen(bp->dev,0);
	//printf("of bytes per data buffer: %lu\n", bp->bufs->nbytes);
	cout<<endl;
	printf("Pages: %lu blocks: %lu Sectors: %lu Planes: %lu\n", chunk_addrs[0].g.pg, chunk_addrs[0].g.blk,chunk_addrs[0].g.sec,chunk_addrs[0].g.pl);
	printf("chunk: %lu pgrup: %lu Punit: %lu Sectors: %lu  %lu %lu\n", chunk_addrs[0].l.chunk, chunk_addrs[0].l.pugrp,chunk_addrs[0].l.punit,chunk_addrs[0].l.sectr, chunk_addrs[0].ppa, chunk_addrs[0].val);
	cout<<endl;

	//
	//uint64_t add = nvm_addr_gen2dev(bp->dev,chunk_addrs[0]);
	//printf(" %lu\n", add);

	printf("# nvm_cmd EWR example 'nchunks: %zu' addresses\n", nchunks);
	//printf("# nvm_cmd EWR example 'nchunks: %lu' addresses\n",);

	err = nvm_cmd_rprt_arbs(bp->dev, NVM_CHUNK_STATE_FREE, nchunks,chunk_addrs);
	
	if (err) {
		perror("nvm_cmd_rprt_arbs");
		return -1;
	}

	
	cout<<endl;
	printf("Test Pages: %lu blocks: %lu Sectors: %lu Planes: %lu nchunks:%ld\n", chunk_addrs[0].g.pg, chunk_addrs[0].g.blk,chunk_addrs[0].g.sec,chunk_addrs[0].g.pl, nchunks);
	printf("Test chunk: %lu pgrup: %lu Punit: %lu Sectors: %lu  %lu %lu\n", chunk_addrs[0].l.chunk, chunk_addrs[0].l.pugrp,chunk_addrs[0].l.punit,chunk_addrs[0].l.sectr, chunk_addrs[0].ppa, chunk_addrs[0].val);
	cout<<endl;

	//g++ write.cpp -fopenmp -llightnvm_cli -llightnvm -o write 
	//g++ example.cpp -fopenmp -llightnvm_cli -llightnvm -o example 


	

	//printf("nsectr: %lu  nbytes:%lu minimal sectors：%d optimal sectors：%d\n",bp->geo->l.nsectr,bp->geo->l.nbytes,nvm_dev_get_ws_min(bp->dev),nvm_dev_get_ws_opt(bp->dev));

	printf("# nvm_cmd_write\n");
	nvm_cli_timer_start();


		//const size_t ofz = cidx * bp->geo->l.nsectr * bp->geo->l.nbytes;
		//printf("ofz :%lu\n",ofz);

		// 这个chunk的最佳要写入的sector数量，循环8次
			//size_t buf_ofz = sectr * bp->geo->l.nbytes + ofz;
			struct nvm_addr addrs[ws_opt];
			//printf("buf_ofz: %lu ws_opt: %lu\n",buf_ofz,ws_opt);

		    //8次循环 8个地址
			for (size_t aidx = 0; aidx < ws_opt; ++aidx) {
				addrs[aidx].val = chunk_addrs[0].val;
				printf("chunk_addrs[0].val: %ld\n", chunk_addrs[0].val);
				addrs[aidx].l.sectr = aidx;
				//if(cidx == 1)
					//printf("aidx: %lu addrs[aidx].val : %lu chunk_addrs[cidx].val %lu addrs[aidx].l.sectr %lu \n",aidx,addrs[aidx].val,chunk_addrs[cidx].val,addrs[aidx].l.sectr);
			}

			printf("=========================");

			err = nvm_cmd_write(bp->dev, addrs, ws_opt, bp->bufs->write, NULL,0x0, NULL);
			//err = nvm_cmd_write(bp->dev, addrs, ws_opt,bp->bufs->write + buf_ofz, NULL,0x0, NULL);

			//printf("write number: %lu \n", cidx);
			err = nvm_cmd_erase(bp->dev, chunk_addrs, nchunks, NULL, 0x0, NULL);
			//char * data = new char;
			err = nvm_cmd_read(bp->dev, addrs, ws_opt,bp->bufs->read, NULL,0x0, NULL);
			if (err) {
				perror("nvm_cmd_write");
				return -1;
			}

			
			//uint64_t *ml = (uint64_t *)bp->bufs->read;
			//for(int i=1;i<2048;i++)
	 		//{
			//	printf("\n shuchu %ld ",ml[i]);  
	 		//}
		
	
	//nvm_cli_timer_stop();
	//nvm_cli_timer_bw_pr("nvm_cmd_write", bp->bufs->nbytes);

	return 0;
}

int main(int argc, char **argv)
{
	struct nvm_bp *bp;
	int err = EXIT_FAILURE;
	//printf("%d %s gfhgfhngfnfngfnfgnfgngngfngfnfngngfngfnfnfnn\n", argc, argv[0]);

	bp = nvm_bp_init_from_args(argc, argv);
	if (!bp) {
		perror("nvm_bp_init");
		return err;
	}

	err = sync_ex01_ewr_prp(bp);
	if (err) {
		perror("sync_ex01_ewr_prp");
		err = EXIT_FAILURE;
	}

	nvm_bp_term(bp);
	return err;
}