#pragma once

class Manager_Shader
{
private:
	Manager_Shader(){}

public:
	static Manager_Shader* instance()
	{
		static Manager_Shader instance;
		return &instance;
	}
}