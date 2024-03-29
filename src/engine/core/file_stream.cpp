#include "pch.h"

using namespace Asura;

FileSystem::Reader::Reader(stl::string FileName) : Pos(0)
{
	Platform::NormalizePath(FileName);

	File = std::move((FileSystem::ContentDir() / FileName).generic_string());

	std::fstream FileStream;
	FileStream.open(File, std::ios_base::in | std::ios::binary);
	game_assert(FileStream.is_open(), "Broken file...", return);

	FileStream.seekp(0, std::ios::end);
	Size = FileStream.tellg();
	Data = new char[Size];

	FileStream.seekp(0, std::ios::beg);
	FileStream.read(Data, Size);
	FileStream.close();
}

FileSystem::Reader::Reader(char* Buffer, size_t BuffSize) : Pos(0), Size(BuffSize)
{
	Data = new char[BuffSize];
	memcpy(Data, Buffer, BuffSize);

	File = "memory";
}

FileSystem::Reader::~Reader()
{
	delete[] Data;
}

FileSystem::Reader FileSystem::Reader::Extract(size_t Offset)
{
	Reader NewPtr(&Data[Pos], Offset);
	return std::move(NewPtr);
}

void FileSystem::Reader::Get(void* Buffer, size_t Offset)
{
	memcpy(Buffer, Data + Pos, Offset);

	Pos += Offset;
	game_assert(Pos <= Size, "File is end...", Debug::dbg_break());
}

void FileSystem::Writer::Save(stl::string_view FileName)
{
	stl::string Path = FileName.data();
	Platform::NormalizePath(Path);

	auto File = std::move((FileSystem::ContentDir() / Path).generic_string());

	std::fstream FileStream;
	FileStream.open(File, std::ios_base::out | std::ios::binary);
	FileStream.write(Data.data(), Data.size());
	FileStream.close();
}
