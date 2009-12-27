/**
 * @file vle/gvle/About.hpp
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


#ifndef VLE_GVLE_ABOUT_HPP
#define VLE_GVLE_ABOUT_HPP

#include <gtkmm/aboutdialog.h>
#include <libglademm.h>

namespace vle { namespace gvle {

    /**
     * @brief modal About dialog box.
     *
     */
    class About : public sigc::trackable
    {
    public:
        /**
         * Buid a modal About dialog box.
         */
        About(Glib::RefPtr < Gnome::Glade::Xml > refXml);

        /**
         * @brief Nothing to delete.
         */
        virtual ~About() {}

        /**
         * @brief Run the Gtk::AboutDialog.
         */
        void run();

    private:
        Glib::RefPtr < Gnome::Glade::Xml > mRefXml;
        Gtk::AboutDialog* mAbout;

        void onAboutClose(int response);
    };

}} // namespace vle gvle

#endif
