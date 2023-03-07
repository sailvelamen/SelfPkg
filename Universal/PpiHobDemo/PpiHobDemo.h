#include <PiPei.h>
#include <Library/PeimEntryPoint.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
// #include <Library/PeiServicesTablePointerLib.h>

#include <Guid/MyHob.h>

typedef EFI_STATUS (EFIAPI *SELF_TEST_FUNCTION)(VOID);

typedef struct _SELF_TEST_INIT_PPI{
  SELF_TEST_FUNCTION    Self_Test;
}SELF_TEST_INIT_PPI;

EFI_GUID gSelfTestInitFunctionPpiGuid = { 0x8A094F61, 0x4B1C, 0x1E0B, { 0x08, 0x5D, 0x99, 0x9D, 0xF5, 0xDD, 0xBC, 0xF4 }};