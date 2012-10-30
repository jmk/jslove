/**
* Copyright (c) 2006-2012 LOVE Development Team
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
**/

#ifndef LOVE_VARIANT_H
#define LOVE_VARIANT_H

#include <common/runtime.h>
#include <common/Object.h>

#include <cstring>

#ifdef JSLOVE
struct JSLPrivateAccess;
#endif

namespace love
{
	class Variant : public love::Object
	{
	private:
#ifdef JSLOVE
                friend struct JSLPrivateAccess;
#endif
		enum Type
		{
			UNKNOWN = 0,
			BOOLEAN,
			NUMBER,
			CHARACTER,
			STRING,
#ifndef JSLOVE
			LUSERDATA,
			FUSERDATA
#endif
		} type;
		union
		{
			bool boolean;
			char character;
			double number;
			struct {
				const char *str;
				size_t len;
			} string;
			void *userdata;
		} data;
#ifndef JSLOVE
		love::Type udatatype;
		bits flags;
#endif

	public:
		
		Variant(bool boolean);
		Variant(double number);
		Variant(const char *string, size_t len);
		Variant(char c);
#ifndef JSLOVE
		Variant(void *userdata);
		Variant(love::Type udatatype, void *userdata);
#endif
		virtual ~Variant();

#ifndef JSLOVE
		static Variant *fromLua(lua_State *L, int n);
		void toLua(lua_State *L);
#endif
	}; // Variant
} // love

#endif // LOVE_VARIANT_H
