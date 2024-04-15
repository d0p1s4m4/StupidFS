#include <ntddk.h>
#include <wdf.h>

DRIVER_INITIALIZE DriverEntry;

PDEVICE_OBJECT StpdDiskFileSystemDeviceObject;

VOID
StpdUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);

    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "StupidFS: Driver unload\n"));
}

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
#pragma prefast(pop)
    return status;
}
