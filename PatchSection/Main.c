#include <stdio.h>
#include <stdlib.h>

#ifdef HF_PLATFORM_WINDOWS

	#include <Windows.h>

#else

	#error "Platform not supported"

#endif

static void HF_ReadFile(char const *Mode, char const *FileName, char **Buffer, long long unsigned *Size);
static void HF_WriteFile(char const *Mode, char const *FileName, char const *Buffer, long long unsigned Size);

static void HF_ReadSection(char const *Image, char const *SectionName, char **Buffer, long long unsigned *Size);
static void HF_WriteSection(char *Image, char const *SectionName, char const *Buffer, long long unsigned Size);

int main(int Argc, char **Argv)
{
	if (strcmp("Write", Argv[1]) == 0)
	{
		char *ImageBuffer = 0;
		char *SectionBuffer = 0;

		long long unsigned ImageSize = 0;
		long long unsigned SectionSize = 0;

		HF_ReadFile("rb", Argv[2], &ImageBuffer, &ImageSize);
		HF_ReadFile(Argv[3], Argv[4], &SectionBuffer, &SectionSize);
		HF_WriteSection(ImageBuffer, Argv[5], SectionBuffer, SectionSize);
		HF_WriteFile("wb", Argv[2], ImageBuffer, ImageSize);

		free(ImageBuffer);
		free(SectionBuffer);
	}

	if (strcmp("Read", Argv[1]) == 0)
	{
		char *ImageBuffer = 0;
		char *SectionBuffer = 0;

		long long unsigned ImageSize = 0;
		long long unsigned SectionSize = 0;

		HF_ReadFile("rb", Argv[2], &ImageBuffer, &ImageSize);
		HF_ReadSection(ImageBuffer, Argv[5], &SectionBuffer, &SectionSize);
		HF_WriteFile(Argv[3], Argv[4], SectionBuffer, SectionSize);

		free(ImageBuffer);
		free(SectionBuffer);
	}

	return 0;
}

static void HF_ReadFile(char const *Mode, char const *FileName, char **Buffer, long long unsigned *Size)
{
	FILE *File = fopen(FileName, Mode);

	fseek(File, 0, SEEK_END);
	*Size = ftell(File);
	*Buffer = malloc(*Size);
	fseek(File, 0, SEEK_SET);

	fread(*Buffer, 1, *Size, File);

	fclose(File);
}

static void HF_WriteFile(char const *Mode, char const *FileName, char const *Buffer, long long unsigned Size)
{
	FILE *File = fopen(FileName, Mode);

	fseek(File, 0, SEEK_SET);

	fwrite(Buffer, 1, Size, File);

	fclose(File);
}

static void HF_ReadSection(char const *Image, char const *SectionName, char **Buffer, long long unsigned *Size)
{
#ifdef HF_PLATFORM_WINDOWS

	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)Image;
	PIMAGE_NT_HEADERS64 NTHeaders = (PIMAGE_NT_HEADERS64)((PBYTE)Image + DosHeader->e_lfanew);
	PIMAGE_FILE_HEADER FileHeader = &NTHeaders->FileHeader;
	PIMAGE_SECTION_HEADER SectionHeaders = (PIMAGE_SECTION_HEADER)((PBYTE)NTHeaders + sizeof(IMAGE_NT_HEADERS64));

	for (WORD SectionIndex = 0; SectionIndex < FileHeader->NumberOfSections; SectionIndex++)
	{
		PIMAGE_SECTION_HEADER SectionHeader = &SectionHeaders[SectionIndex];

		if (strcmp(SectionName, SectionHeader->Name) == 0)
		{
			*Size = SectionHeader->SizeOfRawData;
			*Buffer = malloc(*Size);

			memcpy(*Buffer, Image + SectionHeader->PointerToRawData, *Size);
		}
	}

#else

	#error "Platform not supported"

#endif
}

static void HF_WriteSection(char *Image, char const *SectionName, char const *Buffer, long long unsigned Size)
{
#ifdef HF_PLATFORM_WINDOWS

	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)Image;
	PIMAGE_NT_HEADERS64 NTHeaders = (PIMAGE_NT_HEADERS64)((PBYTE)Image + DosHeader->e_lfanew);
	PIMAGE_FILE_HEADER FileHeader = &NTHeaders->FileHeader;
	PIMAGE_SECTION_HEADER SectionHeaders = (PIMAGE_SECTION_HEADER)((PBYTE)NTHeaders + sizeof(IMAGE_NT_HEADERS64));

	for (WORD SectionIndex = 0; SectionIndex < FileHeader->NumberOfSections; SectionIndex++)
	{
		PIMAGE_SECTION_HEADER SectionHeader = &SectionHeaders[SectionIndex];

		if (strcmp(SectionName, SectionHeader->Name) == 0)
		{
			memcpy(Image + SectionHeader->PointerToRawData, Buffer, Size);
		}
	}

#else

	#error "Platform not supported"

#endif
}