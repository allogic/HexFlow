#ifndef HF_FILE_SYSTEM_H
#define HF_FILE_SYSTEM_H

void HF_FileSystemReadText(char const *FileName, char **Buffer, long long unsigned *Size);
void HF_FileSystemReadBinary(char const *FileName, char **Buffer, long long unsigned *Size);

void HF_FileSystemWriteText(char const *FileName, char const *Buffer, long long unsigned Size);
void HF_FileSystemWriteBinary(char const *FileName, char const *Buffer, long long unsigned Size);

#endif // HF_FILE_SYSTEM_H