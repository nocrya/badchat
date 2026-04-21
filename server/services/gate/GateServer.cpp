#include "ConfigMgr.h"
#include "CServer.h"
#include "const.h"
#include "RedisMgr.h"

int main(int argc, char* argv[])
{
    auto& cfg = ConfigMgr::Inst();

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "--config" || arg == "-c") && i + 1 < argc) {
            cfg.LoadFromFile(argv[++i]);
        }
    }

    std::string gate_port_str = cfg["GateServer"]["Port"];
    unsigned short gate_port = atoi(gate_port_str.c_str());

    try {
        net::io_context ioc{ 1 };
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
            if (error) {
                return;
            }
            ioc.stop();
            });

        std::make_shared<CServer>(ioc, gate_port)->Start();
        std::cout << "Gate Server listen on port: " << gate_port << std::endl;
        ioc.run();
    }
    catch (std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
