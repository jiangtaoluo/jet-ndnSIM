/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2018-2020  CQUPT.
 *
 * @author Jiangtao Luo<Luojt@cqupt.edu.cn>.
 **/


#include "ndn-consumer-id.hpp"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/double.h"

NS_LOG_COMPONENT_DEFINE("ndn.ConsumerCbrId");

namespace ns3 {
namespace ndn {
  
NS_OBJECT_ENSURE_REGISTERED(ConsumerCbrId);

TypeId
ConsumerCbrId::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::ConsumerCbrId")
      .SetGroupName("Ndn")
      .SetParent<ConsumerCbr>()
      .AddConstructor<ConsumerCbrId>()

      .AddAttribute("Identity", "Identity of the consumer: none (default)",
                    StringValue("none"),
                    MakeStringAccessor(&ConsumerCbrId::m_identity),
                    MakeStringChecker())
    ;

  return tid;
}

ConsumerCbrId::ConsumerCbrId()
  : m_identity("none")
{
  NS_LOG_FUNCTION_NOARGS();
}

ConsumerCbrId::~ConsumerCbrId()
{
}

void
ConsumerCbrId::ScheduleNextPacket()
{
  if (m_firstTime) {
    m_sendEvent = Simulator::Schedule(Seconds(0.0), &Consumer::SendPacket, this);
    m_firstTime = false;
  }
  else if (!m_sendEvent.IsRunning())
    m_sendEvent = Simulator::Schedule((m_random == 0) ? Seconds(1.0 / m_frequency)
                                                      : Seconds(m_random->GetValue()),
                                      &ConsumerCbrId::SendPacket, this);
}

void
ConsumerCbrId::SendPacket()
{
	// For Test. Jiangtao
  NS_LOG_INFO("Entering ConsumerId::SendPacket()... ... ");
  
  if (!m_active)
    return;

  NS_LOG_FUNCTION_NOARGS();

  uint32_t seq = std::numeric_limits<uint32_t>::max(); // invalid

  while (m_retxSeqs.size()) {
    seq = *m_retxSeqs.begin();
    m_retxSeqs.erase(m_retxSeqs.begin());
    break;
  }

  if (seq == std::numeric_limits<uint32_t>::max()) {
    if (m_seqMax != std::numeric_limits<uint32_t>::max()) {
      if (m_seq >= m_seqMax) {
        return; // we are totally done
      }
    }

    seq = m_seq++;
  }

  //
  shared_ptr<Name> nameWithSequence = make_shared<Name>(m_interestName);
  nameWithSequence->appendSequenceNumber(seq);
  //

  // shared_ptr<Interest> interest = make_shared<Interest> ();
  shared_ptr<Interest> interest = make_shared<Interest>();
  interest->setNonce(m_rand->GetValue(0, std::numeric_limits<uint32_t>::max()));
  interest->setName(*nameWithSequence);
  time::milliseconds interestLifeTime(m_interestLifeTime.GetMilliSeconds());
  interest->setInterestLifetime(interestLifeTime);
  
  // Setting the identity.
  interest->setIdentity(m_identity);
   
   // Logging the seq and identity. Jiangtao@20181226
  NS_LOG_INFO("> Interest for " << seq << " from " << m_identity);

  WillSendOutInterest(seq);

  m_transmittedInterests(interest, this, m_face);
  m_appLink->onReceiveInterest(*interest);

  ScheduleNextPacket();
}


} // namespace ndn
} // namespace ns3
