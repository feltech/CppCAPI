#pragma once
#include <dlfcn.h>

#include <filesystem>
#include <string>

namespace feltplugin::service
{
using PluginHandle = decltype(dlopen("", 0));
using SymHandle = decltype(dlsym(nullptr, ""));

class PluginLoader
{
public:
	// TODO: dlmopen
	explicit PluginLoader(char const * file_path, int mode = RTLD_LAZY)
		: file_path_{file_path}, handle_{dlopen(file_path, mode)}
	{
		if (!handle_)
			throw std::filesystem::filesystem_error{
				std::string{"Failed to load '"} + file_path + "': " + dlerror(),
				std::make_error_code(std::errc::io_error)};
	}

	template <typename Symbol>
	Symbol load_symbol(char const * name)
	{
		SymHandle sym = dlsym(handle_, name);
		if (!sym)
			throw std::filesystem::filesystem_error{
				std::string{"Failed to find symbol '"} + name + "' in '" + file_path_ +
					"': " + dlerror(),
				std::make_error_code(std::errc::bad_address)};

		return reinterpret_cast<Symbol>(sym);
	}

	template <class Adapter, typename... Args>
	Adapter load_adapter(char const* suite_factory_name, Args&&... args)
	{
		auto const suite_factory =
			load_symbol<typename Adapter::SuiteFactory>( suite_factory_name);

		return Adapter{suite_factory, std::forward<Args>(args)...};
	}

private:
	std::string file_path_;
	PluginHandle handle_;
};
}  // namespace feltplugin::service
