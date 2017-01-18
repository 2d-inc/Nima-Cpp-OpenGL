#ifndef _NIMA_GLSHADERRESOURCES_HPP_
#define _NIMA_GLSHADERRESOURCES_HPP_

#include <unordered_map>
#include <string>

namespace nima
{
	class GLShader;
	class GLShaderResources
	{
		private:
			std::unordered_map<std::string, GLShader*> m_LoadedShaders;

		public:
			GLShader* get(std::string name);
			~GLShaderResources();
	};
}

#endif