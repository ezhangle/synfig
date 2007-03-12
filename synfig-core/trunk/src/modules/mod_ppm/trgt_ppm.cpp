/*! ========================================================================
** Synfig
** ppm Target Module
** $Id: trgt_ppm.cpp,v 1.1.1.1 2005/01/04 01:23:14 darco Exp $
**
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
**
** === N O T E S ===========================================================
**
** ========================================================================= */

/* === H E A D E R S ======================================================= */

#define SYNFIG_TARGET

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "trgt_ppm.h"
#include <synfig/synfig.h>
#include <ETL/stringf>
#include <cstdio>
#include <algorithm>
#include <functional>
#endif

/* === M A C R O S ========================================================= */

using namespace synfig;
using namespace std;
using namespace etl;

/* === G L O B A L S ======================================================= */

SYNFIG_TARGET_INIT(ppm);
SYNFIG_TARGET_SET_NAME(ppm,"ppm");
SYNFIG_TARGET_SET_EXT(ppm,"ppm");
SYNFIG_TARGET_SET_VERSION(ppm,"0.1");
SYNFIG_TARGET_SET_CVS_ID(ppm,"$Id: trgt_ppm.cpp,v 1.1.1.1 2005/01/04 01:23:14 darco Exp $");

/* === M E T H O D S ======================================================= */

ppm::ppm(const char *Filename)
{
	filename=Filename;
	multi_image=false;
	buffer=NULL;
	color_buffer=0;
	set_remove_alpha();
}

ppm::~ppm()
{
	delete [] buffer;
	delete [] color_buffer;
}

bool
ppm::set_rend_desc(RendDesc *given_desc)
{
	//given_desc->set_pixel_format(PF_RGB);
	desc=*given_desc;
	imagecount=desc.get_frame_start();
	if(desc.get_frame_end()-desc.get_frame_start()>0)
		multi_image=true;
	else
		multi_image=false;
	return true;
}

void
ppm::end_frame()
{
	imagecount++;
}

bool
ppm::start_frame(synfig::ProgressCallback *callback)
{
	int w=desc.get_w(),h=desc.get_h();

	if(filename=="-")
	{
		if(callback)callback->task(strprintf("(stdout) %d",imagecount).c_str());
		file=SmartFILE(stdout);
	}
	else if(multi_image)
	{
		String
			newfilename(filename),
			ext(find(filename.begin(),filename.end(),'.'),filename.end());
		newfilename.erase(find(newfilename.begin(),newfilename.end(),'.'),newfilename.end());

		newfilename+=etl::strprintf("%04d",imagecount)+ext;
		file=SmartFILE(fopen(newfilename.c_str(),"wb"));
		if(callback)callback->task(newfilename);
	}
	else
	{
		file=SmartFILE(fopen(filename.c_str(),"wb"));
		if(callback)callback->task(filename);
	}

	if(!file)
		return false;

	fprintf(file.get(), "P6\n");
	fprintf(file.get(), "%d %d\n", w, h);
	fprintf(file.get(), "%d\n", 255);

	delete [] buffer;
	buffer=new unsigned char[3*w];

	delete [] color_buffer;
	color_buffer=new Color[desc.get_w()];

	return true;
}

Color *
ppm::start_scanline(int scanline)
{
	return color_buffer;
}

bool
ppm::end_scanline()
{
	if(!file)
		return false;

	convert_color_format(buffer, color_buffer, desc.get_w(), PF_RGB, gamma());

	if(!fwrite(buffer,1,desc.get_w()*3,file.get()))
		return false;

	return true;
}
