#include "filesystem.h"
#include "plugins.h"

extern IFileSystemEx*g_pFullFileSystem;

bool IFileSystemEx::FileExists(const char *pFileName, const char *pPathID)
{
	return g_pFileSystem->FileExists(pFileName);
}

bool IFileSystemEx::IsFileWritable(char const *pFileName, const char *pPathID)
{
	return true;
}

bool IFileSystemEx::SetFileWritable(char const *pFileName, bool writable, const char *pPathID)
{
	return true;
}

int IFileSystemEx::ReadEx(void* pOutput, int sizeDest, int size, FileHandle_t file)
{
	return g_pFileSystem->Read(pOutput, size, file);
}

bool IFileSystemEx::GetOptimalIOConstraints(FileHandle_t hFile, unsigned *pOffsetAlign, unsigned *pSizeAlign, unsigned *pBufferAlign)
{
	return false;
}

unsigned IFileSystemEx::GetOptimalReadSize(FileHandle_t hFile, unsigned nLogicalSize)
{
	return g_pFileSystem->Size(hFile);
}

void * IFileSystemEx::AllocOptimalReadBuffer(FileHandle_t hFile, unsigned nSize, unsigned nOffset)
{
	return malloc(nSize);
}

void IFileSystemEx::FreeOptimalReadBuffer(void *buffer)
{
	return free(buffer);
}

const char * IFileSystemEx::RelativePathToFullPath(const char *pFileName, const char *pPathID, char *pLocalPath, int localPathBufferSize, PathTypeFilter_t pathFilter, PathTypeQuery_t *pPathType)
{
	pLocalPath[0] = 0;
	return NULL;
}

bool IFileSystemEx::IsDirectory(const char *pFileName, const char *pathID)
{
	DWORD dwAttributes = GetFileAttributesA(pFileName);
	return ((dwAttributes != -1) && (dwAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

bool IFileSystemEx::GetFileTypeForFullPath(char const *pFullPath, wchar_t *buf, size_t bufSizeInBytes)
{
	buf[0] = 0;
	return false;
}

bool IFileSystemEx::FullPathToRelativePathEx(const char *pFullpath, const char *pPathId, char *pRelative, int maxlen)
{
	pRelative[0] = 0;
	return false;
}

const char * IFileSystemEx::FindFirstEx(const char *pWildCard, const char *pPathID, FileFindHandle_t *pHandle)
{
	return g_pFileSystem->FindFirst(pWildCard, pHandle, pPathID);;
}