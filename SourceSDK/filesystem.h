//===== Copyright ?1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef IFILESYSTEMEX_H
#define IFILESYSTEMEX_H

#ifdef _WIN32
#pragma once
#endif

#include <interface.h>
#include <IFileSystem.h>

#ifndef NULL
#define NULL 0
#endif

typedef void *FileHandle_t;
typedef int FileFindHandle_t;
typedef int WaitForResourcesHandle_t;

enum PathTypeFilter_t
{
	FILTER_NONE = 0,
	FILTER_CULLPACK = 1,
	FILTER_CULLNONPACK = 2,
};

typedef unsigned int PathTypeQuery_t;

class IFileSystemEx : public IFileSystem
{
public:
	bool FileExists(const char *pFileName, const char *pPathID = 0);
	bool IsFileWritable(char const *pFileName, const char *pPathID = 0);
	bool SetFileWritable(char const *pFileName, bool writable, const char *pPathID = 0);
	int ReadEx(void* pOutput, int sizeDest, int size, FileHandle_t file);

	bool IsDirectory(const char *pFileName, const char *pathID);
	bool GetFileTypeForFullPath(char const *pFullPath, wchar_t *buf, size_t bufSizeInBytes);

	bool GetOptimalIOConstraints(FileHandle_t hFile, unsigned *pOffsetAlign, unsigned *pSizeAlign, unsigned *pBufferAlign);
	unsigned GetOptimalReadSize(FileHandle_t hFile, unsigned nLogicalSize);
	void *AllocOptimalReadBuffer(FileHandle_t hFile, unsigned nSize = 0, unsigned nOffset = 0);
	void FreeOptimalReadBuffer(void *buffer);
	const char *RelativePathToFullPath( const char *pFileName, const char *pPathID, char *pLocalPath, int localPathBufferSize, PathTypeFilter_t pathFilter = FILTER_NONE, PathTypeQuery_t *pPathType = NULL);

	bool FullPathToRelativePathEx(const char *pFullpath, const char *pPathId, char *pRelative, int maxlen);
	const char *FindFirstEx(const char *pWildCard, const char *pPathID, FileFindHandle_t *pHandle);
};

#define FILESYSTEM_INTERFACE_VERSION "VFileSystem009"

#endif