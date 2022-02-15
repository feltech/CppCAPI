
#include <feltplugin/errors.hpp>
#include <iostream>

#include "feltplugindemoplugin_export.h"
#include "receiver.hpp"

extern "C"
{
	FELTPLUGINDEMOPLUGIN_EXPORT fp_ErrorCode update_dict(fp_ErrorMessage err, fpdemo_StringDict_h hdict)
	{
		using namespace feltplugindemoplugin;
		return feltplugin::wrap_exception(
			err,
			[hdict]
			{
				StringDict dict{hdict};
				dict.insert("pluginkey", "pluginvalue");

				try
				{
					[[maybe_unused]] auto value = dict.at("missingkey");
				}
				catch (std::runtime_error const & ex)
				{
					std::cerr << "Exception propagated from host and caught within plugin: "
							  << ex.what() << "\n";
				}
				throw std::runtime_error{"Boom!"};
			});
	}
}