#ifndef HF_FILESYSTEM_H
#define HF_FILESYSTEM_H

void HF_FileSystemReadText(char const *FileName, char **Buffer, long long unsigned *Size);
void HF_FileSystemReadBinary(char const *FileName, char **Buffer, long long unsigned *Size);

void HF_FileSystemWriteText(char const *FileName, char const *Buffer, long long unsigned Size);
void HF_FileSystemWriteBinary(char const *FileName, char const *Buffer, long long unsigned Size);

#endif // HF_FILESYSTEM_H