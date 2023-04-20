#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>    //gST,gBS
#include <Library/UefiRuntimeServicesTableLib.h> //gRT

#include <Library/Keyboard.h>

#include <Protocol/SimplePointer.h>

// static EFI_SYSTEM_TABLE  *SystemTable;

// VOID myEventNoify(IN EFI_EVENT Event, IN VOID *Context);
// // EFI_STATUS TestNotify();
// // VOID myEventNoify(IN EFI_EVENT Event, IN VOID *Context, IN EFI_SYSTEM_TABLE  *SystemTable);
// EFI_STATUS TestNotify(IN EFI_SYSTEM_TABLE *SystemTable);
// EFI_STATUS TestTimer();
EFI_STATUS  testMouseSimple();
EFI_STATUS myTestEvent();
VOID myNotifyTest(IN EFI_EVENT Event, IN VOID *Context);


EFI_STATUS 
EFIAPI 
MyEventEntryPoint (
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
  Print(L"Hello,this is Entry of UefiMain!(Driver)\n");

  // TestNotify(SystemTable);
  // TestNotify();
  // TestTimer();
  // testMouseSimple();
   myTestEvent();

  return EFI_SUCCESS;
}

// Notification function
VOID myNotifyTest(IN EFI_EVENT Event, IN VOID *Context)
{
    EFI_STATUS   Status = 0;
//    UINTN Index = 0;
    EFI_INPUT_KEY  Key;
    Print(L"Wait...\t");
    // Status = gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &Index);
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);  
    Print(L"Event Status = %r\t",gBS->CheckEvent (Event));
    if (Status == EFI_SUCCESS)
      gBS->SignalEvent(Event); 
}
// creat event
EFI_STATUS myTestEvent()
{
  EFI_STATUS Status;
  EFI_EVENT  myEvent;  
//   UINTN index = 0;
  Print(L"\nEVT_NOTIFY_WAIT\n");
  Status = gBS->CreateEvent(
                  EVT_NOTIFY_WAIT, 
                  TPL_CALLBACK, 
                  (EFI_EVENT_NOTIFY)myNotifyTest, 
                  (VOID *)NULL, 
                  &myEvent
                  ); 
//   Status = gBS->WaitForEvent(1, &myEvent, &index);
  Status = gBS->CheckEvent (myEvent);
  Print (L"\nStatus = %r\n",Status); 

  gBS->CloseEvent(myEvent);

  return EFI_SUCCESS;
}

EFI_STATUS  testMouseSimple()
{
    EFI_STATUS  Status;
    EFI_SIMPLE_POINTER_PROTOCOL* mouse = 0;
    EFI_SIMPLE_POINTER_STATE     State;
    EFI_EVENT events[2]; // = {0, gST->ConIn->WaitForKey};
    // Show Cursor
    gST->ConOut->EnableCursor (gST->ConOut, TRUE);
    // find mouse devices
    Status = gBS->LocateProtocol(
            &gEfiSimplePointerProtocolGuid,
            NULL,
            (VOID**)&mouse
            );
    // reset mouse devices
    Status = mouse->Reset(mouse, TRUE);
    // put mouse event in events
    events[0] = mouse->WaitForInput;
    // put keyboard event in events
    events[1] = gST->ConIn->WaitForKey;
    while(1)
    {
        EFI_INPUT_KEY	   Key;
        UINTN index;
        // Wait for any event in the events to occur
        Status = gBS->WaitForEvent(2, events, &index);
        if(index == 0){
            // get mouse state and output
            Status = mouse->GetState(mouse, &State);
            Print(L"X:%d Y:%d Z:%d L:%d R:%d\n",
                State.RelativeMovementX,
                State.RelativeMovementY,
                State.RelativeMovementZ,
                State.LeftButton,
                State.RightButton
                );
        } else{            
            Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
            // enter 'q' to quit
            if (Key.UnicodeChar == 'q')
                break;
        }
    }
    return EFI_SUCCESS;
}

// // VOID myEventNoify(IN EFI_EVENT Event, IN VOID *Context, IN EFI_SYSTEM_TABLE  *SystemTable)
// VOID myEventNoify(IN EFI_EVENT Event, IN VOID *Context)
// {
//   static UINTN times = 0;
//   Print(L"VOID myEventNoify wait %d\n", times);
//   // Print(L"%s\n myEventNoify signal %d\n", Context, times);
//   times++;
//   if (times > 5)
//   {
//     // gBS->SignalEvent(Event); 
//     gST->BootServices->SignalEvent(Event);
//     // SystemTable->BootServices->SignalEvent(Event);
//   }
// }
// EFI_STATUS TestNotify(IN EFI_SYSTEM_TABLE *SystemTable)
// // EFI_STATUS TestNotify()
// {
//   EFI_STATUS Status;
//   EFI_EVENT myEvent;
//   Status = SystemTable->BootServices->CreateEvent(
//                                         EVT_NOTIFY_WAIT,
//                                         TPL_NOTIFY,
//                                         (EFI_EVENT_NOTIFY)myEventNoify,
//                                         (VOID *)NULL,
//                                         &myEvent
//                                         );
//   // Status = gBS->CreateEvent(EVT_NOTIFY_WAIT, TPL_NOTIFY, (EFI_EVENT_NOTIFY)myEventNoify, (VOID*)NULL, &myEvent);
//   UINTN index = 0;
//   Status = SystemTable->BootServices->WaitForEvent(1, &myEvent, &index);
//   // Status = gBS->WaitForEvent(1, &myEvent, &index);
//   return EFI_SUCCESS;
// }
// EFI_STATUS TestTimer()
// {
//   EFI_STATUS Status;
//   EFI_EVENT myEvent;
//   // UINTN index = 0;
//   Print(L"Test EVT_TIMER | EVT_NOTIFY_SIGNAL\n");
//   // creat EVT_TIMER event
//   // Status = gBS->CreateEvent(EVT_TIMER, TPL_CALLBACK, (EFI_EVENT_NOTIFY)NULL, (VOID *)NULL, &myEvent);
//   // Status = gBS->CreateEvent(EVT_TIMER | EVT_NOTIFY_SIGNAL, TPL_CALLBACK, (EFI_EVENT_NOTIFY)myEventNoify, (VOID *)NULL, &myEvent);
//   Status = gBS->CreateEvent(
//                   EVT_TIMER | EVT_NOTIFY_SIGNAL, 
//                   TPL_CALLBACK, 
//                   (EFI_EVENT_NOTIFY)myEventNoify, 
//                   (VOID *)L"Hello, Time Out!", 
//                   &myEvent
//                   );
//   // Set Timer Properties
//   Status = gBS->SetTimer(myEvent, TimerPeriodic, 10*1000*1000);
//   WaitKey();
//   // while(1)    // Continuously checking the status of event myEvent
//   //   Status = gBS->WaitForEvent(1, &myEvent, &index);

//   gBS->CloseEvent(myEvent);

//   return EFI_SUCCESS;
// }
