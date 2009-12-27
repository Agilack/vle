/**
 * @file examples/recursives/TwoTimesModel.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
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


#include <vle/devs/Dynamics.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/manager/Run.hpp>
#include <vle/utils/Path.hpp>
#include <iostream>

namespace vle { namespace examples { namespace recursives {

    class TwoTimesModel : public devs::Dynamics
    {
    public:
        TwoTimesModel(const devs::DynamicsInit& mdl,
                             const devs::InitEventList& lst)
            : devs::Dynamics(mdl, lst), m_stop(false)
        {}

        virtual ~TwoTimesModel()
        {
            m_root.finish();
        }

        virtual devs::Time init(const devs::Time& /* t */)
        {
            // open a simulation
            vpz::Vpz file(utils::Path::path().getExampleFile("unittest.vpz"));
            m_root.load(file);          // and perform the initialization of
            m_root.init();              // the root coordinator

            return 0.0;
        }

        virtual void internalTransition(const devs::Time& /* t */)
        {
            // If the sub simulation thresh the date 50.0, we stop the sub
            // simulation.
            if (m_root.getCurrentTime() > 50.0) {
                m_stop = true;
            }

            m_root.run();            // run one bag of event in the sub-simulation
        }

        virtual devs::Time timeAdvance() const
        {
            if (not m_stop) {
                return 0;
            } else {
                return devs::Time::infinity;
            }
        }

    private:
        devs::RootCoordinator m_root;
        bool m_stop;
    };

    DECLARE_NAMED_DYNAMICS(twotimesmodel, TwoTimesModel)

}}} // namespace vle examples recursives

