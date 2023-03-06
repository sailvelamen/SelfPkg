/** @file
  This is Sample of UEFI aplication. 
**/
#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>
#include  <Library/MyLibrary.h>
//#include  <../../Library/MyLibrary/MyLibrary.h>
#include <stdio.h>

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
int main (IN int Argc, IN char **Argv)
{
  printf("MyLibApp...\n");
  LibFunction();

  return 0;
}
