#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <liblightnvm.h>
#include <liblightnvm_spec.h>
#include <liblightnvm_cli.h>


int sync_ex01_ewr_prp(struct nvm_bp *bp)
{
	int err =0;
	struct nvm_addr addrs_chunk = nvm_addr_dev2gen(bp->dev, 0);
    size_t ws_min = nvm_dev_get_ws_min(bp->dev);
    struct nvm_addr addrs[ws_min];

    for (size_t aidx = 0; aidx < ws_min; ++aidx) 
    {
		addrs[aidx].val = addrs_chunk.val;
		addrs[aidx].l.sectr =0+ aidx;
	}
	err = nvm_cmd_read(bp->dev, addrs, ws_min,bp->bufs->read, NULL,0x0, NULL);

	char * temp = new char[20];
    for (size_t i = 0; i < 1024; i++)
    {
        for(size_t j = i*16,k=0;j<i*16+16;j++,k++)
        {
            temp[k] = bp->bufs->read[j];
        }
        uint64_t *ML = (uint64_t*) temp;
		uint64_t a = ML[0];
		uint64_t b = ML[1];
        printf("%lu %lu \n",a,b);
    }

	return 0;
}

int main(int argc, char **argv)
{
	struct nvm_bp *bp;
	int err = EXIT_FAILURE;

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
