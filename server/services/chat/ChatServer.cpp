#include "LogicSystem.h"
#include <csignal>
#include <thread>
#include <mutex>
#include "AsioIOServicePool.h"
#include "CServer.h"
#include "ConfigMgr.h"
#include <iostream>
#include "RedisMgr.h"
#include "ChatServiceImpl.h"

bool bstop = false;
std::condition_variable cond_quit;
std::mutex mutex_quit;

int main(int argc, char* argv[])
{
	auto& cfg = ConfigMgr::Inst();

	for (int i = 1; i < argc; ++i) {
		std::string arg = argv[i];
		if ((arg == "--config" || arg == "-c") && i + 1 < argc) {
			cfg.LoadFromFile(argv[++i]);
		}
	}

	auto server_name = cfg["SelfServer"]["Name"];
	try {
		auto pool = AsioIOServicePool::GetInstance();
		RedisMgr::GetInstance()->HSet(LOGIN_COUNT, server_name, "0");
		Defer defer([server_name]() {
			RedisMgr::GetInstance()->HDel(LOGIN_COUNT, server_name);
			RedisMgr::GetInstance()->Close();
			});

		boost::asio::io_context  io_context;
		auto port_str = cfg["SelfServer"]["Port"];
		auto pointer_server = std::make_shared<CServer>(io_context, atoi(port_str.c_str()));
		pointer_server->StartTimer();

		std::string server_address(cfg["SelfServer"]["Host"] + ":" + cfg["SelfServer"]["RPCPort"]);
		ChatServiceImpl service;
		grpc::ServerBuilder builder;
		builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
		builder.RegisterService(&service);
		service.RegisterServer(pointer_server);
		std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
		std::cout << "RPC Server listening on " << server_address << std::endl;

		std::thread  grpc_server_thread([&server]() {
			server->Wait();
			});

		boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
		signals.async_wait([&io_context, pool, &server](auto, auto) {
			io_context.stop();
			pool->Stop();
			server->Shutdown();
			});

		LogicSystem::GetInstance()->SetServer(pointer_server);
		io_context.run();

		grpc_server_thread.join();
		pointer_server->StopTimer();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
}
