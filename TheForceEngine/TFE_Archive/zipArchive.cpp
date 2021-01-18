#include "zipArchive.h"
#include <TFE_FileSystem/fileutil.h>
#include <TFE_System/system.h>
#include "zip/zip.h"
#include <assert.h>
#include <string>
#include <algorithm>
#include <map>

ZipArchive::~ZipArchive()
{
	close();
}

bool ZipArchive::create(const char *archivePath)
{
	return false;
}

bool ZipArchive::open(const char *archivePath)
{
	strcpy(m_archivePath, archivePath);
	m_fileHandle = nullptr;
	m_curFile = INVALID_FILE;
	m_entryCount = 0;

	struct zip_t* zip = zip_open(archivePath, 0, 'r');
	if (!zip)
	{
		TFE_System::logWrite(LOG_ERROR, "zipArchive", "Cannot open Zip Archive '%s'", archivePath);
		return false;
	}

	// Read the directory.
	m_entryCount = zip_total_entries(zip);
	if (m_entryCount <= 0)
	{
		TFE_System::logWrite(LOG_ERROR, "zipArchive", "Zip Archive '%s' is empty.", archivePath);
		zip_close(zip);
		return false;
	}

	m_entries = new ZipEntry[m_entryCount];
	for (s32 i = 0; i < m_entryCount; i++)
	{
		if (zip_entry_openbyindex(zip, i) != 0)
		{
			TFE_System::logWrite(LOG_ERROR, "zipArchive", "Cannot read entry '%d' from archive '%s'", i, archivePath);
			zip_close(zip);
			return false;
		}

		m_entries[i].isDir = (zip_entry_isdir(zip) == 1);
		m_entries[i].name = zip_entry_name(zip);
		m_entries[i].length = (size_t)zip_entry_size(zip);
		zip_entry_close(zip);
	}
	zip_close(zip);
	return true;
}

void ZipArchive::close()
{
	closeFile();

	delete[] m_entries;
	m_entries = nullptr;
	m_curFile = INVALID_FILE;
}

// File Access
bool ZipArchive::openFile(const char *file)
{
	m_curFile = getFileIndex(file);
	if (m_curFile != INVALID_FILE)
	{
		m_fileHandle = zip_open(m_archivePath, 0, 'r');
		if (zip_entry_openbyindex((struct zip_t*)m_fileHandle, m_curFile) != 0)
		{
			TFE_System::logWrite(LOG_ERROR, "zipArchive", "Cannot open file '%s' from archive '%s'", file, m_archivePath);
			m_curFile = INVALID_FILE;
			zip_close((struct zip_t*)m_fileHandle);
			m_fileHandle = nullptr;
		}
	}
	else
	{
		TFE_System::logWrite(LOG_ERROR, "zipArchive", "Cannot find file '%s' from archive '%s'", file, m_archivePath);
	}
	return m_curFile != INVALID_FILE;
}

bool ZipArchive::openFile(u32 index)
{
	m_curFile = INVALID_FILE;
	if (index <= (u32)m_entryCount)
	{
		// Open the system file.
		m_fileHandle = zip_open(m_archivePath, 0, 'r');
		// Open the file entry itself.
		m_curFile = index;
		if (zip_entry_openbyindex((struct zip_t*)m_fileHandle, index) != 0)
		{
			TFE_System::logWrite(LOG_ERROR, "zipArchive", "Cannot open file '%s' from archive '%s'", m_entries[index].name.c_str(), m_archivePath);
			m_curFile = INVALID_FILE;
			zip_close((struct zip_t*)m_fileHandle);
			m_fileHandle = nullptr;
		}
	}
	else
	{
		TFE_System::logWrite(LOG_ERROR, "zipArchive", "Cannot find file from index '%d' from archive '%s'", index, m_archivePath);
	}
	return m_curFile != INVALID_FILE;
}

void ZipArchive::closeFile()
{
	if (m_fileHandle)
	{
		// Close the file entry.
		zip_entry_close((struct zip_t*)m_fileHandle);
		// Close the system file.
		zip_close((struct zip_t*)m_fileHandle);
		m_fileHandle = nullptr;
	}
	m_curFile = INVALID_FILE;
}

bool ZipArchive::fileExists(const char *file)
{
	return getFileIndex(file) != INVALID_FILE;
}

bool ZipArchive::fileExists(u32 index)
{
	return index < (u32)m_entryCount;
}

u32 ZipArchive::getFileIndex(const char* file)
{
	for (s32 i = 0; i < m_entryCount; i++)
	{
		if (strcasecmp(file, m_entries[i].name.c_str()) == 0)
		{
			return i;
		}
	}
	return INVALID_FILE;
}

size_t ZipArchive::getFileLength()
{
	if (m_curFile == INVALID_FILE) { return 0; }
	return m_entries[m_curFile].length;
}

bool ZipArchive::readFile(void *data, size_t size)
{
	if (m_curFile == INVALID_FILE) { return false; }
	if (size == 0) { size = m_entries[m_curFile].length; }

	const size_t sizeToRead = std::min(size, m_entries[m_curFile].length);
	return zip_entry_noallocread((struct zip_t*)m_fileHandle, data, size) > 0;
}

// Directory
u32 ZipArchive::getFileCount()
{
	return m_entryCount;
}

const char* ZipArchive::getFileName(u32 index)
{
	if (index >= (u32)m_entryCount) { return nullptr; }
	return m_entries[index].name.c_str();
}

size_t ZipArchive::getFileLength(u32 index)
{
	if (index >= (u32)m_entryCount) { return 0; }
	return m_entries[index].length;
}

// Edit
void ZipArchive::addFile(const char* fileName, const char* filePath)
{
}