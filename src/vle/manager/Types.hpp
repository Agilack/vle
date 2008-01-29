/** 
 * @file Types.hpp
 * @brief 
 * @author The vle Development Team
 * @date 2008-01-28
 */

/*
 * Copyright (C) 2008 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_MANAGER_TYPE_HPP
#define VLE_MANAGER_TYPE_HPP

#include <boost/multi_array.hpp>
#include <vle/oov/Plugin.hpp>
#include <vector>
#include <list>

namespace vle { namespace manager  {

    /** 
     * @brief Define a list of filename from command line arguments argv and
     * argc.
     */
    typedef std::vector < const char* > CmdArgs;

    //
    // JustRun output types

    /** 
     * @brief Define a list of oov::PluginViewList.
     */
    typedef std::vector < oov::PluginViewList > OutputSimulationList;

    /** 
     * @brief Define a dictionary of key vpz filename and index. Index is the
     * index in the OutputSimulationList.
     */
    typedef std::map < std::string,
            oov::PluginViewList::size_type > OutputSimulationNames;


    //
    // Distant simulation types
    //

    /** 
     * @brief
     */
    struct OutputSimulationDistant
    {
        OutputSimulationDistant(int instance, int replica,
                                const oov::PluginViewList& outputs) :
            instance(instance),
            replica(replica),
            outputs(outputs)
        {
        }

        int instance;
        int replica;
        oov::PluginViewList outputs;
    };

    /** 
     * @brief 
     */
    typedef std::vector < OutputSimulationDistant > OutputSimulationDistantList;

    //
    // Manager simulation types
    //

    /** 
     * @brief Define a two dimensional array of OutputSimulationList to store
     * result of a ManagerRun.
     */
    typedef boost::multi_array < oov::PluginViewList, 2 > OutputSimulationMatrix;

}} // namespace vle manager

#endif
