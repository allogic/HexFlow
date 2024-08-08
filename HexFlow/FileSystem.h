#ifndef HF_FILESYSTEM_H
#define HF_FILESYSTEM_H

void HF_FileSystemReadFile(char const *Mode, char const *FileName, char **Buffer, long long unsigned *Size);
void HF_FileSystemWriteFile(char const *Mode, char const *FileName, char const *Buffer, long long unsigned Size);

#endif