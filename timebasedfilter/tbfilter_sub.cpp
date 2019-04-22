#include <iostream>
#include <thread>
#include <chrono>
#include <gen/tbFilter_DCPS.hpp>

int main (int argc, char *argv[]) {
  dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

  dds::topic::qos::TopicQos topicQos = dp.default_topic_qos()
                        << dds::core::policy::Durability::Transient()
                        << dds::core::policy::Reliability::Reliable();

  dds::topic::Topic<TbasedFilter::Msg> topic(dp, "TbasedFilter_Msg", topicQos);

  dds::sub::qos::SubscriberQos subQos = dp.default_subscriber_qos();
  dds::sub::Subscriber sub(dp, subQos);

  // Create a filter statement

  dds::sub::qos::DataReaderQos drQos = topic.qos();
  
  dds::core::Duration dura;
  // Setup timebased filter
  drQos << dds::core::policy::TimeBasedFilter(dura.from_secs(5));
  
  dds::core::policy::TimeBasedFilter filter;
  // Setup minimum_separation
//  drQos << filter.minimum_separation(dura.from_secs(5));
  // Create the data writer
  dds::sub::DataReader<TbasedFilter::Msg> dr(sub, topic, drQos);

  int count,recv_cnt = 0;
  bool sampleReceived = false;
  do {
    dds::sub::LoanedSamples<TbasedFilter::Msg> samples = dr.take();
    for (dds::sub::LoanedSamples<TbasedFilter::Msg>::const_iterator sample = samples.begin();
        sample < samples.end(); sample++) {
      if (sample->info().valid()) {
        std::cout << " TimeStamp: " << sample->info().timestamp().to_millisecs()
		  << ", sensorID: " << sample->data().sensorID()
        	  << ", value: " << sample->data().value() << std::endl;
    	recv_cnt++;
      }
    }
    count++;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  } while (!sampleReceived && count < 60);
  std::cout << "Recieved total: " << recv_cnt << std::endl;
  return 0;
}
