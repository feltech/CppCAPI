#pragma once
#include <dlfcn.h>

#include <filesystem>
#include <string>

namespace feltplugin::host
{
using PluginHandle = decltype(dlopen("", 0));
using SymHandle = decltype(dlsym(nullptr, ""));

class Plugin
{
public:
	// TODO: dlmopen
	explicit Plugin(char const * file_path, int mode = RTLD_LAZY) : file_path_{file_path},
		  handle_{dlopen(file_path, mode)}
	{
		if (!handle_)
			throw std::filesystem::filesystem_error{
				std::string{"Failed to load '"} + file_path + "': " + dlerror(),
				std::make_error_code(std::errc::io_error)};
	}

	template <typename Symbol>
	Symbol load_symbol(char const* name)
	{
		SymHandle sym = dlsym(handle_, name);
		if (!sym)
			throw std::filesystem::filesystem_error{
				std::string{"Failed to find symbol '"} + name + "' in '" + file_path_ + "'",
				std::make_error_code(std::errc::bad_address)};

		return reinterpret_cast<Symbol>(sym);
	}

private:
	std::string file_path_;
	PluginHandle handle_;
};
}  // namespace feltplugin::host
