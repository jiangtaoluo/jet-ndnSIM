/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2018-2020  CQUPT.
 *
 * @author Jiangtao Luo<Luojt@cqupt.edu.cn>.
 **/
// ndn-producer-push.cpp

#include "ndn-producer-push.hpp"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "model/ndn-l3-protocol.hpp"
#include "helper/ndn-fib-helper.hpp"
#include <memory>

#include "ns3/double.h"
#include "ns3/integer.h"

NS_LOG_COMPONENT_DEFINE("ndn.ProducerPush");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED(ProducerPush);

TypeId
ProducerPush::GetTypeId(void)
{
  static TypeId tid =
    TypeId("ns3::ndn::ProducerPush")
    .SetGroupName("Ndn")
    .SetParent<Producer>()
    .AddConstructor<ProducerPush>()

    .AddAttribute("DataName",
                  "Name of Emergency Data: /(default)", StringValue("/"),
                  MakeNameAccessor(&ProducerPush::m_dataName),
                  MakeNameChecker()
                  )
    .AddAttribute("EmergencyInd",
                  "Emergency indicator: none(default), emergency, other",
                  StringValue("none"),
                  MakeStringAccessor(&ProducerPush::m_emergencyInd),
                  MakeStringChecker()
                  )
    .AddAttribute("Frequency", "Frequency of interest packets", StringValue("1.0"),
                  MakeDoubleAccessor(&ProducerPush::m_frequency), MakeDoubleChecker<double>())

    .AddAttribute("Randomize",
                  "Type of send time randomization: none (default), uniform, exponential",
                  StringValue("none"),
                  MakeStringAccessor(&ProducerPush::SetRandomize, &ProducerPush::GetRandomize),
                  MakeStringChecker())

    .AddAttribute("MaxSeq", "Maximum sequence number to push",
                  IntegerValue(std::numeric_limits<uint32_t>::max()),
                  MakeIntegerAccessor(&ProducerPush::m_seqMax), MakeIntegerChecker<uint32_t>())

    ;
  return tid;
}

ProducerPush::ProducerPush()
  : m_dataName("none")
  , m_emergencyInd("none")
  , m_seq(0)
  , m_frequency(1.0)
  , m_firstTime(true)
{
  NS_LOG_FUNCTION_NOARGS();

  m_seqMax = std::numeric_limits<uint32_t>::max();           
}

void
ProducerPush::SendPacket()
{
  NS_LOG_INFO("\nEntering ProducerPush::SendPacket");
            
  if (!m_active)
    return;

  if (m_seq >= m_seqMax)
    return;

  NS_LOG_FUNCTION_NOARGS();

  Name dataName(m_dataName);

  // Add seq
  dataName.appendSequenceNumber(m_seq++);

  auto data = make_shared<Data>();
  data->setName(dataName);
  data->setFreshnessPeriod(::ndn::time::milliseconds(m_freshness.GetMilliSeconds()));

  data->setContent(make_shared<::ndn::Buffer>(m_virtualPayloadSize));

  Signature signature;
  SignatureInfo signatureInfo(static_cast<::ndn::tlv::SignatureTypeValue>(255));

  if(m_keyLocator.size() > 0) {
    signatureInfo.setKeyLocator(m_keyLocator);
  }

  signature.setInfo(signatureInfo);
  signature.setValue(::ndn::makeNonNegativeIntegerBlock(::ndn::tlv::SignatureValue, m_signature));

  data->setSignature(signature);

  // Set 'EmgergencyInd'
  // Jiangtao Luo. 12 Feb 2020
  data->setEmergencyInd(m_emergencyInd);

  NS_LOG_INFO("node(" << GetNode()->GetId() << ") Sending Data: " << data->getName());

  // Set Nonce
  data->getNonce();

  // Set born time. Jiangtao Luo. 13 Apr 2020
   Time now = Simulator::Now();
  
  uint64_t ts = now.GetNanoSeconds();
  NS_LOG_INFO("Set Data born time =  " << ts);
  data->setBornTime(ts);
            
  // to create real wire encoding
  data->wireEncode();

  m_transmittedDatas(data, this, m_face);
  m_appLink->onReceiveData(*data);

  ScheduleNextPacket();
}

void
ProducerPush::StartApplication()
{
  NS_LOG_INFO("Entering ProducerPush::StartApplication ...");
  NS_LOG_FUNCTION_NOARGS();
  // do base stuff
  App::StartApplication();

  // Send the first packet
  SendPacket();
}

void
ProducerPush::StopApplication()
{
   NS_LOG_FUNCTION_NOARGS();

  // cancel periodic packet generation
  Simulator::Cancel(m_sendEvent);

  // cleanup base stuff
  App::StopApplication();
}

void
ProducerPush::ScheduleNextPacket()
{

  if (m_firstTime) {
    m_sendEvent = Simulator::Schedule(Seconds(0.0), &ProducerPush::SendPacket, this);
    m_firstTime = false;
  }
  else if (!m_sendEvent.IsRunning())
    m_sendEvent = Simulator::Schedule((m_random == 0) ? Seconds(1.0 / m_frequency)
                                                      : Seconds(m_random->GetValue()),
                                      &ProducerPush::SendPacket, this);
}

void
ProducerPush::SetRandomize(const std::string& value)
{
  if (value == "uniform") {
    m_random = CreateObject<UniformRandomVariable>();
    m_random->SetAttribute("Min", DoubleValue(0.0));
    m_random->SetAttribute("Max", DoubleValue(2 * 1.0 / m_frequency));
  }
  else if (value == "exponential") {
    m_random = CreateObject<ExponentialRandomVariable>();
    m_random->SetAttribute("Mean", DoubleValue(1.0 / m_frequency));
    m_random->SetAttribute("Bound", DoubleValue(50 * 1.0 / m_frequency));
  }
  else
    m_random = 0;

  m_randomType = value;
}

std::string
ProducerPush::GetRandomize() const
{
  return m_randomType;
}
  
}  // namespace ndn
}  // namespace ns3
