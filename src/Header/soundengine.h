#ifndef __FMOD_SOUNDEING_

#include <fmod.hpp>
#include <fmod_errors.h>
namespace FModEngine {
	void InitFModLibrary();
	void FreeFModLibrary();
	FMOD_SYSTEM* GetSysTem();
}

#define __FMOD_SOUNDEING_
#endif // !__FMOD_SOUNDEING_