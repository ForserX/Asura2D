#pragma once

namespace Asura::Editor
{
	void Init();
	void Destroy();

	namespace Object
	{
		void CreateFake();
		void UpdateFake();
		void MakeTry();
	}

	namespace UI
	{
		void Destroy();
	}
};