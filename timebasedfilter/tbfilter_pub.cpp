#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <gen/tbFilter_DCPS.hpp>

int main (int argc, char *argv[]) {
  // Join or create the default domain
  dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

  // Create a topic with QoS profile: durability (transient) and reliability (reliable)
  dds::topic::qos::TopicQos TopicQos = dp.default_topic_qos()
                        << dds::core::policy::Durability::Transient()
                        << dds::core::policy::Reliability::Reliable();
  
  // Create a topic named "ContentFilter_Msg" with the given Qos profile
  dds::topic::Topic<TbasedFilter::Msg> topic(dp, "TbasedFilter_Msg", TopicQos);

  // Create publisher with the given QoS
  dds::pub::qos::PublisherQos pubQos = dp.default_publisher_qos();

  dds::pub::Publisher pub(dp, pubQos);
  
  dds::pub::qos::DataWriterQos dwQos = topic.qos();
  dwQos << dds::core::policy::WriterDataLifecycle::ManuallyDisposeUnregisteredInstances();

  dds::pub::DataWriter<TbasedFilter::Msg> dw(pub, topic, dwQos);

  std::random_device rd;
  std::default_random_engine gen = std::default_random_engine(rd());
  std::uniform_real_distribution<float> dis(1.0,100.0);
  int count=0;
  for (int i,j = 0; i < 100; i++) {
	j = i % 5;
	if (j > 1){
		TbasedFilter::Msg msgInstance(j, 100 + j);
		dw << msgInstance;
		std::cout << "Writing a message containing..." 
	 		  << "SensorID: " << msgInstance.sensorID() 
			  << ", Value: " << msgInstance.value() << std::endl;
		count++;
	}
	// Should wait for a monents in case the network are not discovered yet.
	//std::this_thread::sleep_for(std::chrono::seconds(2));
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  std::cout << "total publish: "<< count << std::endl;
 
  // Should wait for a monents in case the network are not discovered yet.
  std::this_thread::sleep_for(std::chrono::seconds(2));

  return 0;
}
