// Main driver object
typedef struct _DRIVER_OBJECT
{
    struct _DEVICE_OBJECT  *DeviceObject;     // Pointer to first device in list
    int                     MajorID;          // The OS-assigned driver Major ID
    uint8_t                 DeviceCount;      // Number of devices in list
    //PLX_PHYS_MEM_OBJECT     CommonBuffer;     // Contiguous memory to be shared by all processes
    struct file_operations  DispatchTable;    // Driver dispatch table

    struct cdev             cdev;             // Char device structure
    struct class           *devclass;
} DRIVER_OBJECT;
