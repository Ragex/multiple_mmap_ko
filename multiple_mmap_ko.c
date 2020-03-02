#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include "multiple_mmap_ko.h"


#define DRIVER_NAME         "Multiple_mmap"
#define DRIVER_DESCRIPTION  "Allocate dma_alloc_noncoherent buffer & check multiple mmap calls"

#define InfoPrintf(arg)       _Info_Print_Macro   arg
#define ErrorPrintf(arg)      _Error_Print_Macro  arg
#define DebugPrintf(arg)      _Debug_Print_Macro      arg
#define DebugPrintf_Cont(arg) _Debug_Print_Cont_Macro arg

#define _Info_Print_Macro(fmt, args...)       printk(KERN_INFO  DRIVER_NAME ": " fmt, ## args)
#define _Error_Print_Macro(fmt, args...)      printk(KERN_ERR   DRIVER_NAME ": " fmt, ## args)
#define _Debug_Print_Macro(fmt, args...)      printk(KERN_DEBUG DRIVER_NAME ": " fmt, ## args)
#define _Debug_Print_Cont_Macro(fmt, args...) printk(KERN_CONT  fmt, ## args)


#define DEVICE_FIRST  0
#define DEVICE_COUNT  1
#define DEVICE_NAME   "mult_mmap"

#define  BUFF_SIZE (100 * 1024 * 1024)


// Pointer to the main driver object
DRIVER_OBJECT *pGbl_DriverObject;

void mod_exit( void );

static inline void *dma_alloc_noncoherent(struct device *dev, size_t size,
                                          dma_addr_t *dma_handle, gfp_t gfp)
{
    unsigned long attrs = DMA_ATTR_NON_CONSISTENT;
    return dma_alloc_attrs(dev, size, dma_handle, gfp, attrs);
}


int Dispatch_mmap( struct file *filp, struct vm_area_struct *vma )
{
    int               rc;
    off_t             offset;
    // uint64_t          AddressToMap;
    // DEVICE_EXTENSION *pdx;


    DebugPrintf_Cont(("\n"));
    DebugPrintf(("Received message ===> MMAP\n"));
    DebugPrintf(("mmap: vm_start:    0x%lx\n", vma->vm_start));
    DebugPrintf(("mmap: vm_vm_pgoff: 0x%lx\n", vma->vm_pgoff));
    DebugPrintf(("mmap: vm_end:      0x%lx\n", vma->vm_end));
    DebugPrintf(("mmap: size:        0x%lx\n", vma->vm_end - vma->vm_start));

    rc = 0;

    // Get device extension
    //pdx = ((DEVICE_OBJECT*)(filp->private_data))->DeviceExtension;

    // Get the supplied offset
    offset = vma->vm_pgoff;






    return rc;
}


long Dispatch_IoControl( struct file *filp, unsigned int cmd, unsigned long args )
{
  DRV_PARAMS  IoBuffer;
  DRV_PARAMS *pIoBuffer;
  int status;


  // Copy the I/O Control message from user space
  status = copy_from_user(&IoBuffer, (DRV_PARAMS*)args, sizeof(DRV_PARAMS));
  if (status != 0) {
    ErrorPrintf(("ERROR - Unable to copy user I/O message data\n"));
    return (-EFAULT);
  }

  pIoBuffer = &IoBuffer;


  DebugPrintf(("...Completed message\n"));

  status = copy_to_user((DRV_PARAMS*)args, pIoBuffer, sizeof(DRV_PARAMS));
  return status;
}



int __init mod_init( void )
{
  int         status;
  dev_t       dev;
  dma_addr_t  BusAddress;


  InfoPrintf(("Start Multiple_mmap driver\n"));

	// Allocate memory for the Driver Object
	pGbl_DriverObject = kmalloc(sizeof(DRIVER_OBJECT), GFP_KERNEL);
	if (pGbl_DriverObject == NULL) {
			ErrorPrintf(("ERROR - memory allocation for Driver Object failed\n"));
			return (-ENOMEM);
	}

  // Fill in the appropriate dispatch handlers
  pGbl_DriverObject->DispatchTable.owner = THIS_MODULE;
  pGbl_DriverObject->DispatchTable.mmap  = Dispatch_mmap;

  // Use newer IOCTL functions
  pGbl_DriverObject->DispatchTable.unlocked_ioctl = Dispatch_IoControl;
  pGbl_DriverObject->DispatchTable.compat_ioctl   = Dispatch_IoControl;

  // Create char device
  status = alloc_chrdev_region(&dev, DEVICE_FIRST, DEVICE_COUNT, DRIVER_NAME);
  pGbl_DriverObject->MajorID = MAJOR( dev );

  DebugPrintf(("Registered driver (MajorID = %03d)\n", pGbl_DriverObject->MajorID));

  if (pGbl_DriverObject->MajorID < 0) {
      status = pGbl_DriverObject->MajorID;
      ErrorPrintf(("ERROR: alloc_chrdev_region() return the error (status=%d)\n", status));
      mod_exit();
      return status;
  }

  cdev_init(&(pGbl_DriverObject->cdev), &(pGbl_DriverObject->DispatchTable));
  pGbl_DriverObject->cdev.owner = THIS_MODULE;
  pGbl_DriverObject->cdev.ops = &pGbl_DriverObject->DispatchTable;

  status = cdev_add(&(pGbl_DriverObject->cdev), DEVICE_FIRST, DEVICE_COUNT);

  // Allocate phys buffer
  // pGbl_DriverObject->pKernelVa = dma_alloc_noncoherent(, BUFF_SIZE,
  //                                               &BusAddress,
  //                                               GFP_KERNEL | __GFP_NOWARN);
  //
  // if (pGbl_DriverObject->pKernelVa == NULL) {
  //   mod_exit();
  //   return -1;
  // }


  InfoPrintf(("...driver loaded\n\n"));
  return 0;
}


void mod_exit( void )
{
  // Release the buffer
  if (pGbl_DriverObject->pKernelVa) {
    // dma_free_coherent(pdx, BUFF_SIZE, pGbl_DriverObject->pKernelVa,
    //                   (dma_addr_t)pMemObject->BusPhysical);
  }

  cdev_del(&(pGbl_DriverObject->cdev));

  // Release driver object
	kfree( pGbl_DriverObject );
	pGbl_DriverObject = NULL;

	InfoPrintf(("...driver unloaded\n"));
}


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(DRIVER_DESCRIPTION);
module_init(mod_init);
module_exit(mod_exit);
