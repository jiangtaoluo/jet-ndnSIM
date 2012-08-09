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
 *          Ilya Moiseenko <iliamo@cs.ucla.edu> 
 */

#ifndef NDN_STACK_HELPER_H
#define NDN_STACK_HELPER_H

#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/object-factory.h"
#include "ns3/nstime.h"

namespace ns3 {

class Node;
class NdnFaceContainer;
class NdnFace;

/**
 * \ingroup ndn
 * \defgroup ndn-helpers Helpers
 */
/**
 * \ingroup ndn-helpers
 * \brief Adding Ndn functionality to existing Nodes.
 *
 * This helper enables pcap and ascii tracing of events in the ndn stack
 * associated with a node.  This is substantially similar to the tracing that
 * happens in device helpers, but the important difference is that, well, there
 * is no device.  This means that the creation of output file names will change,
 * and also the user-visible methods will not reference devices and therefore
 * the number of trace enable methods is reduced.
 *
 * Normally we eschew multiple inheritance, however, the classes
 * PcapUserHelperForNdn and AsciiTraceUserHelperForNdn are treated as
 * "mixins".  A mixin is a self-contained class that encapsulates a general
 * attribute or a set of functionality that may be of interest to many other
 * classes.
 */
class NdnStackHelper 
{
public:
  /**
   * \brief Create a new NdnStackHelper with a default NDN_FLOODING forwarding stategy
   */
  NdnStackHelper();
  
  /**
   * \brief Destroy the NdnStackHelper
   */
  virtual ~NdnStackHelper ();

  /**
   * @brief Set parameters of NdnL3Protocol
   */
  void
  SetNdnAttributes (const std::string &attr1 = "", const std::string &value1 = "",
                     const std::string &attr2 = "", const std::string &value2 = "",
                     const std::string &attr3 = "", const std::string &value3 = "",
                     const std::string &attr4 = "", const std::string &value4 = "");
  
  
  /**
   * @brief Set forwarding strategy class and its attributes
   * @param forwardingStrategyClass string containing name of the forwarding strategy class
   *
   * Valid options are "ns3::NdnFloodingStrategy" (default) and "ns3::NdnBestRouteStrategy"
   *
   * Other strategies can be implemented, inheriting ns3::NdnForwardingStrategy class
   */
  void
  SetForwardingStrategy (const std::string &forwardingStrategyClass,
                         const std::string &attr1 = "", const std::string &value1 = "",
                         const std::string &attr2 = "", const std::string &value2 = "",
                         const std::string &attr3 = "", const std::string &value3 = "",
                         const std::string &attr4 = "", const std::string &value4 = "");

  /**
   * @brief Set content store class and its attributes
   * @param contentStoreClass string, representing class of the content store
   */
  void
  SetContentStore (const std::string &contentStoreClass,
                   const std::string &attr1 = "", const std::string &value1 = "",
                   const std::string &attr2 = "", const std::string &value2 = "",
                   const std::string &attr3 = "", const std::string &value3 = "",
                   const std::string &attr4 = "", const std::string &value4 = "");

  /**
   * @brief Set PIT class and its attributes
   * @param pitClass string, representing class of PIT
   */
  void
  SetPit (const std::string &pitClass,
          const std::string &attr1 = "", const std::string &value1 = "",
          const std::string &attr2 = "", const std::string &value2 = "",
          const std::string &attr3 = "", const std::string &value3 = "",
          const std::string &attr4 = "", const std::string &value4 = "");
  
  /**
   * @brief Set FIB class and its attributes
   * @param pitClass string, representing class of FIB
   */
  void
  SetFib (const std::string &fibClass,
          const std::string &attr1 = "", const std::string &value1 = "",
          const std::string &attr2 = "", const std::string &value2 = "",
          const std::string &attr3 = "", const std::string &value3 = "",
          const std::string &attr4 = "", const std::string &value4 = "");

  /**
   * @brief Enable Interest limits (disabled by default)
   *
   * @param enable           Enable or disable limits
   * @param avgRtt           Average RTT
   * @param avgContentObject Average size of contentObject packets (including all headers)
   * @param avgInterest      Average size of interest packets (including all headers)
   */
  void
  EnableLimits (bool enable = true, Time avgRtt=Seconds(0.1), uint32_t avgContentObject=1100, uint32_t avgInterest=40);
  
  /**
   * \brief Install Ndn stack on the node
   *
   * This method will assert if called on a node that already has Ndn object
   * installed on it
   * 
   * \param nodeName The name of the node on which to install the stack.
   *
   * \returns list of installed faces in the form of a smart pointer
   * to NdnFaceContainer object
   */
  Ptr<NdnFaceContainer>
  Install (std::string nodeName) const;

  /**
   * \brief Install Ndn stack on the node
   *
   * This method will assert if called on a node that already has Ndn object
   * installed on it
   * 
   * \param node The node on which to install the stack.
   *
   * \returns list of installed faces in the form of a smart pointer
   * to NdnFaceContainer object
   */
  Ptr<NdnFaceContainer>
  Install (Ptr<Node> node) const;

  /**
   * \brief Install Ndn stack on each node in the input container
   *
   * The program will assert if this method is called on a container with a node
   * that already has an ndn object aggregated to it.
   * 
   * \param c NodeContainer that holds the set of nodes on which to install the
   * new stacks.
   *
   * \returns list of installed faces in the form of a smart pointer
   * to NdnFaceContainer object
   */
  Ptr<NdnFaceContainer>
  Install (NodeContainer c) const;

  /**
   * \brief Install Ndn stack on all nodes in the simulation
   *
   * \returns list of installed faces in the form of a smart pointer
   * to NdnFaceContainer object
   */
  Ptr<NdnFaceContainer>
  InstallAll () const;

  /**
   * \brief Add forwarding entry in FIB
   *
   * \param nodeName Node name
   * \param prefix Routing prefix
   * \param faceId Face index
   * \param metric Routing metric
   */
  static void
  AddRoute (std::string nodeName, std::string prefix, uint32_t faceId, int32_t metric);

  /**
   * \brief Add forwarding entry in FIB
   *
   * \param nodeName Node
   * \param prefix Routing prefix
   * \param faceId Face index
   * \param metric Routing metric
   */
  static void
  AddRoute (Ptr<Node> node, std::string prefix, uint32_t faceId, int32_t metric);

  /**
   * \brief Add forwarding entry in FIB
   *
   * \param node   Node
   * \param prefix Routing prefix
   * \param face   Face
   * \param metric Routing metric
   */
  static void
  AddRoute (Ptr<Node> node, std::string prefix, Ptr<NdnFace> face, int32_t metric);

  /**
   * \brief Set flag indicating necessity to install default routes in FIB
   */
  void
  SetDefaultRoutes (bool needSet);

private:
  NdnStackHelper (const NdnStackHelper &);
  NdnStackHelper &operator = (const NdnStackHelper &o);
  
private:
  ObjectFactory m_ndnFactory;
  ObjectFactory m_strategyFactory;
  ObjectFactory m_contentStoreFactory;
  ObjectFactory m_pitFactory;
  ObjectFactory m_fibFactory;
  
  bool m_limitsEnabled;
  Time     m_avgRtt;
  uint32_t m_avgContentObjectSize;
  uint32_t m_avgInterestSize;
  bool m_needSetDefaultRoutes;  
};

} // namespace ns3

#endif /* NDN_STACK_HELPER_H */