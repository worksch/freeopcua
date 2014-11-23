/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Remote Computer implementaion.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///



#include <opc/ua/client/client.h>
#include <opc/ua/node.h>
#include <opc/ua/subscription.h>

#include <iostream>
#include <stdexcept>
#include <thread>

using namespace OpcUa;

class SubClient : public SubscriptionClient
{
  void DataChange(uint32_t handle, const Node& node, const Variant& val, AttributeID attr) const override
  {
    std::cout << "Received DataChange event, value of Node " << node << " is now: "  << "(Print of arbitrary variant not suported yet)"<< std::endl;
  }
};

int main(int argc, char** argv)
{
  try
  {
    //std::string endpoint = "opc.tcp://192.168.56.101:48030";
    std::string endpoint = "opc.tcp://127.0.0.1:4841";
    //std::string endpoint = "opc.tcp://localhost:53530/OPCUA/SimulationServer/";

    std::cout << "Connecting to: " << endpoint << std::endl;
    bool debug = false;
    OpcUa::RemoteClient client(endpoint, debug);
    client.Connect();

    std::cout << "Getting root node: " << endpoint << std::endl;
    OpcUa::Node root = client.GetRootNode();
    std::cout << "Root node is: " << root << std::endl;
    std::vector<std::string> path({"Objects", "Server"});
    OpcUa::Node server = root.GetChild(path);
    std::cout << "Server node obtained by path: " << server << std::endl;

    std::cout << "Child of objects node are: " << std::endl;
    for (OpcUa::Node node : client.GetObjectsNode().GetChildren())
      std::cout << "    " << node << std::endl;


    std::cout << "NamespaceArray is: " << std::endl;
    std::vector<std::string> nspath ({"Objects", "Server", "NamespaceArray"});
    OpcUa::Node nsnode = root.GetChild(nspath);
    OpcUa::Variant ns  = nsnode.GetValue();

    for (std::string d : ns.As<std::vector<std::string>>())
      std::cout << "    "  << d << std::endl;


    //Subscription
    std::vector<std::string> varpath({"Objects", "2:NewObject", "2:MyVariable"});
    //std::vector<std::string> varpath({"Objects", "5:Simulation", "5:Random1"}); //Example data from prosys server
    OpcUa::Node myvar = root.GetChild(varpath);

    std::cout << "got node: " << myvar << std::endl;
    SubClient sclt; 
    std::unique_ptr<Subscription> sub = client.CreateSubscription(100, sclt);
    uint32_t handle = sub->SubscribeDataChange(myvar);
    std::cout << "Got sub handle: " << handle << ", sleeping 5 sconds" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::cout << "Disconnecting" << std::endl;
    client.Disconnect();
    return 0;
  }
  catch (const std::exception& exc)
  {
    std::cout << exc.what() << std::endl;
  }
  catch (...)
  {
    std::cout << "Unknown error." << std::endl;
  }
  return -1;
}

