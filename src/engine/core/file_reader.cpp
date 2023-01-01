#include "pch.h"

using namespace Asura;

Reader::Reader(stl::string FileName) : Pos(0)
{
	File = std::move((FileSystem::ContentDir() / FileName).generic_string());

	std::fstream FileStream;
	FileStream.open(FileName, std::ios_base::in | std::ios::binary);
	game_assert(FileStream.is_open(), "Broken file...", return);

	Size = std::filesystem::file_size(FileName);
	Data = new char[Size];

	FileStream.read(Data, Size);
	FileStream.close();
}

Reader::Reader(char* Buffer, size_t BuffSize) : Pos(0), Size(BuffSize)
{
	Data = new char[BuffSize];
	memcpy(Data, Buffer, BuffSize);

	File = "memory";
}

Reader::~Reader()
{
	delete[] Data;
}

Reader Reader::Extract(size_t Offset)
{
	Reader NewPtr(&Data[Pos], Offset);
	return std::move(NewPtr);
}