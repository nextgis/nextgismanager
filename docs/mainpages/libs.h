/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Main page of the Doxygen manual
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013 Bishop
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

/**

@page page_libs Library List

@tableofcontents

wxGIS consists the set of libraries and executable which uses them. The libraries 
briefly described below. This diagram shows the dependencies between them:

@dot
digraph Dependencies
{
    node [shape = box, fontname = "DejaVu Sans", fontsize = 10, style = filled];

    wxGISCore           [fillcolor = deepskyblue, URL = "\ref page_libs_wxgiscore"];

    wxGISDatasource     [fillcolor = deepskyblue, URL = "\ref page_libs_wxgisdatasource"];
    wxGISCatalog        [fillcolor = deepskyblue, URL = "\ref page_libs_wxgiscatalog"];
    wxGISNet            [fillcolor = deepskyblue, URL = "\ref page_libs_wxgisnet"];
    wxGISGeoprocessing  [fillcolor = deepskyblue, URL = "\ref page_libs_wxgisgp"];

    wxGISFramework      [fillcolor = green, URL = "\ref page_libs_wxgisfrw"];
    wxGISCatalogUI      [fillcolor = green, URL = "\ref page_libs_wxgiscatalogui"];
    wxGISDisplay        [fillcolor = green, URL = "\ref page_libs_wxgisdisplay"];
    wxGISCarto          [fillcolor = green, URL = "\ref page_libs_wxgiscarto"];
    wxGISCartoUI        [fillcolor = green, URL = "\ref page_libs_wxgiscartoui"];
    wxGISGeoprocessingUI [fillcolor = green, URL = "\ref page_libs_wxgisgpui"];

    wxGISDatasource -> wxGISCore;
    wxGISNet -> wxGISCore;
    wxGISFramework -> wxGISCore;
    
    wxGISCatalog -> wxGISDatasource;
    wxGISDisplay -> wxGISDatasource;
    
    wxGISGeoprocessing -> wxGISCatalog; 
    wxGISGeoprocessing -> wxGISNet;
    
    wxGISCarto ->  wxGISDatasource;
    wxGISCarto ->  wxGISFramework;
    wxGISCarto ->  wxGISDisplay;

    wxGISCartoUI -> wxGISCarto;
    
    wxGISCatalogUI -> wxGISCatalog;
    wxGISCatalogUI -> wxGISCartoUI;
    wxGISCatalogUI -> wxGISGeoprocessing;
    
    wxGISGeoprocessingUI -> wxGISGeoprocessing;
    wxGISGeoprocessingUI -> wxGISCatalogUI;
}
@enddot

Please note that arrows indicate the "depends from" relation and that all blue
libraries depend are non-GUI libraries, and all green libraries are GUI libraries.


@section page_libs_wxgiscore wxGISCore

This are core classes needed in other libraries:

@li Abstract classes (IApplication, IProgressor, ITrackCancel)
@li Various base application classes (wxGISAppWithLibs, wxGISInitializer)
@li Configure classes (wxGISConfig, wxGISAppConfig)
@li Pointers  (wxGISPointer, wxGISConnectionPointContainer)
@li Crypt functions (crypt, decrypt, ...)
@li wxJSON classes
@li wxGISProcess and relative classes (events, enums, etc)
@li ...others

@section page_libs_wxgisdatasource wxGISDatasource

The library contains the classes to read and write spatial data in various formats (raster and vector spatial data in file system, databases or via internet).
Also several functions to operate with data is present.

Requires @ref page_libs_wxgiscore.


@section page_libs_wxgisnet wxGISNet

The library needed to communicate with different wxGIS components. Using the library classes applications can transfer data or messages throw the network 
or inside the same PC. The background processes (e.g. geoprocessing) communicate 
with foreground (e.g. wxGISCatalog application) using library or wxGISProcess from
the wxGISCore library. There are client side and server side classes.
The main classes are:
@li wxNetMessage
@li wxGISNetEvent
@li Abstract classes (INetConnection, INetConnFactory, INetService, INetPlugin)

Requires @ref page_libs_wxgiscore.

@section page_libs_wxgisframework wxGISFramework

Basic GUI classes such as controls, dialogues, buttons, menus, toolbars, accelerators, 
progressors and progress animations are in this library. All
wxGIS GUI components (usually library name ended with UI) must link against this library, only console mode libraries don't. There are several classes to construct the 
main application interface:
@li wxGISApplicationBase
@li wxGISApplication
@li wxGISApplicationEx

Requires @ref page_libs_wxgiscore.


@section page_libs_wxgisdisplay wxGISDisplay

This library contains display class (wxGISDisplay) for drawing map content on the 
@c cairo @c surfaces. Also the needed coordinate conversions from spatial to display coordinates and vice versa present. The display can be rotated, all data drawing 
are properly.

Requires @ref page_libs_wxgiscore and @ref page_libs_wxgisdatasource.

@section page_libs_wxgiscatalog wxGISCatalog

The library consists classes to present the data tree. This tree include 
several root elements (folder connections, database connections, web connections, 
spatial reference systems, etc.). Each datasource has it representation in tree. The 
base class for all elements is @b wxGxObject. For representing the folders or complex 
data there is @b wxGxObjectContainer class. All the element have no GUI as this is 
virtual for data navigation inside programs.

Requires @ref page_libs_wxgiscore and @ref page_libs_wxgisdatasource.

@section page_libs_wxgiscatalogui wxGISCatalogUI

The library include classes to representation tree elements in GUI 
(e.g. TreeView or ContentsView). This classes provides icons, user friendly names 
and more.

Requires @ref page_libs_wxgiscore, @ref page_libs_wxgisdatasource, @ref page_libs_wxgisdisplay, 
@ref page_libs_wxgisframework and @ref page_libs_wxgiscarto.

@section page_libs_wxgiscarto wxGISCarto

The library provides functions to create maps and control their contents 
and styles. The main classes are:
@li wxGISMap
@li wxGISLayer
@li wxGISRenderer
@li wxMxMapViewEvent

Requires @ref page_libs_wxgiscore, @ref page_libs_wxgisdatasource, @ref page_libs_wxgisdisplay and @ref page_libs_wxgisframework.


@section page_libs_wxgiscartoui wxGISCartoUI

The library consist functions to interacted with maps, such as controls (zoom, 
pan, extent, identify etc.), dialogues and grid views.

Requires @ref page_libs_wxgiscore, @ref page_libs_wxgisdatasource, @ref page_libs_wxgisdisplay, @ref page_libs_wxgisframework and @ref page_libs_wxgiscarto.

@section page_libs_wxgisgp wxGISGeoprocessing

The geoprocess is one or several operation with spatial data (e.g. buffer, 
reprojection, etc.). The library provides unified interface for such operations. 
The geoprocessing operations executed with special application, which controlled 
by wxGIS UI components (e.g. wxGISCatalog application). If geoprocessing crashed 
this is not influence on host application. User will be notified about problem with 
geoprocessing operation.

Requires @ref page_libs_wxgiscore, @ref page_libs_wxgisdatasource, @ref page_libs_wxgiscatalog and @ref page_libs_wxgisnet.

@section page_libs_wxgisgpui wxGISGeoprocessingUI

The library provide classes to manage geoprocessing operations (create, change, 
delete, start, stop and etc.). There is an unified dialogue for creating all 
geoprocessing operations. Also, the special tree of geoprocessing operations 
exist (toolbox).

Requires @ref page_libs_wxgiscore, @ref page_libs_wxgisdatasource, @ref page_libs_wxgiscatalog, @ref page_libs_wxgiscatalogui, @ref page_libs_wxgisnet, @ref page_libs_wxgiscarto, @ref page_libs_wxgiscartoui, @ref page_libs_wxgisdisplay, @ref page_libs_wxgisframework and @ref page_libs_wxgisgp.

*/
