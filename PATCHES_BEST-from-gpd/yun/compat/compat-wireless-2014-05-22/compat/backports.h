#ifndef LINUX_BACKPORTS_PRIVATE_H
#define LINUX_BACKPORTS_PRIVATE_H

#include <linux/version.h>

#ifdef CPTCFG_BACKPORT_BUILD_DMA_SHARED_BUFFER
int __init dma_buf_init(void);
void __exit dma_buf_deinit(void);
#else
static inline int __init dma_buf_init(void) { return 0; }
static inline void __exit dma_buf_deinit(void) { }
#endif

#ifdef CPTCFG_BACKPORT_BUILD_CRYPTO_CCM
int crypto_ccm_module_init(void);
void crypto_ccm_module_exit(void);
#else
static inline int crypto_ccm_module_init(void)
{ return 0; }
static inline void crypto_ccm_module_exit(void)
{}
#endif

#endif /* LINUX_BACKPORTS_PRIVATE_H */
