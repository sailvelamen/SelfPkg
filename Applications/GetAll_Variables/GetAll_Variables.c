#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/ShellCEntryLib.h>
// #include <Library/UefiBootServicesTableLib.h>    //gST,gBs
#include <Library/UefiRuntimeServicesTableLib.h> //gRT

#include <stdio.h>
#include <stdlib.h>

#include <Library/MyLibrary.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/GlobalVariable.h>
/***
  Demonstrates basic workings of the main() function by displaying a
  welcoming message.

  Note that the UEFI command line is composed of 16-bit UCS2 wide characters.
  The easiest way to access the command line parameters is to cast Argv as:
      wchar_t **wArgv = (wchar_t **)Argv;

  @param[in]  Argc    Number of argument tokens pointed to by Argv.
  @param[in]  Argv    Array of Argc pointers to command line tokens.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/

#define INIT_NAME_BUFFER_SIZE 128
#define INIT_DATA_BUFFER_SIZE 1024

VOID PrintDataBuffer(UINT8 *DataBuffer, UINTN DataBufferSize);

int main(IN int Argc, IN char **Argv)
{   
    UINTN NameBufferSize = INIT_NAME_BUFFER_SIZE;
    // 变量名分配内存，初始化为0
    CHAR16 *FoundVarName = AllocateZeroPool(NameBufferSize);
    if (FoundVarName == NULL)
    {
        printf("FoundVarName == NULL, != 0 \n");
        return (EFI_OUT_OF_RESOURCES);
    }
    UINTN DataBufferSize = INIT_DATA_BUFFER_SIZE;
    UINT8 *DataBuffer = AllocatePool(DataBufferSize);
    if(DataBuffer == NULL)
    {// DataBuffer == NULL 说明没有分配成功需要释放FoundVarName
        printf("DataBuffer == NULL\n");
        FreePool(FoundVarName);
        return (EFI_OUT_OF_RESOURCES);
    }

    EFI_STATUS Status;
    UINTN NameSize, OldNameBufferSize, DataSize;
    CHAR16 *OldName;
    UINT32 Atts;
//    BOOLEAN write;

    printf("------------------------------GetAllVariables-----------------------------\n");
    EFI_GUID FoundVarGuid;
    while(1)
    {
        NameSize = NameBufferSize;
        Status = gRT->GetNextVariableName(&NameSize, FoundVarName, &FoundVarGuid);
        if(Status == EFI_BUFFER_TOO_SMALL)
        {
            OldName = FoundVarName;
            OldNameBufferSize = NameBufferSize;
            NameBufferSize = NameSize > NameBufferSize*2 ? NameSize : NameBufferSize*2;
            FoundVarName = AllocateZeroPool(NameBufferSize);
            if (FoundVarName == NULL)
            {
                Status = EFI_OUT_OF_RESOURCES;
                FreePool(OldName);
                break;
            }
            CopyMem(FoundVarName, OldName, OldNameBufferSize);
            FreePool(OldName);
            NameSize = NameBufferSize;
            Status = gRT->GetNextVariableName(&NameSize, FoundVarName, &FoundVarGuid);
        }
        if(Status == EFI_NOT_FOUND)
            break;
        Print(L"Variable:%-22s", FoundVarName);
        DataSize = DataBufferSize;
        Status = gRT->GetVariable(FoundVarName, &FoundVarGuid, &Atts, &DataSize, DataBuffer);
        if(Status == EFI_BUFFER_TOO_SMALL)
        {
            OldName = FoundVarName;
            OldNameBufferSize = NameBufferSize;
            NameBufferSize = NameSize > NameBufferSize*2 ? NameSize : NameBufferSize*2;
            FoundVarName = AllocateZeroPool(NameBufferSize);
            if (FoundVarName == NULL)
            {
                Status = EFI_OUT_OF_RESOURCES;
                FreePool(OldName);
                break;
            }
            CopyMem(FoundVarName, OldName, OldNameBufferSize);
            FreePool(OldName);
            NameSize = NameBufferSize;
            Status = gRT->GetVariable(FoundVarName, &FoundVarGuid, &Atts, &DataSize, DataBuffer);
        }
        printf(" Attributes:0x%08x  DataSize: 0x%03x  \n", Atts, DataSize);
        printf("GUID: ");
        PrintGuid(&FoundVarGuid);

        // Status = gRT->SetVariable(FoundVarName, &FoundVarGuid, Atts, DataSize, DataBuffer);
        // if(Status == EFI_SECURITY_VIOLATION)
        //     write = FALSE;
        // (write == FALSE) ? printf("False\n") : printf("True\n");
    }//End while(1)


    printf("------------------------------GetVariable-----------------------------\n");
    FoundVarName = (CHAR16*)L"ConIn";
    EFI_GUID *Guid = &gEfiGlobalVariableGuid;
    Status = gRT->GetVariable(FoundVarName, Guid, &Atts, &DataSize, DataBuffer);
    if(Status == EFI_BUFFER_TOO_SMALL)
    {
        OldName = FoundVarName;
        OldNameBufferSize = NameBufferSize;
        NameBufferSize = NameSize > NameBufferSize*2 ? NameSize : NameBufferSize*2;
        FoundVarName = AllocateZeroPool(NameBufferSize);
        if (FoundVarName == NULL)
        {
            Status = EFI_OUT_OF_RESOURCES;
            FreePool(OldName);
        }
        CopyMem(FoundVarName, OldName, OldNameBufferSize);
        FreePool(OldName);
        NameSize = NameBufferSize;
        Status = gRT->GetVariable(FoundVarName, Guid, &Atts, &DataSize, DataBuffer);
    }
    DataBuffer[0] = 1;
    gRT->SetVariable(FoundVarName, Guid, Atts, DataSize, DataBuffer);

    Print(L"Variable:%-22s", FoundVarName);
    printf(" Attributes:0x%08x  DataSize: 0x%03x  \n", Atts, DataSize);
    printf("GUID: ");
    PrintGuid(Guid);
    PrintDataBuffer(DataBuffer, DataSize);

    // printf("------------------------------SetVariable-----------------------------\n");
    // FoundVarName = (CHAR16*)L"TestVariable";
    // //E8597DEF-C5EF-4C07-860C-EA864553F186
    // EFI_GUID TestGuid = { 0xE8597DEF, 0xC5EF, 0x4C07, { 0x86, 0x0C, 0xEA, 0x86, 0x45, 0x53, 0xF1, 0x86 }};
    // DataSize = 5;
    // DataBuffer = AllocatePool(DataBufferSize);
    // DataBuffer[0] = 1;DataBuffer[1] = 2;DataBuffer[2] = 3;DataBuffer[3] = 4;DataBuffer[4] = 5;
    // Atts = EFI_VARIABLE_APPEND_WRITE;
    // Status = gRT->SetVariable(FoundVarName, &TestGuid, Atts, DataSize, DataBuffer);
    // Print(L"Variable:%-22s", FoundVarName);
    // printf(" Attributes:0x%08x  DataSize: 0x%03x  \n", Atts, DataSize);
    // printf("GUID: ");
    // PrintGuid(&TestGuid);
    // PrintDataBuffer(DataBuffer, DataSize);

    return 0;
}

VOID PrintDataBuffer(UINT8 *DataBuffer, UINTN DataBufferSize)
{
    printf("*Data = ");
    for(int i = 0; i < DataBufferSize; i++)
    {
        printf("%02x ",DataBuffer[i]);
        if ((i != 0) && ((i+1) % 16 == 0))
            printf("\n\t");
    }
    printf("\n");
}