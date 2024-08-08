#include <stdio.h>
#include <stdlib.h>

#include <HexFlow/FileSystem.h>

void HF_FileSystemReadFile(char const *Mode, char const *FileName, char **Buffer, long long unsigned *Size)
{
	FILE *File = fopen(FileName, Mode);

	char unsigned FoundBinary = 0;
	char const* ModeChar = Mode;

	while (*ModeChar++ != '\0' && !FoundBinary) FoundBinary = *ModeChar == 'b';

	fseek(File, 0, SEEK_END);
	*Size = ftell(File);
	*Buffer = calloc((*Size) + ((FoundBinary) ? 0 : 1), sizeof(char unsigned));
	fseek(File, 0, SEEK_SET);

	fread(*Buffer, 1, *Size, File);

	fclose(File);
}

void HF_FileSystemWriteFile(char const *Mode, char const *FileName, char const *Buffer, long long unsigned Size)
{
	FILE *File = fopen(FileName, Mode);

	fseek(File, 0, SEEK_SET);

	fwrite(Buffer, 1, Size, File);

	fclose(File);
}