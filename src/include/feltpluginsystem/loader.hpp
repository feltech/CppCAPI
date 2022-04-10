// Copyright 2022 David Feltell
// SPDX-License-Identifier: MIT
/**
 * Contains the Plugin class used for loading DSOs and their symbols.
 */
#pragma once
#include <dlfcn.h>

#include <filesystem>
#include <string>

namespace feltplugin
{
/// Result of calling `dlopen`.
using PluginHandle = decltype(dlopen("", 0));
/// Result of calling `dlsym`.
using SymHandle = decltype(dlsym(nullptr, ""));

/**
 * RAII wrapper and utilities for loading a DSO and its symbols.
 */
class Loader
{
public:
	/**
	 * Constructor - load the plugin and throw `filesystem_error` on failure.
	 *
	 * @param file_path Path to DSO file to load.
	 * @param mode Mode to pass to `dlopen`.
	 */
	// TODO: dlmopen
	explicit Loader(char const * file_path, int mode = RTLD_LAZY)
		: file_path_{file_path}, handle_{dlopen(file_path, mode)}
	{
		if (!handle_)
			throw std::filesystem::filesystem_error{
				std::string{"Failed to load '"} + file_path + "': " + dlerror(),
				std::make_error_code(std::errc::io_error)};
	}

	/// Destructor - call `dlclose` on the handle.
	~Loader()
	{
		if (handle_)
			dlclose(handle_);
		handle_ = nullptr;
	}

	/**
	 * Load an arbitrary symbol from the plugin DSO.
	 *
	 * @tparam Symbol Type of the symbol.
	 * @param name Name of the symbol.
	 * @return Loaded symbol.
	 */
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

	/**
	 * Load a function pointer suite factory from the plugin DSO and instantiate an adapter class
	 * around it.
	 *
	 * @tparam Adapter client::HandleAdapter type to instantiate.
	 * @tparam Args Additional argument types to pass to Adapter constructor.
	 * @param suite_factory_name Symbol name in DSO of suite factory function.
	 * @param args Additional arguments to pass to Adapter constructor.
	 * @return Adapter instance.
	 */
	template <class Adapter, typename... Args>
	Adapter load_adapter(char const * suite_factory_name, Args &&... args)
	{
		auto const suite_factory = load_symbol<typename Adapter::SuiteFactory>(suite_factory_name);

		return Adapter{suite_factory, std::forward<Args>(args)...};
	}

private:
	std::string file_path_;
	PluginHandle handle_;
};
}  // namespace feltplugin
