#include "UefiShellDebug1CommandsLib.h"
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/ShellLib.h>

UINT16  HwmBase = 0;
UINT16 SIO_EFIR = 0x4E; //Extended Function Index Register
UINT16 SIO_EFDR = 0x4F; //Extended Function Data Register


/*--------------------------------------------------------------------*/
/*                              SIO                                   */

#define SIO_EFER              0x2E //Extended Function Enable Register

#define SIO_CFG_INDX_LDN      0x07

#define SIO_HWM_DEVICE_ID     0x0B

STATIC CONST SHELL_PARAM_ITEM   ParamList[] = {
  { NULL,   TypeMax   }
};


VOID
SioEntryExtFunMode(
	VOID
)
{
	IoWrite8(SIO_EFER, 0x87);
	IoWrite8(SIO_EFER, 0x87);
}

VOID
SioExitExtFunMode(
	VOID
)
{
	IoWrite8(SIO_EFER, 0xAA);
}

VOID
SioWrite(
	UINT8 iLDN,
	UINT8 bData
)
{
	IoWrite8(SIO_EFIR, iLDN);
	IoWrite8(SIO_EFDR, bData);
}

UINT8
SioRead(
	UINT8 Index
)
{
	IoWrite8(SIO_EFIR, Index);
	return IoRead8(SIO_EFDR);
}

VOID
SioDevSet(
	UINT8 Device
)
{
	SioWrite(SIO_CFG_INDX_LDN, Device);
}

VOID
SioMotherboard(
	VOID
)
{
	UINT16 Motherboard;
	SioEntryExtFunMode();

	Motherboard = (SioRead(0x20) << 8) | SioRead(0x21);
	if (Motherboard == 0x0000 || Motherboard == 0xFFFF) {
		SIO_EFIR = 0x2E;
		SIO_EFDR = 0x2F;
	}

	Motherboard = (SioRead(0x20) << 8) | SioRead(0x21);
	if (Motherboard == 0x0000 || Motherboard == 0xFFFF) {
		SIO_EFIR = 0x6E;
		SIO_EFDR = 0x6F;
	}

	SioExitExtFunMode();
}

UINT16
GetDeviceID(
	VOID
)
{
	UINT16 Base;
	SioEntryExtFunMode();

	Base = (SioRead(0x20) << 8) | SioRead(0x21);
	
	SioExitExtFunMode();

	return Base;
}


VOID
PrintLdnStatus(
	VOID
)
{
	if (GetDeviceID() == 0x0000 || GetDeviceID() == 0xFFFF) {
		Print(L"ERROR");
	}


	if (GetDeviceID() == 0xD42A) {
		Print(L"Nuvoton NCT5585D = 0xD42A\r\n");
		int i;
		SioEntryExtFunMode();
		UINT8 registerAddresses2[] = { 0x30, 0x60, 0x61, 0x70, 0xF0, 0xF2 };
		UINT8 statusRegister;
		Print(L"\nLDN 0x02 (UART A)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses2) / sizeof(registerAddresses2[0]); i++) {
			Print(L"%02x ", registerAddresses2[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses2) / sizeof(registerAddresses2[0]); i++) {
			SioDevSet(0x02);
			statusRegister = SioRead(registerAddresses2[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 01 03 F8 04 00 00\n");

		UINT8 registerAddresses5[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x70, 0x72, 0xF0 };
		Print(L"\nLDN 0x05 (Keyboard Controller)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses5) / sizeof(registerAddresses5[0]); i++) {
			Print(L"%02x ", registerAddresses5[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses5) / sizeof(registerAddresses5[0]); i++) {
			SioDevSet(0x05);
			statusRegister = SioRead(registerAddresses5[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 00 00 00 00 00 43\n");

		UINT8 registerAddresses6[] = { 0x30, 0x60, 0x61, 0x70 };
		Print(L"\nLDN 0x06 (Consumer IR)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses6) / sizeof(registerAddresses6[0]); i++) {
			Print(L"%02x ", registerAddresses6[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses6) / sizeof(registerAddresses6[0]); i++) {
			SioDevSet(0x06);
			statusRegister = SioRead(registerAddresses6[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 00 00\n");

		UINT8 registerAddresses7[] = { 0x30, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE };
		Print(L"\nLDN 0x07 (GPIO 7, GPIO 8, GPIO 9)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses7) / sizeof(registerAddresses7[0]); i++) {
			Print(L"%02x ", registerAddresses7[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses7) / sizeof(registerAddresses7[0]); i++) {
			SioDevSet(0x07);
			statusRegister = SioRead(registerAddresses7[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 0A FE NA 00 NA FF NA 00 NA FD NA 00 NA 00 00 00\n");

		UINT8 registerAddresses8[] = { 0x30, 0x60, 0x61, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFE, 0xFF };
		Print(L"\nLDN 0x08 (WDT1, WDT3, KBC P20)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses8) / sizeof(registerAddresses8[0]); i++) {
			Print(L"%02x ", registerAddresses8[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses8) / sizeof(registerAddresses8[0]); i++) {
			SioDevSet(0x08);
			statusRegister = SioRead(registerAddresses8[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 00 00 00 00 00 00 00 00 00\n");

		UINT8 registerAddresses9[] = { 0x30, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE9, 0xEA, 0xEB, 0xF4, 0xF5, 0xF6, 0xF7 };
		Print(L"\nLDN 0x09 (GPIO 2, GPIO 3, GPIO 5)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses9) / sizeof(registerAddresses9[0]); i++) {
			Print(L"%02x ", registerAddresses9[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses9) / sizeof(registerAddresses9[0]); i++) {
			SioDevSet(0x09);
			statusRegister = SioRead(registerAddresses9[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 FF NA 00 NA 7F NA 00 NA 00 00 00 FF NA 00 NA\n");

		UINT8 registerAddressesA[] = { 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE9, 0xEB, 0xEC, 0xED, 0xEE, 0xF0, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xFC, 0xFE, 0xFF };
		Print(L"\nLDN 0x0a (ACPI)\nIDX ");
		for (i = 0; i < sizeof(registerAddressesA) / sizeof(registerAddressesA[0]); i++) {
			Print(L"%02x ", registerAddressesA[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddressesA) / sizeof(registerAddressesA[0]); i++) {
			SioDevSet(0x0A);
			statusRegister = SioRead(registerAddressesA[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 01 00 00 00 00 00 1A 00 00 03 00 01 00 10 DE 00 00 00 00 C0 80 00 18\n");

		UINT8 registerAddressesB[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x70, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE6, 0xE7, 0xEA, 0xED, 0xF0, 0xF1, 0xF2, 0xF4, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFC };
		Print(L"\nLDN 0x0b (Hardware Monitor, Front Panel LED)\nIDX ");
		for (i = 0; i < sizeof(registerAddressesB) / sizeof(registerAddressesB[0]); i++) {
			Print(L"%02x ", registerAddressesB[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddressesB) / sizeof(registerAddressesB[0]); i++) {
			SioDevSet(0x0B);
			statusRegister = SioRead(registerAddressesB[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 00 00 00 00 00 00 99 99 99 99 99 00 FF 81 24 00 00 00 02 00 87 47 00 00 00\n");

		UINT8 registerAddressesD[] = { 0xE1, 0xE2, 0xE3, 0xE7, 0xED, 0xF0, 0xF3 };
		Print(L"\nLDN 0x0d (GPIO Functions, RSTOUT)\nIDX ");
		for (i = 0; i < sizeof(registerAddressesD) / sizeof(registerAddressesD[0]); i++) {
			Print(L"%02x ", registerAddressesD[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddressesD) / sizeof(registerAddressesD[0]); i++) {
			SioDevSet(0x0D);
			statusRegister = SioRead(registerAddressesD[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 01 FF 10 E0 00 00\n");

		UINT8 registerAddressesE[] = { 0x30, 0x60, 0x61, 0x70, 0xF0, 0xF1, 0xF4, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA };
		Print(L"\nLDN 0x0e (CIR Wake-Up)\nIDX ");
		for (i = 0; i < sizeof(registerAddressesE) / sizeof(registerAddressesE[0]); i++) {
			Print(L"%02x ", registerAddressesE[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddressesE) / sizeof(registerAddressesE[0]); i++) {
			SioDevSet(0x0E);
			statusRegister = SioRead(registerAddressesE[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 00 00 02 1F 10 17 0B 00 07 00\n");

		UINT8 registerAddressesF[] = { 0xE1, 0xE2, 0xE4, 0xE6, 0xE7, 0xEA, 0xF0, 0xF1, 0xF2 };
		Print(L"\nLDN 0x0f (GPIO Push-Pull or Open-Drain selection)\nIDX ");
		for (i = 0; i < sizeof(registerAddressesF) / sizeof(registerAddressesF[0]); i++) {
			Print(L"%02x ", registerAddressesF[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddressesF) / sizeof(registerAddressesF[0]); i++) {
			SioDevSet(0x0F);
			statusRegister = SioRead(registerAddressesF[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF DF FF FF FE FF FD 9D 00 00\n");

		UINT8 registerAddresses11[] = { 0xE0, 0xE1, 0xE2 };
		Print(L"\nLDN 0x11 (GPIO, RI PSOUT Wake-Up Status)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses11) / sizeof(registerAddresses11[0]); i++) {
			Print(L"%02x ", registerAddresses11[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses11) / sizeof(registerAddresses11[0]); i++) {
			SioDevSet(0x11);
			statusRegister = SioRead(registerAddresses11[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 08 NA 00\n");

		UINT8 registerAddresses12[] = { 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xEA, 0xEB, 0xEC, 0xED, 0xEF };
		Print(L"\nLDN 0x12 (GPIO Functions)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses12) / sizeof(registerAddresses12[0]); i++) {
			Print(L"%02x ", registerAddresses12[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses12) / sizeof(registerAddresses12[0]); i++) {
			SioDevSet(0x12);
			statusRegister = SioRead(registerAddresses12[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 37 80 00 00 80 00 00 00 00 00 00\n");

		UINT8 registerAddresses14[] = { 0xE0, 0xE1, 0xE2, 0xE3, 0xE4 };
		Print(L"\nLDN 0x14 (PORT80 UART)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses14) / sizeof(registerAddresses14[0]); i++) {
			Print(L"%02x ", registerAddresses14[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses14) / sizeof(registerAddresses14[0]); i++) {
			SioDevSet(0x14);
			statusRegister = SioRead(registerAddresses14[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 80 00 00 10 00\n");

		UINT8 registerAddresses15[] = { 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA };
		Print(L"\nLDN 0x15 (Fading LED)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses15) / sizeof(registerAddresses15[0]); i++) {
			Print(L"%02x ", registerAddresses15[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses15) / sizeof(registerAddresses15[0]); i++) {
			SioDevSet(0x15);
			statusRegister = SioRead(registerAddresses15[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF FF 7F 00 06 05 17 00 0F 10 06 05\n");

		UINT8 registerAddresses16[] = { 0x30, 0xE0, 0xE1, 0xE2, 0xE3, 0xF3, 0xF4, 0xF5, 0xF8 };
		Print(L"\nLDN 0x16 (Deep Sleep)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses16) / sizeof(registerAddresses16[0]); i++) {
			Print(L"%02x ", registerAddresses16[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses16) / sizeof(registerAddresses16[0]); i++) {
			SioDevSet(0x16);
			statusRegister = SioRead(registerAddresses16[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF A0 20 04 05 01 80 01 00 20\n");

		SioExitExtFunMode();
	}


	if (GetDeviceID() == 0x5570) {
		Print(L"IT5570E = 0x5570");
		int i;
		SioEntryExtFunMode();
		UINT8 registerAddresses1[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x70, 0x71, 0xF0 };
		UINT8 statusRegister;
		Print(L"\nLDN 0x01 (Serial Port 1 (UART1)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses1) / sizeof(registerAddresses1[0]); i++) {
			Print(L"%02x ", registerAddresses1[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses1) / sizeof(registerAddresses1[0]); i++) {
			SioDevSet(0x01);
			statusRegister = SioRead(registerAddresses1[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 02 F8 00 00 03 02 00\n");

		UINT8 registerAddresses2[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x70, 0x71, 0xF0 };
		Print(L"\nLDN 0x02 (Serial Port 2 (UART2)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses2) / sizeof(registerAddresses2[0]); i++) {
			Print(L"%02x ", registerAddresses2[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses2) / sizeof(registerAddresses2[0]); i++) {
			SioDevSet(0x02);
			statusRegister = SioRead(registerAddresses2[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 02 F8 00 00 03 02 00\n");

		UINT8 registerAddresses4[] = { 0x30, 0x60, 0x61, 0x70, 0x71 };
		Print(L"\nLDN 0x04 (System Wake-Up Control (SWUC)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses4) / sizeof(registerAddresses4[0]); i++) {
			Print(L"%02x ", registerAddresses4[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses4) / sizeof(registerAddresses4[0]); i++) {
			SioDevSet(0x04);
			statusRegister = SioRead(registerAddresses4[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 00 00 01\n");

		UINT8 registerAddresses5[] = { 0x30, 0x70, 0x71 };
		Print(L"\nLDN 0x05 (KBC Mouse)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses5) / sizeof(registerAddresses5[0]); i++) {
			Print(L"%02x ", registerAddresses5[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses5) / sizeof(registerAddresses5[0]); i++) {
			SioDevSet(0x05);
			statusRegister = SioRead(registerAddresses5[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 0C 01\n");

		UINT8 registerAddresses6[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x70, 0x71 };
		Print(L"\nLDN 0x06 (KBC Keyboard)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses6) / sizeof(registerAddresses6[0]); i++) {
			Print(L"%02x ", registerAddresses6[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses6) / sizeof(registerAddresses6[0]); i++) {
			SioDevSet(0x06);
			statusRegister = SioRead(registerAddresses6[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 60 00 64 01 01\n");

		UINT8 registerAddressesA[] = { 0x30, 0x60, 0x61, 0x70, 0x71 };
		Print(L"\nLDN 0x0a (Consumer IR)\nIDX ");
		for (i = 0; i < sizeof(registerAddressesA) / sizeof(registerAddressesA[0]); i++) {
			Print(L"%02x ", registerAddressesA[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddressesA) / sizeof(registerAddressesA[0]); i++) {
			SioDevSet(0x0A);
			statusRegister = SioRead(registerAddressesA[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 03 10 00 02\n");

		UINT8 registerAddressesF[] = { 0x30, 0x60, 0x61, 0x70, 0x71, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF9, 0xFA, 0xFB, 0xFC };
		Print(L"\nLDN 0x0f (Shared Memory/Flash Interface (SMFI))\nIDX ");
		for (i = 0; i < sizeof(registerAddressesF) / sizeof(registerAddressesF[0]); i++) {
			Print(L"%02x ", registerAddressesF[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddressesF) / sizeof(registerAddressesF[0]); i++) {
			SioDevSet(0x0F);
			statusRegister = SioRead(registerAddressesF[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 00 00 02 00 00 00 00 00 00 00 00 00 00 01\n");

		UINT8 registerAddresses10[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x70, 0x71, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5 };
		Print(L"\nLDN 0x10 (RTC-like Timer (RTC))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses10) / sizeof(registerAddresses10[0]); i++) {
			Print(L"%02x ", registerAddresses10[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses10) / sizeof(registerAddresses10[0]); i++) {
			SioDevSet(0x10);
			statusRegister = SioRead(registerAddresses10[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 70 02 72 00 00 00 00 00 00 08 01 00 49 4A 00 00 00\n");

		UINT8 registerAddresses11[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x70, 0x71 };
		Print(L"\nLDN 0x11 (Power Management Interface Channel 1 (PMC1))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses11) / sizeof(registerAddresses11[0]); i++) {
			Print(L"%02x ", registerAddresses11[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses11) / sizeof(registerAddresses11[0]); i++) {
			SioDevSet(0x11);
			statusRegister = SioRead(registerAddresses11[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 62 00 66 01 01\n");

		UINT8 registerAddresses12[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x70, 0x71, 0xF0 };
		Print(L"\nLDN 0x12 (Power Management Interface Channel 2 (PMC2))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses12) / sizeof(registerAddresses12[0]); i++) {
			Print(L"%02x ", registerAddresses12[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses12) / sizeof(registerAddresses12[0]); i++) {
			SioDevSet(0x12);
			statusRegister = SioRead(registerAddresses12[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 68 00 6C 00 00 01 01 NA\n");

		UINT8 registerAddresses13[] = { 0x30, 0x60, 0x61, 0x70, 0x71 };
		Print(L"\nLDN 0x13 (Serial Peripheral Interface (SSPI))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses13) / sizeof(registerAddresses13[0]); i++) {
			Print(L"%02x ", registerAddresses13[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses13) / sizeof(registerAddresses13[0]); i++) {
			SioDevSet(0x13);
			statusRegister = SioRead(registerAddresses13[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 03 00 00 02\n");

		UINT8 registerAddresses14[] = { 0x30, 0x60, 0x61 };
		Print(L"\nLDN 0x14 (Platform Environment Control Interface (PECI))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses14) / sizeof(registerAddresses14[0]); i++) {
			Print(L"%02x ", registerAddresses14[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses14) / sizeof(registerAddresses14[0]); i++) {
			SioDevSet(0x14);
			statusRegister = SioRead(registerAddresses14[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 00\n");

		UINT8 registerAddresses17[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x70, 0x71 };
		Print(L"\nLDN 0x17 (Power Management Interface Channel 3 (PMC3))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses17) / sizeof(registerAddresses17[0]); i++) {
			Print(L"%02x ", registerAddresses17[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses17) / sizeof(registerAddresses17[0]); i++) {
			SioDevSet(0x17);
			statusRegister = SioRead(registerAddresses17[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 6A 00 6E 01 01\n");

		UINT8 registerAddresses18[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x70, 0x71 };
		Print(L"\nLDN 0x18 (Power Management Interface Channel 4 (PMC4))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses18) / sizeof(registerAddresses18[0]); i++) {
			Print(L"%02x ", registerAddresses18[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses18) / sizeof(registerAddresses18[0]); i++) {
			SioDevSet(0x18);
			statusRegister = SioRead(registerAddresses18[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 74 00 78 01 01\n");

		UINT8 registerAddresses19[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x70, 0x71 };
		Print(L"\nLDN 0x19 (Power Management Interface Channel 5 (PMC5))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses19) / sizeof(registerAddresses19[0]); i++) {
			Print(L"%02x ", registerAddresses19[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses19) / sizeof(registerAddresses19[0]); i++) {
			SioDevSet(0x19);
			statusRegister = SioRead(registerAddresses19[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 7A 00 7C 01 01\n");

		SioExitExtFunMode();
	}

	if (GetDeviceID() == 0x5571) {
		Print(L"IT5571E = 0x5571");
		int i;
		SioEntryExtFunMode();
		UINT8 registerAddresses1[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x70, 0x71, 0xF0 };
		UINT8 statusRegister;
		Print(L"\nLDN 0x01 (UART 1)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses1) / sizeof(registerAddresses1[0]); i++) {
			Print(L"%02x ", registerAddresses1[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses1) / sizeof(registerAddresses1[0]); i++) {
			SioDevSet(0x01);
			statusRegister = SioRead(registerAddresses1[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 02 F8 00 00 03 02 00\n");

		UINT8 registerAddresses2[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x70, 0x71, 0xF0 };
		Print(L"\nLDN 0x02 (UART 2)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses2) / sizeof(registerAddresses2[0]); i++) {
			Print(L"%02x ", registerAddresses2[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses2) / sizeof(registerAddresses2[0]); i++) {
			SioDevSet(0x02);
			statusRegister = SioRead(registerAddresses2[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 02 F8 00 00 03 02 00\n");

		UINT8 registerAddresses4[] = { 0x30, 0x60, 0x61, 0x70, 0x71 };
		Print(L"\nLDN 0x04 (System Wake-Up Control (SWUC))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses4) / sizeof(registerAddresses4[0]); i++) {
			Print(L"%02x ", registerAddresses4[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses4) / sizeof(registerAddresses4[0]); i++) {
			SioDevSet(0x04);
			statusRegister = SioRead(registerAddresses4[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 00 00 01\n");

		UINT8 registerAddresses5[] = { 0x30, 0x70, 0x71 };
		Print(L"\nLDN 0x05 (KBC Mouse)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses5) / sizeof(registerAddresses5[0]); i++) {
			Print(L"%02x ", registerAddresses5[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses5) / sizeof(registerAddresses5[0]); i++) {
			SioDevSet(0x05);
			statusRegister = SioRead(registerAddresses5[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 0C 01\n");

		UINT8 registerAddresses6[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x70, 0x71 };
		Print(L"\nLDN 0x06 (KBC Keyboard)\nIDX ");
		for (i = 0; i < sizeof(registerAddresses6) / sizeof(registerAddresses6[0]); i++) {
			Print(L"%02x ", registerAddresses6[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses6) / sizeof(registerAddresses6[0]); i++) {
			SioDevSet(0x06);
			statusRegister = SioRead(registerAddresses6[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 60 00 64 01 01\n");

		UINT8 registerAddressesA[] = { 0x30, 0x60, 0x61, 0x70, 0x71 };
		Print(L"\nLDN 0x0a (Consumer IR)\nIDX ");
		for (i = 0; i < sizeof(registerAddressesA) / sizeof(registerAddressesA[0]); i++) {
			Print(L"%02x ", registerAddressesA[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddressesA) / sizeof(registerAddressesA[0]); i++) {
			SioDevSet(0x0A);
			statusRegister = SioRead(registerAddressesA[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 03 10 00 02\n");

		UINT8 registerAddressesF[] = { 0x30, 0x60, 0x61, 0x70, 0x71, 0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF9, 0xFA, 0xFB, 0xFC };
		Print(L"\nLDN 0x0f (Shared Memory/Flash Interface (SMFI))\nIDX ");
		for (i = 0; i < sizeof(registerAddressesF) / sizeof(registerAddressesF[0]); i++) {
			Print(L"%02x ", registerAddressesF[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddressesF) / sizeof(registerAddressesF[0]); i++) {
			SioDevSet(0x0F);
			statusRegister = SioRead(registerAddressesF[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 00 00 02 00 00 00 00 00 00 00 00 00 00 01\n");

		UINT8 registerAddresses10[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x70, 0x71, 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5 };
		Print(L"\nLDN 0x10 (RTC-like Timer (RTC))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses10) / sizeof(registerAddresses10[0]); i++) {
			Print(L"%02x ", registerAddresses10[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses10) / sizeof(registerAddresses10[0]); i++) {
			SioDevSet(0x10);
			statusRegister = SioRead(registerAddresses10[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 70 02 72 00 00 00 00 00 00 08 01 00 49 4A 00 00 00\n");

		UINT8 registerAddresses11[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x70, 0x71 };
		Print(L"\nLDN 0x11 (Power Management Interface Channel 1 (PMC1))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses11) / sizeof(registerAddresses11[0]); i++) {
			Print(L"%02x ", registerAddresses11[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses11) / sizeof(registerAddresses11[0]); i++) {
			SioDevSet(0x11);
			statusRegister = SioRead(registerAddresses11[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 62 00 66 01 01\n");

		UINT8 registerAddresses12[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x70, 0x71, 0xF0 };
		Print(L"\nLDN 0x12 (Power Management Interface Channel 2 (PMC2))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses12) / sizeof(registerAddresses12[0]); i++) {
			Print(L"%02x ", registerAddresses12[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses12) / sizeof(registerAddresses12[0]); i++) {
			SioDevSet(0x12);
			statusRegister = SioRead(registerAddresses12[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 68 00 6C 00 00 01 01 NA\n");

		UINT8 registerAddresses13[] = { 0x30, 0x60, 0x61, 0x70, 0x71 };
		Print(L"\nLDN 0x13 (Serial Peripheral Interface (SSPI))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses13) / sizeof(registerAddresses13[0]); i++) {
			Print(L"%02x ", registerAddresses13[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses13) / sizeof(registerAddresses13[0]); i++) {
			SioDevSet(0x13);
			statusRegister = SioRead(registerAddresses13[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 03 00 00 02\n");

		UINT8 registerAddresses14[] = { 0x30, 0x60, 0x61 };
		Print(L"\nLDN 0x14 (Platform Environment Control Interface (PECI))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses14) / sizeof(registerAddresses14[0]); i++) {
			Print(L"%02x ", registerAddresses14[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses14) / sizeof(registerAddresses14[0]); i++) {
			SioDevSet(0x14);
			statusRegister = SioRead(registerAddresses14[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 00\n");

		UINT8 registerAddresses17[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x70, 0x71 };
		Print(L"\nLDN 0x17 (Power Management Interface Channel 3 (PMC3))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses17) / sizeof(registerAddresses17[0]); i++) {
			Print(L"%02x ", registerAddresses17[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses17) / sizeof(registerAddresses17[0]); i++) {
			SioDevSet(0x17);
			statusRegister = SioRead(registerAddresses17[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 6A 00 6E 01 01\n");

		UINT8 registerAddresses18[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x70, 0x71 };
		Print(L"\nLDN 0x18 (Power Management Interface Channel 4 (PMC4))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses18) / sizeof(registerAddresses18[0]); i++) {
			Print(L"%02x ", registerAddresses18[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses18) / sizeof(registerAddresses18[0]); i++) {
			SioDevSet(0x18);
			statusRegister = SioRead(registerAddresses18[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 74 00 78 01 01\n");

		UINT8 registerAddresses19[] = { 0x30, 0x60, 0x61, 0x62, 0x63, 0x70, 0x71 };
		Print(L"\nLDN 0x19 (Power Management Interface Channel 5 (PMC5))\nIDX ");
		for (i = 0; i < sizeof(registerAddresses19) / sizeof(registerAddresses19[0]); i++) {
			Print(L"%02x ", registerAddresses19[i]);
		}
		Print(L"\nVAL ");
		for (i = 0; i < sizeof(registerAddresses19) / sizeof(registerAddresses19[0]); i++) {
			SioDevSet(0x19);
			statusRegister = SioRead(registerAddresses19[i]);
			Print(L"%02x ", statusRegister);
		}
		Print(L"\nDEF 00 00 7A 00 7C 01 01\n");

		SioExitExtFunMode();
	}
}

UINT16
GetHwmBase(
	VOID
)
{
	UINT16 Hwm;

	SioEntryExtFunMode();

	SioDevSet(SIO_HWM_DEVICE_ID);

	Hwm = (SioRead(0x60) << 8) | SioRead(0x61);

	SioExitExtFunMode();

	return Hwm;
}

/*                                                                    */
/*--------------------------------------------------------------------*/
/*                              HWM                                   */


#define HW_MONITOR_INDEX_OFFSET 			0x5
#define HW_MONITOR_DATA_OFFSET 				0x6
#define HW_MONITOR_BANK_SELECT_INDEX 	    0x4E

#define HWM_BANK_0              0
#define HWM_BANK_1              1
#define HWM_BANK_2              2
#define HWM_BANK_3              3
#define HWM_BANK_4              4
#define HWM_BANK_5              5
#define HWM_BANK_6              6
#define HWM_BANK_7              7
#define HWM_BANK_8              8
#define HWM_BANK_9              9
#define HWM_BANK_10             10
#define HWM_BANK_11             11
#define HWM_BANK_12             12

#define TEMP_SRC_AUXTIN3  0x06
#define TEMP_SRC_PECI0    0x10


VOID
HwmWrite(
	UINT8 Index,
	UINT8 Data
)
{
	IoWrite8(HwmBase + HW_MONITOR_INDEX_OFFSET, Index);
	IoWrite8(HwmBase + HW_MONITOR_DATA_OFFSET, Data);

}

UINT8
HwmRead(
	UINT8 Index
)
{
	IoWrite8(HwmBase + HW_MONITOR_INDEX_OFFSET, Index);
	return IoRead8(HwmBase + HW_MONITOR_DATA_OFFSET);
}

VOID
HwmSelectBank(
	UINT8 Bank
)
{
	HwmWrite(HW_MONITOR_BANK_SELECT_INDEX, Bank);	//Select Bank
}




VOID
HwmBank(
	VOID
)
{
	UINT8 Bank = 0;

	HwmBase = GetHwmBase();


	if (HwmBase == 0x02A0) {

		Print(L"\nHwm base = %02x\n", GetHwmBase());

		for (Bank = HWM_BANK_0; Bank <= HWM_BANK_12; ++Bank) {
			HwmSelectBank(Bank);
			Print(L"Bank: %d\n    ", Bank);
			for (unsigned char i = 0; i < 16; ++i) {
				Print(L"%02x ", i);
			}
			Print(L"\n");

			for (unsigned char i = 0; i < 16; ++i) {
				Print(L"%02x: ", i);
				for (unsigned char j = 0; j < 16; ++j) {
					unsigned char index = i * 16 + j;
					Print(L"%02x ", HwmRead(index));
				}
				Print(L"\n");
			}
			Print(L"\n");
		}
	}
	else {
		Print(L"\nHwm base = %02x\n", HwmBase);
	}
}

/*                                                                    */
/*--------------------------------------------------------------------*/
/*                              BRAM                                  */
#define EC_TIMEOUT_COUNT        1000
#define EC_STATUS_PORT          0x66
#define EC_CMD_PORT             0x66
#define EC_DATA_PORT            0x62
#define EC_OBF                  0x01
#define EC_IBF                  0x02
#define EC_CMD_READ_VERSION     0xD0
#define EC_CMD_READ_INDEX_DATA  0x80

#define ITE_SET_INDEX_OFFSET      0xA2 // адрес
#define ITE_SET_INDEX_PAGE        0xA3 // страница
#define ITE_READ_DATA_CMD         0xA4 // прочитать данные
#define ITE_WRITE_DATA_CMD        0xA5 // записать данные

EFI_STATUS
WaitEcOBF()
{
	UINT8   EcStsPort;
	UINT32  timeout = EC_TIMEOUT_COUNT;
	do {
		EcStsPort = IoRead8(EC_STATUS_PORT);
		timeout--;
	} while (!(EcStsPort & EC_OBF) && timeout > 0);

	if (timeout == 0) {
		return EFI_UNSUPPORTED;
	}
	return EFI_SUCCESS;
}

EFI_STATUS
WaitEcIBE()
{
	UINT8  EcStsPort;
	UINT32  timeout = EC_TIMEOUT_COUNT;

	do {
		EcStsPort = IoRead8(EC_CMD_PORT);
		timeout--;
	} while ((EcStsPort & EC_IBF) && timeout > 0);

	if (timeout == 0) {
		return EFI_UNSUPPORTED;
	}
	return EFI_SUCCESS;
}

EFI_STATUS
ReadEcData(UINT8* EcData8)
{
	EFI_STATUS Status;

	Status = WaitEcOBF();
	if (!EFI_ERROR(Status)) {
		*EcData8 = IoRead8(EC_DATA_PORT);
		return EFI_SUCCESS;
	}

	return EFI_UNSUPPORTED;
}

EFI_STATUS
WriteEcCmd(UINT8 EcCmd)
{
	EFI_STATUS Status;

	Status = WaitEcIBE();

	if (!EFI_ERROR(Status)) {
		IoWrite8(EC_CMD_PORT, EcCmd);
		WaitEcIBE();
		return EFI_SUCCESS;
	}

	return EFI_UNSUPPORTED;
}

EFI_STATUS
WriteEcData(UINT8 EcData)
{
	EFI_STATUS Status;

	Status = WaitEcIBE();

	if (!EFI_ERROR(Status)) {
		IoWrite8(EC_DATA_PORT, EcData);
		WaitEcIBE();
		return EFI_SUCCESS;
	}

	return EFI_UNSUPPORTED;
}

EFI_STATUS
ReadEcIndexData(UINT16 EcDataIndex, UINT8* EcData8)
{
	EFI_STATUS Status;

	Status = WriteEcCmd(EC_CMD_READ_INDEX_DATA);

	if (!EFI_ERROR(Status)) {
		Status = WriteEcData((UINT8)(EcDataIndex & 0xFF));
		if (!EFI_ERROR(Status)) {
			Status = WriteEcData((UINT8)((EcDataIndex >> 8) & 0xFF));
			if (!EFI_ERROR(Status)) {
				Status = ReadEcData(EcData8);
			}
		}


	}
	return Status;
}

EFI_STATUS
EcIbFree()
{
	UINT8   Status;
	do {
		Status = IoRead8(0x66);
		IoWrite8(0x80, 0xEB);
	} while (Status & 2);
	return EFI_SUCCESS;

}

EFI_STATUS
EcObFull()
{
	UINT8    Status;
	do {
		Status = IoRead8(0x66);
		IoWrite8(0x80, 0xEC);
	} while (!(Status & 1));

	return EFI_SUCCESS;
}

UINT8
BramPageAddress(UINT8 Page, UINT8 Address)
{
	UINT8 Bram;

	EcIbFree();
	IoWrite8(0x66, ITE_SET_INDEX_PAGE);
	EcIbFree();
	IoWrite8(0x62, Page);
	EcIbFree();
	IoWrite8(0x66, ITE_SET_INDEX_OFFSET);
	EcIbFree();
	IoWrite8(0x62, Address);
	EcIbFree();
	IoWrite8(0x66, ITE_READ_DATA_CMD);
	EcObFull();
	Bram = IoRead8(0x62);
	return Bram;
}

VOID
BRAMReader(
	VOID
)
{
	if (GetDeviceID() == 0x5570) {
		UINT8                      EC_MainVersion;
		UINT8                      EC_SubVersion;
		UINT8                      EC_KBCVersion;
		UINT8                      EC_ModelVersion;
		UINT8                      EC_VersionSuffix;
		UINT8                      EC_BuildIndex;


		UINT8                       EcBuildYear;
		UINT8                       EcBuildMonth;
		UINT8                       EcBuildDay;
		UINT8                       EcBuildHour;
		UINT8                       EcBuildMin;
		UINT8                       EcBuildSec;
		
		UINT8 Bram;
		UINT16 i;
		UINT16 count = 0;


		EcIbFree();
		IoWrite8(0x66, 0x80);
		EcIbFree();
		IoWrite8(0x62, 0x93);
		EcObFull();

		Print(L"\nBRAM Reader\n");

		ReadEcIndexData(0x0400, &EC_MainVersion);
		ReadEcIndexData(0x0401, &EC_SubVersion);
		ReadEcIndexData(0x0402, &EC_KBCVersion);
		ReadEcIndexData(0x0403, &EC_ModelVersion);
		ReadEcIndexData(0x0404, &EC_VersionSuffix);

		ReadEcIndexData(0x0408, &EcBuildHour);
		ReadEcIndexData(0x0409, &EcBuildMin);
		ReadEcIndexData(0x040A, &EcBuildSec);
		ReadEcIndexData(0x040C, &EcBuildYear);
		ReadEcIndexData(0x040D, &EcBuildMonth);
		ReadEcIndexData(0x040E, &EcBuildDay);

		ReadEcIndexData(0x040F, &EC_BuildIndex);
		Print(L"BuildIndex = %d\n", EC_BuildIndex);

		if (EC_BuildIndex == 0)
		{
			Print(L"%d.%d-%c%d\n", EC_MainVersion, EC_SubVersion, EC_VersionSuffix, EC_ModelVersion);
		}
		else 
		{
			Print(L"%d.%d-%c#%d:%d\n", EC_MainVersion, EC_SubVersion, EC_VersionSuffix, EC_BuildIndex, EC_ModelVersion);
		}

		Print(L"%02d/%02d/%04d %02d:%02d:%02d\n", EcBuildMonth, EcBuildDay, 2000 + EcBuildYear, EcBuildHour, EcBuildMin, EcBuildSec);


		Print(L"    ");

		for (i = 0x0400; i <= 0x04FF; i++) {
			ReadEcIndexData(i, &Bram);
			Print(L"%5s%04x", L"", i);
			count++;

			if (count == 8) {
				Print(L"\n");
				Print(L"[10]");
				for (UINT16 j = i - 7; j <= i; j++) {
					ReadEcIndexData(j, &Bram);
					Print(L"%9d", Bram);	
				}
				Print(L"\n");
				Print(L"[16]");
				for (UINT16 j = i - 7; j <= i; j++) {
					ReadEcIndexData(j, &Bram);
						Print(L"%7s%02x", L"", Bram);
				}
				Print(L"\n\n");

				Print(L"    ");
				count = 0;
			}
		}
	}



	if (GetDeviceID() == 0x5571) {
		UINT8 Bram;
		UINT8 i;
		UINT16 count = 0;

		Print(L"\nBRAM Reader\n");
		Print(L"\nPage 4\n");

		Print(L"    ");

		for (i = 0x00; i < 0xF8; i++) {
			Bram = BramPageAddress(0x04, i);
			Print(L"%5s%04x", L"", i);
			count++;

			if (count == 8) {
				Print(L"\n");
				Print(L"[10]");
				for (UINT16 j = i - 7; j <= i; j++) {
					ReadEcIndexData(j, &Bram);
					Print(L"%9d", Bram);
				}
				Print(L"\n");
				Print(L"[16]");
				for (UINT16 j = i - 7; j <= i; j++) {
					ReadEcIndexData(j, &Bram);
					Print(L"%7s%02x", L"", Bram);
				}
				Print(L"\n\n");

				Print(L"    ");
				count = 0;
			}
			
		}

		Print(L"\nPage 7\n");
		for (i = 0x00; i < 0xF8; i++) {
			Bram = BramPageAddress(0x07, i);
			Print(L"%5s%04x", L"", i);
			count++;

			if (count == 8) {
				Print(L"\n");
				Print(L"[10]");
				for (UINT16 j = i - 7; j <= i; j++) {
					ReadEcIndexData(j, &Bram);
					Print(L"%9d", Bram);
				}
				Print(L"\n");
				Print(L"[16]");
				for (UINT16 j = i - 7; j <= i; j++) {
					ReadEcIndexData(j, &Bram);
					Print(L"%7s%02x", L"", Bram);
				}
				Print(L"\n\n");

				Print(L"    ");
				count = 0;
			}
		}

		Print(L"\nPage 14\n");
		for (i = 0x00; i < 0xF8; i++) {
			Bram = BramPageAddress(0x14, i);
			Print(L"%5s%04x", L"", i);
			count++;

			if (count == 8) {
				Print(L"\n");
				Print(L"[10]");
				for (UINT16 j = i - 7; j <= i; j++) {
					ReadEcIndexData(j, &Bram);
					Print(L"%9d", Bram);
				}
				Print(L"\n");
				Print(L"[16]");
				for (UINT16 j = i - 7; j <= i; j++) {
					ReadEcIndexData(j, &Bram);
					Print(L"%7s%02x", L"", Bram);
				}
				Print(L"\n\n");

				Print(L"    ");
				count = 0;
			}
		}
	}
}


SHELL_STATUS
EFIAPI
ShellCommandRunChipinfo
(
	IN EFI_HANDLE        ImageHandle,
	IN EFI_SYSTEM_TABLE* SystemTable
)
{
	EFI_STATUS      Status;
	SHELL_STATUS    ShellStatus;
	LIST_ENTRY* Package;
	CHAR16* ProblemParam;

	ShellStatus = SHELL_SUCCESS;
	Status = EFI_SUCCESS;
	Package = NULL;

	Status = ShellInitialize();
	ASSERT_EFI_ERROR(Status);

	Status = CommandInit();
	ASSERT_EFI_ERROR(Status);


	Status = ShellCommandLineParse(ParamList, &Package, &ProblemParam, TRUE);
	if (EFI_ERROR(Status)) {
		if ((Status == EFI_VOLUME_CORRUPTED) && (ProblemParam != NULL)) {
			ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_PROBLEM), gShellDebug1HiiHandle, L"chipinfo", ProblemParam);
			FreePool(ProblemParam);
			ShellStatus = SHELL_INVALID_PARAMETER;
		}
		else {
			ASSERT(FALSE);
		}
	}
	else {
		if (ShellCommandLineGetCount(Package) > 1) {
			ShellPrintHiiEx(-1, -1, NULL, STRING_TOKEN(STR_GEN_TOO_MANY), gShellDebug1HiiHandle, L"chipinfo");
			ShellStatus = SHELL_INVALID_PARAMETER;
			goto Done;
		}
	}
	SioMotherboard();
	GetDeviceID();
	PrintLdnStatus();
	HwmBank();
	BRAMReader();
	ShellStatus = SHELL_SUCCESS;
Done:
	return ShellStatus;
}
