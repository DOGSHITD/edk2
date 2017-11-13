/** @file
  Provide FSP API related function.

  Copyright (c) 2014 - 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiPei.h>

#include <Library/FspWrapperApiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

/**
  Wrapper for a thunk  to transition from long mode to compatibility mode to execute 32-bit code and then transit back to
  long mode.

  @param[in] Function     The 32bit code entry to be executed.
  @param[in] Param1       The first parameter to pass to 32bit code.
  @param[in] Param2       The second parameter to pass to 32bit code.

  @return EFI_STATUS.
**/
EFI_STATUS
Execute32BitCode (
  IN UINT64      Function,
  IN UINT64      Param1,
  IN UINT64      Param2
  );

/**
  Find FSP header pointer.

  @param[in] FlashFvFspBase Flash address of FSP FV.

  @return FSP header pointer.
**/
FSP_INFO_HEADER *
EFIAPI
FspFindFspHeader (
  IN EFI_PHYSICAL_ADDRESS  FlashFvFspBase
  )
{
  EFI_FIRMWARE_VOLUME_HEADER      *FvHeader;
  EFI_FIRMWARE_VOLUME_EXT_HEADER  *FvExt;
  UINT8                           *CheckPointer;
  UINTN                           Offset;
  FSP_INFO_HEADER                 *FspInfoHeader;

  FvHeader = (EFI_FIRMWARE_VOLUME_HEADER *) (UINTN) FlashFvFspBase;
  CheckPointer = (UINT8 *) (UINTN) FlashFvFspBase;
  Offset = 0;

  if (FvHeader->Signature != EFI_FVH_SIGNATURE) {
    return NULL;
  }

  if (FvHeader->ExtHeaderOffset != 0) {
    FvExt = (EFI_FIRMWARE_VOLUME_EXT_HEADER *)((UINT8 *)FvHeader + FvHeader->ExtHeaderOffset);
    Offset = (UINTN)FvHeader->ExtHeaderOffset + (UINTN)FvExt->ExtHeaderSize;
  } else {
    Offset = (UINTN)FvHeader->HeaderLength;
  }
  Offset = (UINTN) ALIGN_POINTER (Offset, 8);

  Offset = Offset + sizeof (EFI_FFS_FILE_HEADER);

  if (((EFI_RAW_SECTION *)(CheckPointer + Offset))->Type != EFI_SECTION_RAW) {
    return NULL;
  }

  Offset = Offset + sizeof (EFI_RAW_SECTION);

  if (Offset != FSP_INFO_HEADER_OFF) {
    return NULL;
  }

  FspInfoHeader = (FSP_INFO_HEADER *)(CheckPointer + Offset);
  if (FspInfoHeader->Signature != FSP_INFO_HEADER_SIGNATURE) {
    return NULL;
  }

  return FspInfoHeader;
}

/**
  Call FSP API - FspNotifyPhase.

  @param[in] NotifyPhaseParams Address pointer to the NOTIFY_PHASE_PARAMS structure.

  @return EFI status returned by FspNotifyPhase API.
**/
EFI_STATUS
EFIAPI
CallFspNotifyPhase (
  IN NOTIFY_PHASE_PARAMS *NotifyPhaseParams
  )
{
  FSP_INFO_HEADER     *FspHeader;
  FSP_NOTIFY_PHASE    NotifyPhaseApi;
  EFI_STATUS          Status;
  BOOLEAN             InterruptState;

  FspHeader = (FSP_INFO_HEADER *)FspFindFspHeader (PcdGet32 (PcdFspsBaseAddress));
  if (FspHeader == NULL) {
    return EFI_DEVICE_ERROR;
  }

  NotifyPhaseApi = (FSP_NOTIFY_PHASE)((UINTN)FspHeader->ImageBase + FspHeader->NotifyPhaseEntryOffset);
  InterruptState = SaveAndDisableInterrupts ();
  Status = Execute32BitCode ((UINTN)NotifyPhaseApi, (UINTN)NotifyPhaseParams, (UINTN)NULL);
  SetInterruptState (InterruptState);

  return Status;
}

/**
  Call FSP API - FspMemoryInit.

  @param[in]  FspmUpdDataPtr Address pointer to the FSP_MEMORY_INIT_PARAMS structure.
  @param[out] HobListPtr     Address of the HobList pointer.

  @return EFI status returned by FspMemoryInit API.
**/
EFI_STATUS
EFIAPI
CallFspMemoryInit (
  IN VOID                       *FspmUpdDataPtr,
  OUT VOID                      **HobListPtr
  )
{
  FSP_INFO_HEADER     *FspHeader;
  FSP_MEMORY_INIT     FspMemoryInitApi;
  EFI_STATUS          Status;
  BOOLEAN             InterruptState;

  FspHeader = (FSP_INFO_HEADER *)FspFindFspHeader (PcdGet32 (PcdFspmBaseAddress));
  if (FspHeader == NULL) {
    return EFI_DEVICE_ERROR;
  }

  FspMemoryInitApi = (FSP_MEMORY_INIT)((UINTN)FspHeader->ImageBase + FspHeader->FspMemoryInitEntryOffset);
  InterruptState = SaveAndDisableInterrupts ();
  Status = Execute32BitCode ((UINTN)FspMemoryInitApi, (UINTN)FspmUpdDataPtr, (UINTN)HobListPtr);
  SetInterruptState (InterruptState);

  return Status;
}

/**
  Call FSP API - TempRamExit.

  @param[in] TempRamExitParam    Address pointer to the TempRamExit parameters structure.

  @return EFI status returned by TempRamExit API.
**/
EFI_STATUS
EFIAPI
CallTempRamExit (
  IN VOID                       *TempRamExitParam
  )
{
  FSP_INFO_HEADER     *FspHeader;
  FSP_TEMP_RAM_EXIT   TempRamExitApi;
  EFI_STATUS          Status;
  BOOLEAN             InterruptState;

  FspHeader = (FSP_INFO_HEADER *)FspFindFspHeader (PcdGet32 (PcdFspmBaseAddress));
  if (FspHeader == NULL) {
    return EFI_DEVICE_ERROR;
  }

  TempRamExitApi = (FSP_TEMP_RAM_EXIT)((UINTN)FspHeader->ImageBase + FspHeader->TempRamExitEntryOffset);
  InterruptState = SaveAndDisableInterrupts ();
  Status = Execute32BitCode ((UINTN)TempRamExitApi, (UINTN)TempRamExitParam, (UINTN)NULL);
  SetInterruptState (InterruptState);

  return Status;
}

/**
  Call FSP API - FspSiliconInit.

  @param[in] FspsUpdDataPtr Address pointer to the Silicon Init parameters structure.

  @return EFI status returned by FspSiliconInit API.
**/
EFI_STATUS
EFIAPI
CallFspSiliconInit (
  IN VOID                       *FspsUpdDataPtr
  )
{
  FSP_INFO_HEADER     *FspHeader;
  FSP_SILICON_INIT    FspSiliconInitApi;
  EFI_STATUS          Status;
  BOOLEAN             InterruptState;

  FspHeader = (FSP_INFO_HEADER *)FspFindFspHeader (PcdGet32 (PcdFspsBaseAddress));
  if (FspHeader == NULL) {
    return EFI_DEVICE_ERROR;
  }

  FspSiliconInitApi = (FSP_SILICON_INIT)((UINTN)FspHeader->ImageBase + FspHeader->FspSiliconInitEntryOffset);
  InterruptState = SaveAndDisableInterrupts ();
  Status = Execute32BitCode ((UINTN)FspSiliconInitApi, (UINTN)FspsUpdDataPtr, (UINTN)NULL);
  SetInterruptState (InterruptState);

  return Status;
}
