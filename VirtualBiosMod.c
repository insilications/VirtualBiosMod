/*
        This file is part of VirtualBiosMod, a virtual bios setup interface
        Copyright (C) 2020 Alexandru Marc Serdeliuc

        https://github.com/serdeliuk/VirtualBiosMod

        VirtualBiosMod is free software: you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation, either version 3 of the License, or
        (at your option) any later version.

        VirtualBiosMod is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with VirtualBiosMod.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <efi.h>
#include <efilib.h>
#include <efilink.h>

#define BUFSIZE 8

EFI_STATUS
efi_main (EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
    InitializeLib(image, systab);
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN Size, CurSize;
    CHAR16 *Name, *val;
    EFI_GUID curGuid= NullGuid;
    CurSize = BUFSIZE;
    Name = AllocateZeroPool(CurSize);
    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    while (TRUE) {
        Size = CurSize;
        Status = uefi_call_wrapper(RT->GetNextVariableName, 3, &Size, Name, &curGuid);
        if (Status ==  EFI_NOT_FOUND) {
            break;
        }
        if (Status == EFI_BUFFER_TOO_SMALL) {
            Name = ReallocatePool(Name, CurSize, Size);
            CurSize = Size;
            Status = uefi_call_wrapper(RT->GetNextVariableName, 3, &Size, Name, &curGuid);
        }
        if (Status != EFI_SUCCESS) {
            Print(L"ERROR: GetNextVariableName failed: %d\n", Status);
            break;
        }
		val = LibGetVariable(Name, &curGuid);
		Print(L"%g - %s - %s\n", &curGuid, Name, val);
		FreePool(val);
    }
    Print(L"Status: %r\n" , Status);
    return Status;
}







/*
	EFI_STATUS status;
	CHAR16 name[256], *val, fmt[20];
	EFI_GUID vendor;
	UINTN size;

	InitializeLib(image, systab);

	name[0] = 0;
	vendor = NullGuid;

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);

	Print(L"GUID                                Variable Name        Value\n");
	Print(L"=================================== ==================== ========\n");

	StrCpy(fmt, L"%.-35g %.-20s %s\n");
	while (1) {
		size = sizeof(name);
		status = uefi_call_wrapper(RT->GetNextVariableName, 3, &size, name, &vendor);
		if (status != EFI_SUCCESS)
			break;

		val = LibGetVariable(name, &vendor);
		Print(fmt, &vendor, name, val);
		FreePool(val);
	}
    if (status != EFI_SUCCESS) {
	uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
    Print(L"Unsupported B.I.O.S.\n" , status);
	WaitForSingleEvent(ST->ConIn->WaitForKey, 200000000); // 10000000 = one second
    uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
    }*/

