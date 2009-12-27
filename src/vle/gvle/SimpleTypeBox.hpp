/**
 * @file vle/gvle/SimpleTypeBox.hpp
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
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_GVLE_SIMPLETYPEBOX_HPP
#define VLE_GVLE_SIMPLETYPEBOX_HPP

#include <gtkmm.h>
#include <vle/gvle/DllDefines.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Integer.hpp>
#include <vle/value/Double.hpp>
#include <vle/value/String.hpp>
#include <vle/value/Tuple.hpp>

namespace vle { namespace gvle {

    class VLE_GVLE_EXPORT SimpleTypeBox : public Gtk::Dialog
    {
    public:
        SimpleTypeBox(value::Value* base);
        SimpleTypeBox(const std::string& title,
		      const std::string& value);

        virtual ~SimpleTypeBox();

        std::string run();

        /**
         * @brief Return true if the user have selected the Gtk::Button valid.
         * False otherwise.
         *
         * @return true if Gtk::Dialog is true, false otherwise.
         */
        bool valid() const { return mValid; }

    private:
        value::Value* mBase;
        Gtk::Entry* mEntry;
        bool mValid;

        void makeDialog();
    };

}} // namespace vle gvle

#endif
