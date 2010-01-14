/**
 * @file vle/extension/difference-equation/Generic.cpp
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


#include <vle/extension/difference-equation/Generic.hpp>

namespace vle { namespace extension { namespace DifferenceEquation {

using namespace devs;
using namespace value;

Generic::Generic(const DynamicsInit& model,
                 const InitEventList& events) :
    Simple(model, events, false)
{
    mMode = PORT;
}

void Generic::allSync()
{
    mAllSynchro = true;
}

void Generic::beginExt()
{
    mNamesIt = mExternalValues.begin();
}

bool Generic::endExt()
{
    return mNamesIt == mExternalValues.end();
}

void Generic::nextExt()
{
    ++mNamesIt;
}

std::string Generic::nameExt()
{
    return mNamesIt->first;
}

double Generic::valueExt(int shift)
{
    const std::string name = mNamesIt->first;
    VariableIterators iterators;

    iterators.mValues = &mExternalValues.find(name)->second;
    iterators.mInitValues = &mInitExternalValues.find(name)->second;
    iterators.mReceivedValues = &mReceivedValues.find(name)->second;
    iterators.mSynchro = mAllSynchro;
    return Base::val(name, &mExternalValues.find(name)->second,
                     iterators, shift);
}

void Generic::externalTransition(const ExternalEventList& events,
                                 const Time& time)
{
    ExternalEventList::const_iterator it = events.begin();

    while (it != events.end()) {
        if ((*it)->onPort("add")) {
            std::string name = (*it)->getStringAttributeValue("name");

            if (mAllSynchro) {
                mSynchros.insert(name);
                ++mWaiting;
            }
            mDepends.insert(name);
            if (mState != INIT) {
                initExternalVariable(name);
            }
        } else if ((*it)->onPort("remove")) {
            std::string name = (*it)->getStringAttributeValue("name");

            if (mAllSynchro) {
                mSynchros.erase(name);
                --mSyncs;
            }
            mDepends.erase(name);
            mExternalValues.erase(name);
            mInitExternalValues.erase(name);
            if (mReceivedValues.find(name)->second) {
                --mReceive;
            }
            mReceivedValues.erase(name);
        }
        ++it;
    }
    Simple::externalTransition(events, time);
}

}}} // namespace vle extension DifferenceEquation