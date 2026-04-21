#pragma once
#include "Singleton.h"
#include "const.h"

class RedisConPool {
public:
	RedisConPool(size_t poolSize, const char* host, int port, const char* pwd)
		: poolSize_(poolSize), host_(host), port_(port), b_stop_(false) {
		for (size_t i = 0; i < poolSize_; ++i) {
			auto* context = redisConnect(host, port);
			if (context == nullptr || context->err != 0) {
				if (context != nullptr) {
					redisFree(context);
				}
				continue;
			}

			auto reply = (redisReply*)redisCommand(context, "AUTH %s", pwd);
			if (reply->type == REDIS_REPLY_ERROR) {
				std::cout << "认证失败" << std::endl;
				//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
				freeReplyObject(reply);
				redisFree(context);
				continue;
			}

			//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
			freeReplyObject(reply);
			std::cout << "认证成功" << std::endl;
			connections_.push(context);
		}

		check_thread_ = std::thread([this]() {
			while (!b_stop_) {
				counter_++;
				if (counter_ >= 60) {
					checkThreadPro();
					counter_ = 0;
				}

				std::this_thread::sleep_for(std::chrono::seconds(1)); // 每隔 30 秒发送一次 PING 命令
			}
			});

	}

	~RedisConPool() {
		std::lock_guard<std::mutex> lock(mutex_);
		while (!connections_.empty()) {
			connections_.pop();
		}
	}

	redisContext* getConnection() {
		std::unique_lock<std::mutex> lock(mutex_);
		cond_.wait(lock, [this] {
			if (b_stop_) {
				return true;
			}
			return !connections_.empty();
			});
		//如果停止则直接返回空指针
		if (b_stop_) {
			return  nullptr;
		}
		auto* context = connections_.front();
		connections_.pop();
		return context;
	}

	redisContext* getConNonBlock() {
		std::unique_lock<std::mutex> lock(mutex_);
		if (b_stop_) {
			return nullptr;
		}

		if (connections_.empty()) {
			return nullptr;
		}

		auto* context = connections_.front();
		connections_.pop();
		return context;
	}

	void returnConnection(redisContext* context) {
		std::lock_guard<std::mutex> lock(mutex_);
		if (b_stop_) {
			return;
		}
		connections_.push(context);
		cond_.notify_one();
	}

	void Close() {
		b_stop_ = true;
		cond_.notify_all();
	}

private:
	bool  reconnect() {
		auto context = redisConnect("127.0.0.1", 6379);
		if (context == nullptr || context->err != 0) {
			if (context != nullptr) {
				redisFree(context);
			}
			return false;
		}

		auto reply = (redisReply*)redisCommand(context, "AUTH %s", "123456");
		if (reply->type == REDIS_REPLY_ERROR) {
			std::cout << "认证失败" << std::endl;
			//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
			freeReplyObject(reply);
			redisFree(context);
			return false;
		}

		//执行成功 释放redisCommand执行后返回的redisReply所占用的内存
		freeReplyObject(reply);
		std::cout << "认证成功" << std::endl;
		returnConnection(context);
		return true;
	}

	void checkThreadPro() {
		size_t pool_size;
		{
			// 先拿到当前连接数
			std::lock_guard<std::mutex> lock(mutex_);
			pool_size = connections_.size();
		}


		for (int i = 0; i < pool_size && !b_stop_; ++i) {
			redisContext* ctx = nullptr;
			// 1) 取出一个连接(持有锁)
			bool bsuccess = false;
			auto* context = getConNonBlock();
			if (context == nullptr) {
				break;
			}

			redisReply* reply = nullptr;
			try {
				reply = (redisReply*)redisCommand(context, "PING");
				// 2. 先看底层 I/O／协议层有没有错
				if (context->err) {
					std::cout << "Connection error: " << context->err << std::endl;
					if (reply) {
						freeReplyObject(reply);
					}
					redisFree(context);
					fail_count_++;
					continue;
				}

				// 3. 再看 Redis 自身返回的是不是 ERROR
				if (!reply || reply->type == REDIS_REPLY_ERROR) {
					std::cout << "reply is null, redis ping failed: " << std::endl;
					if (reply) {
						freeReplyObject(reply);
					}
					redisFree(context);
					fail_count_++;
					continue;
				}
				// 4. 如果都没问题，则还回去
				//std::cout << "connection alive" << std::endl;
				freeReplyObject(reply);
				returnConnection(context);
			}
			catch (std::exception& exp) {
				if (reply) {
					freeReplyObject(reply);
				}

				redisFree(context);
				fail_count_++;
			}

		}

		//执行重连操作
		while (fail_count_ > 0) {
			auto res = reconnect();
			if (res) {
				fail_count_--;
			}
			else {
				//留给下次再重试
				break;
			}
		}
	}

	std::atomic<bool> b_stop_;
	size_t poolSize_;
	const char* host_;
	const char* pwd_;
	int port_;
	std::queue<redisContext*> connections_;
	std::mutex mutex_;
	std::atomic<int> fail_count_;
	std::condition_variable cond_;

	std::thread  check_thread_;
	int counter_;
};

class RedisMgr: public Singleton<RedisMgr>
{
	friend class Singleton<RedisMgr>;
public:
	~RedisMgr();
	bool Get(const std::string& key, std::string& value);
	bool Set(const std::string& key, const std::string& value);
	bool Auth(const std::string& password);
	bool LPush(const std::string& key, const std::string& value);
	bool LPop(const std::string& key, std::string& value);
	bool RPush(const std::string& key, const std::string& value);
	bool RPop(const std::string& key, std::string& value);
	bool HSet(const std::string& key, const std::string& hkey, const std::string& value);
	bool HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen);
	std::string HGet(const std::string& key, const std::string& hkey);
	bool HDel(const std::string& key, const std::string& field);
	bool Del(const std::string& key);
	bool ExistsKey(const std::string& key);
	void Close();
private:
	RedisMgr();

	std::unique_ptr<RedisConPool> _con_pool;
};

