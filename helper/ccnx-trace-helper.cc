/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2011 UCLA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author:  Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#include "ccnx-trace-helper.h"

#include "ns3/config.h"
#include "ns3/callback.h"
#include "ns3/ccnx-app.h"
#include "ns3/ccnx-face.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/node-list.h"
#include "ns3/object-vector.h"
#include "ns3/simulator.h"
#include "ns3/names.h"

#include <boost/ref.hpp>
#include <boost/lexical_cast.hpp>

#include "tracers/ccnx-aggregate-app-tracer.h"
#include "tracers/ccnx-aggregate-l3-tracer.h"
#include "tracers/ccnx-rate-l3-tracer.h"

#include "ns3/ccnx-interest-header.h"
#include "ns3/ccnx-content-object-header.h"

#include <fstream>

using namespace std;
using namespace boost;

NS_LOG_COMPONENT_DEFINE ("CcnxTraceHelper");

namespace ns3 {

CcnxTraceHelper::CcnxTraceHelper ()
  : m_l3RateTrace (0)
{
}

CcnxTraceHelper::~CcnxTraceHelper ()
{
  NS_LOG_FUNCTION (this);
  if (m_l3RateTrace != 0) delete m_l3RateTrace;
  
  if (m_apps.size () > 0)
    {
      ofstream of;
      if (!m_appTrace.empty ())
        {
          of.open (m_appTrace.c_str (), ios_base::trunc | ios_base::out);
          of << "# ";
          m_apps.front ()->PrintHeader (of);
          of << "\n";
        }
      
      for (std::list<Ptr<CcnxAppTracer> >::iterator app = m_apps.begin ();
           app != m_apps.end ();
           app++)
        {
          if (!m_appTrace.empty ())
            {
              (*app)->Print (of);
              of << "\n";
            }
          else
            {
              NS_LOG_INFO (*(*app));
            }
        }
    }

  if (m_l3s.size () > 0)
    {
      ofstream of;
      if (!m_l3Trace.empty ())
        {
          of.open (m_l3Trace.c_str (), ios_base::trunc | ios_base::out);
          of << "# ";
          m_l3s.front ()->PrintHeader (of);
          of << "\n";
        }
      
      for (std::list<Ptr<CcnxL3Tracer> >::iterator l3 = m_l3s.begin ();
           l3 != m_l3s.end ();
           l3++)
        {
          if (!m_l3Trace.empty ())
            {
              (*l3)->Print (of);
              of << "\n";
            }
          else
            {
              NS_LOG_INFO (*(*l3));
            }
        }
    }
}

void
CcnxTraceHelper::SetAppTraceFile (const std::string &appTrace)
{
  NS_LOG_FUNCTION (this << appTrace);
  m_appTrace = appTrace;
}

void
CcnxTraceHelper::SetL3TraceFile (const std::string &l3Trace)
{
  NS_LOG_FUNCTION (this << l3Trace);
  m_l3Trace = l3Trace;
}

void
CcnxTraceHelper::EnableAggregateAppAll (const std::string &appName)
{
  NS_LOG_FUNCTION (this << appName);
  for (NodeList::Iterator node = NodeList::Begin ();
       node != NodeList::End ();
       node++)
    {
      ObjectVectorValue apps;
      (*node)->GetAttribute ("ApplicationList", apps);

      NS_LOG_DEBUG ("Node: " << lexical_cast<string> ((*node)->GetId ()));
      
      uint32_t appId = 0;
      for (ObjectVectorValue::Iterator app = apps.Begin ();
           app != apps.End ();
           app++, appId++)
        {
          NS_LOG_DEBUG ("App: " << lexical_cast<string> (appId) << ", typeId: " << (*app)->GetInstanceTypeId ().GetName ());
          if ((*app)->GetInstanceTypeId ().GetName () == appName)
            {
              m_apps.push_back (Create<CcnxAggregateAppTracer> (appName,
                                                                *node,
                                                                lexical_cast<string> (appId)));
            }
        }
    }
}

void
CcnxTraceHelper::EnableAggregateL3All ()
{
  NS_LOG_FUNCTION (this);
  
  for (NodeList::Iterator node = NodeList::Begin ();
       node != NodeList::End ();
       node++)
    {
      NS_LOG_DEBUG ("Node: " << lexical_cast<string> ((*node)->GetId ()));
      
      m_l3s.push_back (Create<CcnxAggregateL3Tracer> (*node));
    }
}

void
CcnxTraceHelper::EnableRateL3All (const std::string &l3RateTrace)
{
  NS_LOG_FUNCTION (this);
  m_l3RateTrace = new ofstream (l3RateTrace.c_str (), ios::trunc);

  for (NodeList::Iterator node = NodeList::Begin ();
       node != NodeList::End ();
       node++)
    {
      NS_LOG_DEBUG ("Node: " << lexical_cast<string> ((*node)->GetId ()));

      Ptr<CcnxRateL3Tracer> trace = Create<CcnxRateL3Tracer> (boost::ref(*m_l3RateTrace), *node);
      trace->SetAveragingPeriod (Seconds (0.5));
      m_l3Rates.push_back (trace);
    }

  if (m_l3Rates.size () > 0)
    {
      // *m_l3RateTrace << "# "; // not necessary for R's read.table
      m_l3Rates.front ()->PrintHeader (*m_l3RateTrace);
      *m_l3RateTrace << "\n";
    }
}

} // namespace ns3