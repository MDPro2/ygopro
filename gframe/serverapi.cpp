#include "serverapi.h"
#include "game.h"
#include "netserver.h"
#include "network.h"
#include "config.h"
#include "data_manager.h"
#include "gframe.h"
#include <event2/thread.h>
#include <memory>

namespace ygo {
	YGOSERVER_API int start_server(const char* args) {
		std::vector<char*> argv_vec;

		const char* server_name = "ygoserver";
		char* arg0 = new char[strlen(server_name) + 1];
		strcpy(arg0, server_name);
		argv_vec.push_back(arg0);

		if (args != nullptr) {
			const char* p = args;
			while (*p != '\0') {
				while (*p == ' ') p++;
				if (*p == '\0') break;

				std::string token;
				bool in_quotes = false;

				while (*p != '\0') {
					if (*p == '"') {
						in_quotes = !in_quotes;
					} 
					else if (*p == ' ' && !in_quotes) {
						break;
					} 
					else {
						token += *p;
					}
					p++;
				}

				char* currentToken = new char[token.length() + 1];
				strcpy(currentToken, token.c_str());
				argv_vec.push_back(currentToken);
			}
		}

		int argc = static_cast<int>(argv_vec.size());
		int result = main(argc, argv_vec.data());

		for (char* arg : argv_vec) {
			delete[] arg;
		}

		return result;
	}
	YGOSERVER_API void stop_server() {
		NetServer::StopServer();
	}
}
