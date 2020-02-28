#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>

#define DRIVER_NAME         "Multiple_mmap"
#define DRIVER_DESCRIPTION  "Allocate dma_alloc_noncoherent buffer & check multiple mmap calls"

#define InfoPrintf(arg)   _Info_Print_Macro   arg
#define ErrorPrintf(arg)  _Error_Print_Macro  arg

#define _Info_Print_Macro(fmt, args...)   printk(KERN_INFO  DRIVER_NAME ": " fmt, ## args)
#define _Error_Print_Macro(fmt, args...)  printk(KERN_ERR   DRIVER_NAME ": " fmt, ## args)

#define  BUFF_LEN  (100 * 1024 * 1024)

// #define  PRINT_BUFF        InfoPrintf((">> ===== pBuff =====\n"));     \
// 			   for(i = 0; i < 20; i++)                                  \
// 				InfoPrintf((">> pBuff[%d]:   0x%X\n", i, pBuff[i]));\
// 			   InfoPrintf((">> pBuff[125]: 0x%X\n", pBuff[125]));       \
// 			   InfoPrintf((">> pBuff[255]: 0x%X\n", pBuff[255]));       \
// 			   InfoPrintf((">> =================\n"));


// Pointer to the main driver object
DRIVER_OBJECT *pGbl_DriverObject;





int __init module_init(void)
{

  InfoPrintf(("Start Multiple_mmap driver\n"));

	// Allocate memory for the Driver Object
	pGbl_DriverObject = kmalloc(sizeof(DRIVER_OBJECT), GFP_KERNEL);
	if (pGbl_DriverObject == NULL) {
			ErrorPrintf(("ERROR - memory allocation for Driver Object failed\n"));
			return (-ENOMEM);
	}


    return 0;
}


void __exit module_exit(void)
{
	// Release driver object
	kfree( pGbl_DriverObject );
	pGbl_DriverObject = NULL;

	InfoPrintf(("...driver unloaded\n"));
}


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(DRIVER_DESCRIPTION);
module_init(module_init);
module_exit(module_exit);
