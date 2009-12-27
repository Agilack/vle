/**
 * @file examples/gens/GenExecutiveClass.hpp
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


#ifndef EXAMPLES_GENS_GENEXECUTIVECLASS_HPP
#define EXAMPLES_GENS_GENEXECUTIVECLASS_HPP

#include <vle/devs.hpp>
#include <stack>

namespace vle { namespace examples { namespace gens {

    class GenExecutiveClass : public devs::Executive
    {
    public:
        GenExecutiveClass(const devs::ExecutiveInit& mdl,
                          const devs::InitEventList& events);

        virtual ~GenExecutiveClass() { }

        virtual devs::Time init(const devs::Time& /* time */);

        virtual devs::Time timeAdvance() const;

        virtual void internalTransition(
            const devs::Time& ev);

        virtual value::Value* observation(
            const devs::ObservationEvent& ev) const;

    private:
        enum state { INIT, IDLE, ADDMODEL, DELMODEL };

        std::stack < std::string >  m_stacknames;
        state                       m_state;
        bool                        m_buildbeepbeep;

        void add_new_model();
        void del_first_model();
        int get_nb_model() const;
    };

}}} // namespace vle examples gens

DECLARE_NAMED_EXECUTIVE(executiveclass, vle::examples::gens::GenExecutiveClass)

#endif
