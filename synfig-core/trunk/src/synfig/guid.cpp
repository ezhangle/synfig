/* === S Y N F I G ========================================================= */
/*!	\file guid.cpp
**	\brief Template File
**
**	$Id: guid.cpp,v 1.4 2005/01/13 06:48:39 darco Exp $
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**
**	This package is free software; you can redistribute it and/or
**	modify it under the terms of the GNU General Public License as
**	published by the Free Software Foundation; either version 2 of
**	the License, or (at your option) any later version.
**
**	This package is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
**	General Public License for more details.
**	\endlegal
*/
/* ========================================================================= */

/* === H E A D E R S ======================================================= */

#define HASH_MAP_H <ext/hash_map>
#define SUBTRACT_RNG_H <ext/functional>

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "guid.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ETL/stringf>
#include <functional>

#ifdef _DEBUG
#include "general.h"
#endif

#ifdef HASH_MAP_H
#include HASH_MAP_H
using namespace __gnu_cxx;
#endif

#ifdef SUBTRACT_RNG_H
#include SUBTRACT_RNG_H
using namespace __gnu_cxx;
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace etl;
using namespace synfig;

/* === M A C R O S ========================================================= */

#define MANUAL_GUID_CALC

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

// A fast 32-bit linear congruential random number generator
class quick_rng
{
	unsigned long next;
public:
	quick_rng(unsigned long seed=0):next(seed) { }

	void set_seed(unsigned long x)
	{
		next=x;
	}

	unsigned long i32()
	{
		static const unsigned long a(1664525);
		static const unsigned long c(1013904223);

		return next=next*a+c;
	}

	unsigned long i16()
	{
		return i32()>>16;
	}

	float f()
	{
		static const float m(int(65535));

		return float(i16())/m;
	}

	unsigned long operator()(const unsigned long& m)
	{
		if(m==65536)
			return i16();
		else
		if(m<=65536)
			return i16()%m;
		else
			return i32()%m;
	}
};

#define GUID_RNG quick_rng
//#define GUID_RNG subtractive_rng



#ifdef MANUAL_GUID_CALC
#include <time.h>
static GUID_RNG _a, _b;
static void _set_up_rand_long_long(uint64_t &x);
static void _get_rand_long_long(uint64_t &x);
static void (*get_rand_long_long)(uint64_t&)=_set_up_rand_long_long;
static void _set_up_rand_long_long(uint64_t &x)
{
#ifdef _DEBUG
	synfig::info("Starting up GUID system...");
#endif
	_a=GUID_RNG(time(0)+clock());
	_b=GUID_RNG(clock());
	get_rand_long_long=_get_rand_long_long;
	_get_rand_long_long(x);
}

static void _get_rand_long_long(uint64_t &x)
{
	//subtractive_rng _c(clock());
	unsigned short* data(reinterpret_cast<unsigned short *>(&x));
	data[0]=_a(65536);
	data[1]=_a(65536);
	data[2]=_a(65536);
	data[3]=_a(65536);
}

#else
// Use OS-Dependent method

#ifdef _WIN32
// Win32
static void get_rand_long_long(uint64_t &x)
{
	_GUID* guid(reinterpret_cast<_GUID*>(&x));
	CoCreateGuid(guid);
}

#else
// Unix
static int rand_fd;
static void _set_up_rand_long_long(uint64_t &x);
static void _get_rand_long_long(uint64_t &x);
static void (*get_rand_long_long)(uint64_t&)=_set_up_rand_long_long;
static void _set_up_rand_long_long(uint64_t &x)
{
#ifdef _DEBUG
	synfig::info("Starting up GUID system...");
#endif
	rand_fd=open("/dev/urandom",O_RDONLY);
	get_rand_long_long=_get_rand_long_long;
	_get_rand_long_long(x);
}

static void _get_rand_long_long(uint64_t &x){	read(rand_fd,&x,sizeof(x));}

#endif
#endif



void
synfig::GUID::make_unique()
{
	get_rand_long_long(data.u_64.a);
	get_rand_long_long(data.u_64.b);
}

synfig::GUID
synfig::GUID::hasher(const String& str)
{
#ifdef HASH_MAP_H
	hash<const char*> string_hash_;
	const unsigned int seed(
		string_hash_(
			str.c_str()
		)
	);
#else
	const unsigned int seed(0x3B642879);
	for(int i=0;i<str.size();i++)
	{
		seed^=(seed*str[i])*i
		seed=(seed>>(32-(i%24)))^(seed<<(i%24))
	}
#endif

	GUID_RNG random(seed);
	GUID ret(0);
	ret.data.u_32.a=random(~(unsigned int)0);
	ret.data.u_32.b=random(~(unsigned int)0);
	ret.data.u_32.c=random(~(unsigned int)0);
	ret.data.u_32.d=random(~(unsigned int)0);
	return ret;
}

synfig::GUID
synfig::GUID::hasher(int i)
{
	GUID ret(0);
	GUID_RNG random(i);
	ret.data.u_32.a=random(~(unsigned int)0);
	ret.data.u_32.b=random(~(unsigned int)0);
	ret.data.u_32.c=random(~(unsigned int)0);
	ret.data.u_32.d=random(~(unsigned int)0);
	return ret;
}

String
synfig::GUID::get_string()const
{
	return strprintf("%08X%08X%08X%08X",data.u_32.a,data.u_32.b,data.u_32.c,data.u_32.d);
}

synfig::GUID::GUID(const String &str)
{
	strscanf(str,"%08X%08X%08X%08X",&data.u_32.a,&data.u_32.b,&data.u_32.c,&data.u_32.d);
}
