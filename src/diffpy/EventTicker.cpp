/*****************************************************************************
*
* diffpy.srreal     Complex Modeling Initiative
*                   Pavol Juhas
*                   (c) 2013 Brookhaven National Laboratory,
*                   Upton, New York.  All rights reserved.
*
* File coded by:    Pavol Juhas
*
* See AUTHORS.txt for a list of people who contributed.
* See LICENSE.txt for license information.
*
******************************************************************************
*
* class EventTicker for managing modification times of dependent objects.
*
* This is used to determine if cached values need update, due to a change
* of their dependencies.  The EventTicker class is inspired by the
* ObjCryst::RefinableObjClock class.
*
*****************************************************************************/

#include <diffpy/EventTicker.hpp>

namespace diffpy {
namespace eventticker {

//////////////////////////////////////////////////////////////////////////////
// class EventTicker
//////////////////////////////////////////////////////////////////////////////

// Constructor ---------------------------------------------------------------

EventTicker::EventTicker() : mtick(0, 0)
{ }

// Public Methods ------------------------------------------------------------

void EventTicker::click()
{
    ++gtick.second;
    if (0 >= gtick.second)
    {
        ++gtick.first;
        gtick.second = 0;
    }
    mtick = gtick;
}


void EventTicker::updateFrom(const EventTicker& other)
{
    if (other > *this)  mtick = other.mtick;
}


bool EventTicker::operator<(const EventTicker& other) const
{
    return mtick < other.mtick;
}


bool EventTicker::operator<=(const EventTicker& other) const
{
    return mtick <= other.mtick;
}


bool EventTicker::operator>(const EventTicker& other) const
{
    return mtick > other.mtick;
}


bool EventTicker::operator>=(const EventTicker& other) const
{
    return mtick >= other.mtick;
}

// Static Global Data --------------------------------------------------------

std::pair<unsigned long, unsigned long> EventTicker::gtick(0, 0);

}   // namespace eventticker
}   // namespace diffpy