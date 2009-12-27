/**
 * @file vle/gvle/modeling/DifferenceEquation/Multiple.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
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


#ifndef VLE_GVLE_MODELING_DIFFERENCEEQUATION_MULTIPLE_HPP
#define VLE_GVLE_MODELING_DIFFERENCEEQUATION_MULTIPLE_HPP

#include <vle/gvle/ModelingPlugin.hpp>
#include <vle/gvle/modeling/DifferenceEquation/Plugin.hpp>
#include <vle/gvle/conditions/DifferenceEquation/Variables.hpp>
#include <vle/utils/Template.hpp>
#include <gtkmm/dialog.h>

namespace vle { namespace gvle { namespace modeling {

class Multiple : public modeling::Plugin,
                 public conditions::Variables
{
public:
    Multiple(const std::string& name);
    virtual ~Multiple();

    virtual bool create(graph::AtomicModel& atom,
                        vpz::AtomicModel& model,
                        vpz::Dynamic& dynamic,
                        vpz::Conditions& conditions,
                        vpz::Observables& observables,
                        const std::string& classname,
                        const std::string& namespace_);

    virtual bool modify(graph::AtomicModel& atom,
                        vpz::AtomicModel& model,
                        vpz::Dynamic& dynamic,
                        vpz::Conditions& conditions,
                        vpz::Observables& observables,
                        const std::string& conf,
                        const std::string& buffer);

private:
    Gtk::Dialog*         m_dialog;
    Gtk::Button*         m_buttonSource;

    std::list < sigc::connection > mList;

    void build();
    void fillFields(const vpz::Condition& condition);
    virtual void generateCondition(graph::AtomicModel& atom,
                                   vpz::AtomicModel& model,
                                   vpz::Conditions& conditions);
    virtual void generateOutputPorts(graph::AtomicModel& atom);
    virtual void generateVariables(utils::Template& tpl_);
    std::string getTemplate() const;
};

}}} // namespace vle gvle modeling

#endif
