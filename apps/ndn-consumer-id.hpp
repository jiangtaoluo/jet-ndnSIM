/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2018-2020  CQUPT.
 *
 * @author Jiangtao Luo<Luojt@cqupt.edu.cn>.
 **/

#ifndef NDN_CONSUMER_ID_H
#define NDN_CONSUMER_ID_H

#include "ndn-consumer-cbr.hpp"

namespace ns3 {
namespace ndn {

/**
 * @ingroup ndn-apps
 * @brief Ndn application for sending out Interest packets at a "constant" rate (Poisson process) with identity of the consumer.
 */
class ConsumerCbrId : public ConsumerCbr {
public:
  static TypeId
  GetTypeId();

  /**
   * \brief Default constructor
   * Sets up randomizer function and packet sequence number
   */
  ConsumerCbrId();
  virtual ~ConsumerCbrId();

protected:
  /**
   * \brief Constructs the Interest packet and sends it using a callback to the underlying NDN
   * protocol
   */
  virtual void
  ScheduleNextPacket();

  /**
   * @brief Actually send packets, from ndn::Consumer
   */
  void
  SendPacket();

protected:
  // Identity of the consumer
  std::string m_identity;
};

} // namespace ndn
} // namespace ns3

#endif
