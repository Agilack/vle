/**
 * @file vle/oov/LocalStreamReader.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2007 Gauthier Quesnel quesnel@users.sourceforge.net
 * Copyright (C) 2007-2009 INRA http://www.inra.fr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_OOV_LOCALSTREAMREADER_HPP
#define VLE_OOV_LOCALSTREAMREADER_HPP

#include <vle/oov/StreamReader.hpp>
#include <vle/oov/DllDefines.hpp>

namespace vle { namespace oov {

    /**
     * @brief The LocalStreamReader inherits the class StreamReader and provided
     * a direct access to the observations information produced by models and
     * send via the devs::LocalStreamWriter.
     */
    class VLE_OOV_EXPORT LocalStreamReader : public StreamReader
    {
    public:
        /**
         * @brief Build a new LocalStreamReader.
         */
        LocalStreamReader() :
            m_image(0)
        {}

        /**
         * @brief Nothing to delete.
         */
        virtual ~LocalStreamReader()
        {}

        ///
        ////
        ///

        /**
         * @brief The LocalStreamReader can write cairo image if the Plugin is a
         * CairoPlugin a can grab image.
         * @param simulator The name of the simulator.
         * @param parent The parents of the simulator.
         * @param port The observation's port.
         * @param view The observation's view.
         * @param time The date of the observation.
         * @param value The value attached to this observation.
         */
        virtual void onValue(const std::string& simulator,
                             const std::string& parent,
                             const std::string& port,
                             const std::string& view,
                             const double& time,
                             value::Value* value);

    private:
        /**
         * @brief A counter of images.
         */
        unsigned int m_image;
    };

}} // namespace vle oov

#endif
