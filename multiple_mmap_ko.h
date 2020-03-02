#include <linux/fs.h>
#include <linux/cdev.h>

typedef uint8_t BOOLEAN;

// Information about contiguous, page-locked buffers
typedef struct _PHYS_MEM_OBJECT
{
    struct list_head  ListEntry;
    void             *pOwner;
    uint8_t          *pKernelVa;
    uint64_t          CpuPhysical;            // CPU Physical Address
    uint64_t          BusPhysical;            // Bus Physical Address
    uint32_t          Size;                   // Buffer size
} PHYS_MEM_OBJECT;


// Used to pass IOCTL arguments down to the driver
typedef struct _DRV_PARAMS
{
    int     ReturnCode;      // API status code
    // PLX_DEVICE_KEY Key;             // Device key information
    uint64_t       value[3];        // Generic storage for parameters
    uint64_t       ExData[5];
} DRV_PARAMS;


// Main driver object
typedef struct _DRIVER_OBJECT
{
    struct _DEVICE_OBJECT  *DeviceObject;     // Pointer to first device in list
    int                     MajorID;          // The OS-assigned driver Major ID
    uint8_t                 DeviceCount;      // Number of devices in list
    PHYS_MEM_OBJECT         DmaBuffer;        // Contiguous memory to be shared by all processes
    struct file_operations  DispatchTable;    // Driver dispatch table

    uint8_t                *pKernelVa;

    struct cdev             cdev;             // Char device structure
    struct class           *devclass;
} DRIVER_OBJECT;
