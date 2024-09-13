/**
 * \addtogroup driver
 * @{
 */
#include <ntddk.h>
#include <wdf.h>

/**
 * \defgroup win32 Windows Driver
 * @{
 */

DRIVER_INITIALIZE DriverEntry;

PDEVICE_OBJECT StpdDiskFileSystemDeviceObject;

/**
 * Unload routine for StpdFS.
 * 
 * \param DriverObject pointer to driver object.
 */
VOID
StpdUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);

    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "StupidFS: Driver unload\n"));
}

/**
 * Initialization routine for the StupidFS device driver.
 *
 * \param DriverObject Pointer to driver object
 * \param RegistryPath XXX
 * 
 * \return status
 */
NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT     DriverObject,
    _In_ PUNICODE_STRING    RegistryPath
)
{
    UNICODE_STRING ustr;
    NTSTATUS status;

    UNREFERENCED_PARAMETER(RegistryPath);

    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "StupidFS: DriverEntry\n"));

    RtlInitUnicodeString(&ustr, L"\\Stpd");
    status = IoCreateDevice(DriverObject, 0, &ustr,
        FILE_DEVICE_DISK_FILE_SYSTEM, 0, FALSE,
        &StpdDiskFileSystemDeviceObject);

    if (!NT_SUCCESS(status))
    {
        return status;
    }
#pragma prefast(push)
#pragma prefast(disable:28155, "these are all correct")
#pragma prefast(disable:28169, "these are all correct")
#pragma prefast(disable:28175, "this is a filesystem, touching FastIoDispatch is allowed")
    DriverObject->DriverUnload = StpdUnload;

    DriverObject->MajorFunction[IRP_MJ_CREATE] = NULL;
#pragma prefast(pop)
    return status;
}

/** @} @} */