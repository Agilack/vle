/**
 * @file examples/lifegame/Cell.hpp
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


#ifndef MODEL_LIFEGAME_CELL_HPP
#define MODEL_LIFEGAME_CELL_HPP

#include <vle/devs.hpp>
#include <vle/extension.hpp>

namespace vle { namespace examples { namespace lifegame {

class Cell : public vle::extension::CellDevs
{
public:
    Cell(const vle::devs::DynamicsInit& model,
         const vle::devs::InitEventList& events) :
        vle::extension::CellDevs(model, events)
    { }

    virtual ~Cell() { }

    // DEVS Methods
    virtual vle::devs::Time init(const vle::devs::Time& /* time */);
    virtual void internalTransition(const vle::devs::Time& /* event */);
    virtual void processPerturbation(const vle::devs::ExternalEvent& /* event */) { }

private:
    enum state { INIT, IDLE, NEWSTATE };

    double mTimeStep;
    state mState;

    virtual void updateSigma(vle::devs::Event*) { setSigma(0); };
};

}}} // namespace vle examples lifegame

DECLARE_NAMED_DYNAMICS(cell, vle::examples::lifegame::Cell)

#endif
