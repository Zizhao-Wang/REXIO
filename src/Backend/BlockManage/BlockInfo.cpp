/**
 * @date 2022.6.17 0:08
 * @author zz.wang
 * @details
 *          This file implemented the management of blocks in the SSD, including block information, page information, rate of
 *              used blocks or pages.
 *
 *
 *
 *         IDENTIFICATION
 *   Experiment1/Backend/BlockManage/BlockInfo.cpp
 */

#include "BlockInfo.h"
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <liblightnvm.h>
#include <liblightnvm_cli.h>


int vblk_ex01_ewr(struct nvm_bp *bp)
{
	const int nchunks = bp->naddrs;
	struct nvm_addr chunk_addrs[nchunks];
	struct nvm_vblk *vblk;
	ssize_t err;
	size_t diff;

	printf("# VBLK EWR example 'nchunks: %d' addresses\n",
	       nchunks);
	err = nvm_cmd_rprt_arbs(bp->dev, NVM_CHUNK_STATE_FREE, nchunks,
				chunk_addrs);
	if (err) {
		perror("nvm_cmd_rprt_arbs");
		return -1;
	}

	vblk = nvm_vblk_alloc(bp->dev, chunk_addrs, nchunks);		// ALLOC
	if (!vblk) {
		perror("nvm_vblk_alloc");
		return -1;
	}

	printf("# vblk: write\n");
	nvm_cli_timer_start();
	err = nvm_vblk_write(vblk, bp->bufs->write, bp->bufs->nbytes);	// WRITE
	if (err < 0) {
		perror("nvm_vblk_write");
		return -1;
	}
	nvm_cli_timer_stop();
	nvm_cli_timer_bw_pr("nvm_vblk_write", bp->bufs->nbytes);

	printf("# vblk: read\n");
	nvm_cli_timer_start();
	err = nvm_vblk_read(vblk, bp->bufs->read, bp->bufs->nbytes);	// READ
	if (err < 0) {
		perror("nvm_vblk_read");
		return -1;
	}
	nvm_cli_timer_stop();
	nvm_cli_timer_bw_pr("nvm_cmd_read", bp->bufs->nbytes);

	printf("# vblk: erase\n");
	nvm_cli_timer_start();
	err = nvm_vblk_erase(vblk);					// ERASE
	if (err < 0) {
		perror("nvm_vblk_erase");
		return -1;
	}
	nvm_cli_timer_stop();
	nvm_cli_timer_bw_pr("nvm_vblk_erase", bp->bufs->nbytes);

	nvm_vblk_free(vblk);						// FREE

	// Sanity check: did we actually read from device
	diff = nvm_buf_diff(bp->bufs->write, bp->bufs->read,
				   bp->bufs->nbytes);
	if (diff) {
		nvm_buf_diff_pr(bp->bufs->write, bp->bufs->read,
				bp->bufs->nbytes);
		errno = EIO;
		return -1;
	}

	return 0;
}






