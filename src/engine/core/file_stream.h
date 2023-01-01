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

		template<typename T>
		T Get()
		{
			static_assert(!std::is_pointer_v<T>, "Not supported!");

			T Result = 0;

			constexpr size_t TypeSize = sizeof(std::remove_reference<T>::type);

			memcpy(&Result, Data + Pos, TypeSize);

			Pos += TypeSize;
			game_assert(!eof(), "File is end...", Debug::dbg_break());

			return Result;
		}

		inline void Seek(size_t NewPos)
		{
			Pos = NewPos;
		}
		
		inline void Move(size_t Offset)
		{
			Pos += Offset;
		}

		inline bool eof() const
		{
			return Pos > Size;
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