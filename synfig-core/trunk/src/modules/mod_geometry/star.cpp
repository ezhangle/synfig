/*! ========================================================================
** Synfig
** Template File
** $Id: star.cpp,v 1.1.1.1 2005/01/04 01:23:10 darco Exp $
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

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "star.h"
#include <ETL/stringf>
#include <ETL/bezier>
#include <ETL/hermite>

#include <synfig/string.h>
#include <synfig/time.h>
#include <synfig/context.h>
#include <synfig/paramdesc.h>
#include <synfig/renddesc.h>
#include <synfig/surface.h>
#include <synfig/value.h>
#include <synfig/valuenode.h>
#include <synfig/segment.h>

#endif

using namespace etl;

/* === M A C R O S ========================================================= */

#define SAMPLES		75

/* === G L O B A L S ======================================================= */

SYNFIG_LAYER_INIT(Star);
SYNFIG_LAYER_SET_NAME(Star,"star");
SYNFIG_LAYER_SET_LOCAL_NAME(Star,_("Star"));
SYNFIG_LAYER_SET_CATEGORY(Star,_("Geometry"));
SYNFIG_LAYER_SET_VERSION(Star,"0.1");
SYNFIG_LAYER_SET_CVS_ID(Star,"$Id: star.cpp,v 1.1.1.1 2005/01/04 01:23:10 darco Exp $");

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

/* === E N T R Y P O I N T ================================================= */

Star::Star():
	radius1(1.0),
	radius2(0.38),
	points(5),
	angle(Angle::deg(90))
{
	sync();
}

void
Star::sync()
{
	Angle dist_between_points(Angle::rot(1)/float(points));
	std::vector<Point> vector_list;

	int i;
	for(i=0;i<points;i++)
	{
		Angle dist1(dist_between_points*i+angle);
		Angle dist2(dist_between_points*i+dist_between_points/2+angle);
		vector_list.push_back(Point(Angle::cos(dist1).get()*radius1,Angle::sin(dist1).get()*radius1));
		vector_list.push_back(Point(Angle::cos(dist2).get()*radius2,Angle::sin(dist2).get()*radius2));
	}
	clear();
	add_polygon(vector_list);
}

bool
Star::set_param(const String & param, const ValueBase &value)
{
	if(	param=="radius1" && value.same_as(radius1))
	{
		value.put(&radius1);
		sync();
		return true;
	}

	if(	param=="radius2" && value.same_as(radius2))
	{
		value.put(&radius2);
		sync();
		return true;
	}

	if(	param=="points" && value.same_as(points))
	{
		value.put(&points);
		if(points<2)points=2;
		sync();
		return true;
	}

	if(	param=="angle" && value.same_as(angle))
	{
		value.put(&angle);
		sync();
		return true;
	}

	if(param=="vector_list")
		return false;

	return Layer_Polygon::set_param(param,value);
}

ValueBase
Star::get_param(const String& param)const
{
	EXPORT(radius1);
	EXPORT(radius2);
	EXPORT(points);
	EXPORT(angle);

	EXPORT_NAME();
	EXPORT_VERSION();

	if(param=="vector_list")
		return ValueBase();

	return Layer_Polygon::get_param(param);
}

Layer::Vocab
Star::get_param_vocab()const
{
	Layer::Vocab ret(Layer_Polygon::get_param_vocab());

	// Pop off the polygon parameter from the polygon vocab
	ret.pop_back();

	ret.push_back(ParamDesc("radius1")
		.set_local_name(_("Outer Radius"))
		.set_description(_("The radius of the outer points in the star"))
		.set_is_distance()
		.set_origin("offset")
	);

	ret.push_back(ParamDesc("radius2")
		.set_local_name(_("Inner Radius"))
		.set_description(_("The radius of the inner points in the star"))
		.set_is_distance()
		.set_origin("offset")
	);

	ret.push_back(ParamDesc("angle")
		.set_local_name(_("Angle"))
		.set_description(_("The orientation of the star"))
		.set_origin("offset")
	);

	ret.push_back(ParamDesc("points")
		.set_local_name(_("Points"))
		.set_description(_("The number of points in the star"))
	);

	return ret;
}
