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
#include <stdbool.h>

#define MAX_VARIABLE_SIZE (1024)
#define MAX_VAR_DATA_SIZE (65536)

EFI_STATUS efi_main (EFI_HANDLE image, EFI_SYSTEM_TABLE *systab)
{
    int changes = 0;
    int vmajor = 1;
    int vminor = 0;
    int vpatch = 8;
    int params = 0;

    EFI_STATUS status;
//     EFI_GUID guid = EFI_GLOBAL_VARIABLE;
//     EC87D643-EBA4-4BB5-A1E5-3F3E36B20DA9
//     EFI_GUID guid = { 0xEC87D643, 0xEBA4, 0x4BB5, { 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 } }; // Setup
//     4570B7F1-ADE8-4943-8DC3-406472842384
//     EFI_GUID guid = { 0x4570B7F1, 0xADE8, 0x4943, { 0x8D, 0xC3, 0x40, 0x64, 0x72, 0x84, 0x23, 0x84 } }; // PchSetup
//     4570B7F1-ADE8-4943-8DC3-406472842384
    EFI_GUID guid = { 0x4570B7F1, 0xADE8, 0x4943, { 0x8D, 0xC3, 0x40, 0x64, 0x72, 0x84, 0x23, 0x84 } }; // PchSetup
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

    status = uefi_call_wrapper(systab->BootServices->HandleProtocol, 3, image, &LoadedImageProtocol, (void **) &loaded_image);
    if (EFI_ERROR(status)) {
            Print(L"[VirtualBiosMod] err: %r\n", status);
    } else {
	params = loaded_image->LoadOptionsSize;
    }

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

    while (true) {
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
	    if ( params == 0){
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
    }

/*
    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    if ( params == 0 ){
    Print(L"\n\n\n[VirtualBiosMod v%d.%d.%d] Press ",vmajor,vminor,vpatch);
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLUE);
    Print(L"del");
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLACK);
    Print(L" or any other key to enter Setup...");

    WaitForSingleEvent(ST->ConIn->WaitForKey, 40000000); // 10000000 = one second

    while (!exit) {

    efi_status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &efi_input_key);

	if (efi_status != EFI_SUCCESS) {
	    Print(L" Exiting\n\n\n");
	    return EFI_SUCCESS;
	} else {
	    break;
	}
    }
    }

    uefi_call_wrapper(ST->ConOut->ClearScreen, 1, ST->ConOut);
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLUE);

//  Print(L"123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_");
/////////--DRAW MAIN BLUE BOX--/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Print(L"             SERDELIUK - VirtualBiosMod v%d.%d.%d CMOS Setup Utility               ",vmajor,vminor,vpatch);
    draw_box_simple(80, 8, 0, 1);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 2); // h, v ;pos
    Print(L"Firmware version:       %d.%02d", ST->FirmwareRevision >> 16, ST->FirmwareRevision & 0xffff);
    Print(L" (%s)", ST->FirmwareVendor,L"%c"); // firmware vendoor

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 3); // h, v ;pos
    Print(L"UEFI version:           %d.%02d", ST->Hdr.Revision >> 16, ST->Hdr.Revision & 0xffff);

    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 12);

    status = get_bios_variables( &guid, L"PchSetup", &data, &data_size, attr);
    Print(L"Variable store: %r\n" , status);
    if (status != EFI_SUCCESS) {
	uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
        Print(L"Unsupported B.I.O.S.\n" , status);
	WaitForSingleEvent(ST->ConIn->WaitForKey, 200000000); // 10000000 = one second
	    if ( params == 0){
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
    }

    int offset_lock = 0x17;
//     int offset_video = 0xa45;
//     int offset_audio = 0x108E;
    int offset_xtu = 0x1B8;
    int offset_overclock = 0xDA;
//     int offset_ratio = 0x85A;
//     int offset_ring = 0x865;
//     int offset_maxring = 0x8AC;
//     int offset_adaptive = 0x40F;
    int offset_avx = 0x1C2;

redraw:
    WaitForSingleEvent(ST->ConIn->WaitForKey, 10); // 10000000 = one second
    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLUE);

//      Print(L"123456789_123456789_123456789_123456789_123456789_123456789_123456789_123456789_");
    if ( data[offset_lock] == 0) {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
	Print(L"B.I.O.S. status:        Unlocked      ");
	} else {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 5); // h, v ;pos
	Print(L"B.I.O.S. status:        Locked        ");
    }

//     if ( data[offset_video] == 0) {
// 	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 6); // h, v ;pos
// 	Print(L"Video card:             IGFX -- Intel ");
// 	} else {
// 	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 6); // h, v ;pos
// 	Print(L"Video card:             SG   -- Nvidia");
//     }

//     if ( data[offset_audio] == 0) {
// 	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
// 	Print(L"HDMI Audio:             Enabled       ");
// 	} else {
// 	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 7); // h, v ;pos
// 	Print(L"HDMI Audio:             Disabled      ");
//     }

    if ( data[offset_xtu] == 0) {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 8); // h, v ;pos
	Print(L"Intel  XTU:             Disabled   ");
	} else {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 8); // h, v ;pos
	Print(L"Intel  XTU:             Enabled    ");
    }

//     if ( data[offset_adaptive] == 0) {
// 	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 9); // h, v ;pos
// 	Print(L"Adaptive performance:   Disabled   ");
// 	} else {
// 	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 3, 9); // h, v ;pos
// 	Print(L"Adaptive performance:   Enabled    ");
//     }

    if ( data[offset_overclock] == 0) {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 5); // h, v ;pos
	Print(L"Overclock :             Disabled   ");
	} else {
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 5); // h, v ;pos
	Print(L"Overclock :             Enabled    ");
    }

//     if ( data[offset_ratio] == 0) {
// 	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 6); // h, v ;pos
// 	Print(L"Core RATIO:             Default    ");
// 	} else {
// 	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 6); // h, v ;pos
// 	Print(L"Core RATIO:             53         ");
//     }
//
//     if ( data[offset_ring] == 0) {
// 	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 7); // h, v ;pos
// 	Print(L"Core Ring:              Default    ");
// 	} else {
// 	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 7); // h, v ;pos
// 	Print(L"Core Ring:              53         ");
//     }
//
//     if ( data[offset_maxring] == 0) {
// 	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 8); // h, v ;pos
// 	Print(L"Max Ring:               Default    ");
// 	} else {
// 	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 8); // h, v ;pos
// 	Print(L"Max Ring:               53         ");
//     }

//     if ( data[offset_avx] == 0) {
// 	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 9); // h, v ;pos
// 	Print(L"Max AVX:                Default    ");
// 	} else {
// 	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 9); // h, v ;pos
// 	Print(L"Max AVX:                0x1F       ");
//     }
	uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 43, 9); // h, v ;pos
	Print(L"Max AVX:                %d          ", data[offset_avx]);

    uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_WHITE|EFI_BACKGROUND_BLACK);
    uefi_call_wrapper(ST->ConOut->SetCursorPosition, 3, ST->ConOut, 0, 11);

    Print(L" Press B to unlock the bios\n");
//     Print(L" Press V to switch video card\n");
//     Print(L" Press H to enable/disable HDMI audio\n");
    Print(L" Press I to enable/disable Intel XTU\n");
    Print(L" Press O to enable/disable Overclocking\n");
//     Print(L" Press C to enable/disable CORE speed ratio\n");
//     Print(L" Press R to enable/disable CORE ring ratio\n");
//     Print(L" Press M to enable/disable MAX  ring ratio\n");
//     Print(L" Press A to enable/disable adaptive ratio\n");
//     Print(L" Press X to enable/disable MAX AVX2 ratio\n");


    Print(L" Press ENTER to save new settings\n");
    if ( changes == 0 ) {
	Print(L" Press any other key or wait to boot without any mods\n");
    } else {
	Print(L" Press E to boot without any mods                    \n");
    }

    WaitForSingleEvent(ST->ConIn->WaitForKey, 180000000); // 10000000 = one second

    while (!exit) {

    efi_status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &efi_input_key);

    switch (efi_input_key.UnicodeChar) {
//         case 'v':
// 	changes=1;
// 	    if ( data[offset_video] == 0) {
// 		data[offset_video] = 0x4;
// 	    } else {
// 		data[offset_video] = 0x0;
// 	    }
// 	    efi_input_key = KeyReset;
// 	    goto redraw;
        case 'b':
	changes=1;
	    if ( data[offset_lock] == 0) {
		data[offset_lock] = 0x1;
	    } else {
		data[offset_lock] = 0x0;
	    }
	    efi_input_key = KeyReset;
	    goto redraw;
//         case 'h':
// 	changes=1;
// 	    if ( data[offset_audio] == 0) {
// 		data[offset_audio] = 0x1;
// 	    } else {
// 		data[offset_audio] = 0x0;
// 	    }
// 	    efi_input_key = KeyReset;
// 	    goto redraw;
        case 'i':
	changes=1;
	    if ( data[offset_xtu] == 0) {
		data[offset_xtu] = 0x1;
	    } else {
		data[offset_xtu] = 0x0;
	    }
	    efi_input_key = KeyReset;
	    goto redraw;
        case 'o':
	changes=1;
	    if ( data[offset_overclock] == 0) {
		data[offset_overclock] = 0x1;
	    } else {
		data[offset_overclock] = 0x0;
	    }
	    efi_input_key = KeyReset;
	    goto redraw;
//         case 'c':
// 	changes=1;
// 	    if ( data[offset_ratio] == 0) {
// 		data[offset_ratio] = 0x53;
// 	    } else {
// 		data[offset_ratio] = 0x0;
// 	    }
// 	    efi_input_key = KeyReset;
// 	    goto redraw;
//         case 'r':
// 	changes=1;
// 	    if ( data[offset_ring] == 0) {
// 		data[offset_ring] = 0x53;
// 	    } else {
// 		data[offset_ring] = 0x0;
// 	    }
// 	    efi_input_key = KeyReset;
// 	    goto redraw;
//         case 'm':
// 	changes=1;
// 	    if ( data[offset_maxring] == 0) {
// 		data[offset_maxring] = 0x53;
// 	    } else {
// 		data[offset_maxring] = 0x0;
// 	    }
// 	    efi_input_key = KeyReset;
// 	    goto redraw;
//         case 'a':
// 	changes=1;
// 	    if ( data[offset_adaptive] == 0) {
// 		data[offset_adaptive] = 0x1;
// 	    } else {
// 		data[offset_adaptive] = 0x0;
// 	    }
// 	    efi_input_key = KeyReset;
// 	    goto redraw;
//         case 'x':
// 	changes=1;
// 	    if ( data[offset_avx] == 0) {
// 		data[offset_avx] = 0x1f;
// 	    } else {
// 		data[offset_avx] = 0x0;
// 	    }
// 	    efi_input_key = KeyReset;
// 	    goto redraw;
        case 'e':
    	    Print(L" Exiting......\n");
    	    WaitForSingleEvent(ST->ConIn->WaitForKey, 30000000); // 10000000 = one second
	    if ( params == 0){
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
        case CHAR_CARRIAGE_RETURN:
	    if ( changes == 0 ) {
    		Print(L" Nothing to save, booting......\n");
    		WaitForSingleEvent(ST->ConIn->WaitForKey, 30000000); // 10000000 = one second
	    if ( params == 0){
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
	    }
	    status = set_bios_variables(L"SetupMode", &guid, data_size, data);
	    if (status != EFI_SUCCESS) {
		uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_RED|EFI_BACKGROUND_BLACK);
		Print(L" ERROR saving data %r\n" , status);
		WaitForSingleEvent(ST->ConIn->WaitForKey, 30000000); // 10000000 = one second
	    if ( params == 0){
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
	    }
		uefi_call_wrapper(ST->ConOut->SetAttribute, 2, ST->ConOut, EFI_GREEN|EFI_BACKGROUND_BLACK);
	    Print(L" Write data OK, rebooting...          \n");
    	    WaitForSingleEvent(ST->ConIn->WaitForKey, 30000000); // 10000000 = one second
	    uefi_call_wrapper(RT->ResetSystem, 4, EfiResetCold, EFI_SUCCESS, 0, NULL);
    	    return EFI_SUCCESS;
        default: // continue boot
	    if ( changes == 1 ) {
		efi_input_key = KeyReset;
		goto redraw;
	    }
    	    Print(L" Exiting......\n");
    	    WaitForSingleEvent(ST->ConIn->WaitForKey, 30000000); // 10000000 = one second
	    if ( params == 0){
    		return EFI_SUCCESS;
	    } else {
		uefi_call_wrapper(RT->ResetSystem, 4, EfiResetWarm, EFI_SUCCESS, 0, NULL);
	    }
        }
    }
    	    return EFI_SUCCESS;
            */
}
