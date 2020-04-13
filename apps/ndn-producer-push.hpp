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

#include "ns3/random-variable-stream.h"

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

  virtual void
  StopApplication();  // Stop the applicaiton, and cancel the event of Data packet;

  
protected:
  /**
   * \brief Constructs the Data packet and sends it using a callback to the underlying NDN
   * protocol
   */
  virtual void
  ScheduleNextPacket();

  /**
   * @brief Set type of frequency randomization
   * @param value Either 'none', 'uniform', or 'exponential'
   */
  void
  SetRandomize(const std::string& value);

  /**
   * @brief Get type of frequency randomization
   * @returns either 'none', 'uniform', or 'exponential'
   */
  std::string
  GetRandomize() const;

protected:
  double m_frequency; // Frequency of interest packets (in hertz)
  bool m_firstTime;
  Ptr<RandomVariableStream> m_random;
  std::string m_randomType;

private:
  Name m_dataName;  // Name of the generated Data packets
  std::string  m_emergencyInd;  // Indicator of Emergency
  uint32_t m_seq;  // sequence numer current being sent
  uint32_t m_seqMax;   ///< @brief maximum number of sequence number

  EventId m_sendEvent; ///< @brief EventId of pending "send packet" event
};
  
} 
}

#endif
