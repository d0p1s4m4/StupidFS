#include <ntddk.h>
#include <ntifs.h>
#include <wdf.h>
#include <ntddscsi.h>
#include <scsi.h>
#include <ntddcdrm.h>
#include <ntdddisk.h>
#include <ntddstor.h>
#include <ntintsafe.h>

typedef struct _VOLUME_DEVICE_OBJECT {
	DEVICE_OBJECT DeviceObject;
} VOLUME_DEVICE_OBJECT;

typedef VOLUME_DEVICE_OBJECT *PVOLUME_DEVICE_OBJECT;

_Function_class_(IRP_MJ_CREATE)
_Function_class_(DRIVER_DISPATCH)
NTSTATUS
StpdFsdCreate(_In_ PVOLUME_DEVICE_OBJECT VolumeDeviceObject, _Inout_ PIRP Irp)
{
	return (0);
}