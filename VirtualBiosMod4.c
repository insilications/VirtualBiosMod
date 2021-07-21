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
#include "functions.c"

EFI_STATUS efi_main (EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
    int params = 0;
    EFI_STATUS status;
//     EFI_GUID guid = EFI_GLOBAL_VARIABLE;
//     EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9
//     EFI_GUID guid = { 0xEC87D643, 0xEBA4, 0x4BB5, { 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 } }; // Setup
//     4570B7F1-ADE8-4943-8DC3-406472842384
//     EFI_GUID guid = { 0x4570B7F1, 0xADE8, 0x4943, { 0x8D, 0xC3, 0x40, 0x64, 0x72, 0x84, 0x23, 0x84 } }; // PchSetup
//     4570b7f1-ade8-4943-8dc3-406472842384
    EFI_GUID guid = { 0x4570B7F1, 0xADE8, 0x4943, {  0x8D, 0xC3, 0x40, 0x64, 0x72, 0x84, 0x23, 0x84 } }; // PchSetup
//     8BE4DF61-93CA-11D2-AA0D-00E098032B8C
//     EFI_GUID guid = { 0x8BE4DF61, 0x93CA, 0x11d2, { 0xAA, 0x0D, 0x00, 0xE0, 0x98, 0x03, 0x2B, 0x8C } }; // SetupMode

    CHAR8 *data;
    UINTN data_size;
    UINT32 *attr = 0;
    EFI_INPUT_KEY efi_input_key;
    EFI_INPUT_KEY KeyReset = {0};

    EFI_STATUS efi_status;
    EFI_LOADED_IMAGE *loaded_image = NULL;
    BOOLEAN exit = FALSE;

    InitializeLib(image, systab);

//     status = uefi_call_wrapper(systab->BootServices->HandleProtocol, 3, image, &LoadedImageProtocol, (void **) &loaded_image);
//     if (EFI_ERROR(status)) {
//             Print(L"[VirtualBiosMod] err: %r\n", status);
//     } else {
// 	params = loaded_image->LoadOptionsSize;
//     }

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
//     find_name_buf = AllocatePool(find_name_size);
//     if (!find_name_buf) {
//         Print(L"Can't allocate find_name_buf: %r\n" , status);
//         return EFI_OUT_OF_RESOURCES;
//     }

//     CHAR16 *SetupPch = L"SetupPch";
    EFI_GUID find_guid;
    UINTN find_name_size = sizeof(CHAR16);
    UINTN max_find_name_size = sizeof(CHAR16);
    CHAR16 *find_name_buf;
    find_name_buf = AllocateZeroPool(max_find_name_size);
    find_guid = NullGuid;
//     find_name_buf[0] = 0x0;
//
//     Print(L"Buffer size: %d\n", find_name_size);

    while (TRUE) {
        find_name_size = max_find_name_size;
        status = uefi_call_wrapper(RT->GetNextVariableName, 3, &find_name_size, find_name_buf, &find_guid);
    //         status = get_next_bios_variables(&guid, find_name_buf, &find_name_size);
    //         Print(L"store name: %s, store name size: %u, store guid: %08x-%04x-%04x - %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n",
    //                 find_name_buf,
    //                 (UINT32) find_name_size,
    //                 guid.Data1,
    //                 guid.Data2,
    //                 guid.Data3,
    //                 guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
//             Print(L"find_name_size: %d\n" , find_name_size);
//             Print(L"find_name_buf: %s\n" , find_name_buf);
        if(status == EFI_INVALID_PARAMETER) {
            Print(L"Error status: invalid parameter\n" , status);
            break;
        }

        if(status == EFI_BUFFER_TOO_SMALL) {
            Print(L"EFI_BUFFER_TOO_SMALL\n");
            find_name_buf = ReallocatePool(find_name_buf, max_find_name_size, find_name_size);
            if (find_name_buf == NULL) {
                Print(L"Unable to reallocate memory\n");
                status = EFI_OUT_OF_RESOURCES;
                break;
            } else {
                max_find_name_size = find_name_size;
            }
        }

//         if (EFI_ERROR(status)) {
//             FreePool(find_name_buf);
//             Print(L"Error status: %r\n" , status);
//             break;
//         }

        if(status == EFI_NOT_FOUND) {
            Print(L"Unable to found variable store\n" , status);
            break;
        }



        Print(L"%g : %s\n", &find_guid, find_name_buf);
//         if (!strlen_valid(find_name_buf, find_name_size)) {
//              Print(L"strlen_valid error: %s - %d \n" , find_name_buf, find_name_size);
//             break;
//         }
//         if (!status) {
// //             Print(L"find_name_buf: %s\n" , find_name_buf);
//             if(StrCmp(find_name_buf, SetupPch) == 0) {
//                 Print(L"store name: %s, store name size: %u, store guid: %08x-%04x-%04x - %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n\n",
//                       find_name_buf,
//                       (UINT32) find_name_size,
//                       guid.Data1,
//                       guid.Data2,
//                       guid.Data3,
//                       guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
//             }
//         }
    };

    if (status != EFI_SUCCESS) {
	uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
        Print(L"Unsupported B.I.O.S.\n" , status);
	WaitForSingleEvent(ST->ConIn->WaitForKey, 200000000); // 10000000 = one second
    uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
}
