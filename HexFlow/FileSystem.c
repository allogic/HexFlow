#include <stdio.h>
#include <stdlib.h>

#include <HexFlow/FileSystem.h>
#include <HexFlow/Memory.h>

void HF_FileSystemReadText(char const *FileName, char **Buffer, long long unsigned *Size)
{
	FILE *File = fopen(FileName, "r");

	fseek(File, 0, SEEK_END);

	*Size = ftell(File);
	*Buffer = HF_MemoryAlloc((*Size) + 1, 0);

	fseek(File, 0, SEEK_SET);
	fread(*Buffer, 1, *Size, File);
	fclose(File);
}

void HF_FileSystemReadBinary(char const *FileName, char **Buffer, long long unsigned *Size)
{
	FILE *File = fopen(FileName, "rb");

	fseek(File, 0, SEEK_END);

	*Size = ftell(File);
	*Buffer = HF_MemoryAlloc(*Size, 0);

	fseek(File, 0, SEEK_SET);
	fread(*Buffer, 1, *Size, File);
	fclose(File);
}

void HF_FileSystemWriteText(char const *FileName, char const *Buffer, long long unsigned Size)
{
	FILE *File = fopen(FileName, "w");

	fseek(File, 0, SEEK_SET);
	fwrite(Buffer, 1, Size, File);
	fclose(File);
}

void HF_FileSystemWriteBinary(char const *FileName, char const *Buffer, long long unsigned Size)
{
	FILE *File = fopen(FileName, "wb");

	fseek(File, 0, SEEK_SET);
	fwrite(Buffer, 1, Size, File);
	fclose(File);
}