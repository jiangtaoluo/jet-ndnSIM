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
                    );
            return tid;
        }

        ProducerPush::ProducerPush()
          :m_dataName("none"), m_emergencyInd("none"),m_seq(0)
        {
            NS_LOG_FUNCTION_NOARGS();
        }

        void
        ProducerPush::SendPacket()
        {
            NS_LOG_INFO("\nEntering ProducerPush::SendPacket");
            
            if (!m_active)
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

            
            // to create real wire encoding
            data->wireEncode();

            m_transmittedDatas(data, this, m_face);
            m_appLink->onReceiveData(*data);
        }


        void ProducerPush::StartApplication()
        {
            NS_LOG_INFO("Entering ProducerPush::StartApplication ...");
            NS_LOG_FUNCTION_NOARGS();
            // do base stuff
            App::StartApplication();

            // Send the first packet
            SendPacket();
        }

    }
}
