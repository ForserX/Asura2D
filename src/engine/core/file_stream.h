#pragma once

namespace Asura::FileSystem
{
	class Reader
	{
		char* Data;
		
		size_t Pos;
		size_t Size;

		stl::string File;

	public:
		Reader() = delete;
		Reader(stl::string File);
		Reader(char* Buffer, size_t Size);

		~Reader();

		Reader Extract(size_t Offset);
		void Get(void* Buffer, size_t Offset);

		template<typename T>
		T Get()
		{
			static_assert(!std::is_pointer_v<T>, "Not supported!");

			T Result = {};

			constexpr size_t TypeSize = sizeof(std::remove_reference<T>::type);

			memcpy(&Result, Data + Pos, TypeSize);

			Pos += TypeSize;
			game_assert(!Eof(), "File is end...", Debug::dbg_break());

			return std::move(Result);
		}

		inline void Seek(size_t NewPos)
		{
			Pos = NewPos;
		}
		
		inline size_t Tell()	const { return Pos; }
		inline size_t Elapsed() const { return Size - Pos; }
		inline size_t Lenght()	const { return Pos; }
		inline bool   Eof()		const { return Pos >= Size; }

		inline void Move(size_t Offset)
		{
			Pos += Offset;
		}

	};

	class Writer
	{
		stl::vector<char> Data;
	public:

		template <typename T>
		void Push(T ValueData)
		{
			static_assert(!std::is_pointer_v<T>, "Not supported!");

			constexpr size_t TypeSize = sizeof(std::remove_reference<T>::type);
			size_t OldSize = Data.size();

			Data.resize(OldSize + TypeSize);
			memcpy(Data.data() + OldSize, &ValueData, TypeSize);
		}

		void Save(stl::string_view FileName);
	};
}