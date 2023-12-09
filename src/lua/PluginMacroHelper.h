
#ifdef HOLMGARD_USE_PLUGINS
#include "Plugins.h"

#define HOLMGARD_MAKE_ENUM(name, _) \
	PLUG_##name,

#define HOLMGARD_MAKE_INIT(name, _) \
	libraries[LibraryID::PLUG_##name] = new name();

#define HOLMGARD_MAKE_REQUIRE(qname, reqname) \
	else if(name == reqname) \
		return LibraryID::PLUG_##qname;
#endif



