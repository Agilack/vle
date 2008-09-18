/**
 * @file vle/gvle/BooleanBox.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
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


#include <iostream>
#include <vle/gvle/BooleanBox.hpp>

using namespace vle;

namespace vle
{
namespace gvle {
BooleanBox::BooleanBox(value::BooleanFactory* boolean):
        Gtk::Dialog("Boolean",true,true),
        mValue(boolean)
{
    add_button(Gtk::Stock::APPLY, Gtk::RESPONSE_APPLY);
    add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);

    m_Combo = new Gtk::ComboBoxText();
    get_vbox()->pack_start(*m_Combo);

    m_Combo->append_text("true");
    m_Combo->append_text("false");
    m_Combo->set_active_text(boolean->toString());
    show_all();
}

BooleanBox::~BooleanBox()
{
    hide_all();
    delete m_Combo;
}

void BooleanBox::run()
{
    int ret = Gtk::Dialog::run();
    if (ret == Gtk::RESPONSE_APPLY) {
        std::string new_val = m_Combo->get_active_text();

        if (new_val == "true")
            mValue->set(true);
        else
            mValue->set(false);
    }

}

}
} // namespace vle gvle