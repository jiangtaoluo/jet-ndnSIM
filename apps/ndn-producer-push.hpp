/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2018-2020  CQUPT.
 *
 * @author Jiangtao Luo<Luojt@cqupt.edu.cn>.
 **/
// ndn-producer-push.hpp

#ifndef NDN_PRODUCER_PUSH_H
#define NDN_PRODUCER_PUSH_H

#include "ndn-producer.hpp"

namespace ns3 {
namespace ndn {

/**
 * @ingroup ndn-apps
 * @brief Ndn application for proactively sending out a Data packet, especially for emergency or notification message transfer.
 */
class ProducerPush : public Producer {
public:
  static TypeId GetTypeId(void);
  ProducerPush();

  /**
   * @brief Actually send packets
   */
  // Send a single Data packet each time.
  void
  SendPacket();

protected:
  // inherited from Application base class.
  virtual void
  StartApplication();  // Start the applicaiton, and send the first Data packet;

private:
  Name m_dataName;  // Name of the generated Data packets
  std::string  m_emergencyInd;  // Indicator of Emergency
  uint32_t m_seq;  // sequence numer current being sent
};
} 
}

#endif
