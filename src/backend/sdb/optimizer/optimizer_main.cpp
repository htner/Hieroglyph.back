// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

// A server to receive HelloRequest and send back HelloReply

#include "backend/sdb/common/pg_export.hpp"

#include <chrono>
#include <gflags/gflags.h>
#include <brpc/server.h>
#include <brpc/restful.h>

#include "include/sdb/optimizer_main.h"
#include "include/sdb/postgres_init.h"

#include "backend/sdb/common/singleton.hpp"
#include "backend/sdb/common/shared_queue.hpp"
#include "backend/sdb/optimizer/optimize_task.hpp"
#include "backend/sdb/optimizer/optimizer_service.hpp"
#include "backend/sdb/common/s3_context.hpp"

#include "backend/sdb/common/common.hpp"
#include "backend/sdb/common/flags.hpp"

#include <butil/logging.h> // LOG Last
#include <thread>

int OptimizerServerRun(int argc, char** argv);

int OptimizerServiceMain(int argc, char* argv[]) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	create_session_context(TopMemoryContext);
	//
	kNotInitdb = true;
	MyDatabaseId = FLAGS_dbid;
	MyDatabaseTableSpace = 1;
	Gp_role = GP_ROLE_DISPATCH;

	/*
	kDBBucket = FLAGS_bucket;
	kDBS3User = FLAGS_s3user;
	kDBS3Password = FLAGS_s3passwd;
	kDBS3Region = FLAGS_region;
	kDBS3Endpoint = FLAGS_endpoint;
	kDBIsMinio = FLAGS_isminio;
	*/

	auto s3_cxt = GetS3Context();
    auto& sess_info = thr_sess->session_cxt_;

	s3_cxt->lake_bucket_ = FLAGS_bucket;
	s3_cxt->lake_user_ = FLAGS_s3user;
	s3_cxt->lake_password_ = FLAGS_s3passwd;
	s3_cxt->lake_region_ = FLAGS_region;
	s3_cxt->lake_endpoint_ = FLAGS_endpoint;
	s3_cxt->lake_isminio_ = FLAGS_isminio;

	sess_info.dbid_ = FLAGS_dbid;
	sess_info.sessionid_ = 1;
	sess_info.query_id_ = 1;

	sdb::CatalogInfo catalog_info;

    //InitMinimizePostgresEnv(argc, argv, "template1", "template1");
	InitMinimizePostgresEnv(argv[0], FLAGS_dir.data(), FLAGS_database.data(), "root");

	Gp_role = GP_ROLE_DISPATCH;
	std::thread pg_thread(OptimizerServerRun, argc, argv);

	while (true) {
		auto task = sdb::TaskQueueSingleton::GetInstance()->pop_front(); 
		task->Run(catalog_info);
	}

	pg_thread.join();	
}

int OptimizerServerRun(int argc, char** argv) {
	// Parse gflags. We recommend you to use gflags as well.
	FLAGS_reuse_addr = true;
	FLAGS_reuse_port = true;
	// Generally you only need one Server.
	//
	brpc::Server server;

	sdb::OptimizerService http_svc;

	// Add services into server. Notice the second parameter, because the
	// service is put on stack, we don't want server to delete it, otherwise
	// use brpc::SERVER_OWNS_SERVICE.
	for (int i = 0; i < FLAGS_try_num; ++i) {
		if (server.AddService(&http_svc,
						   brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
			LOG(ERROR) << "Fail to add http_svc";
			std::this_thread::sleep_for(std::chrono::seconds(5));
			continue;
		} else {
			break;
		}
	}

	// FLAGS_port = PostPortNumber;

	// Start the server.
	brpc::ServerOptions options;
	options.idle_timeout_sec = FLAGS_idle_timeout_s;
	for (int i = 0; i < FLAGS_try_num; ++i) {
		int32 port = FLAGS_port;
		if (server.Start(port, &options) != 0) {
			LOG(ERROR) << "Fail to start HttpServer, port" << port;
			std::this_thread::sleep_for(std::chrono::seconds(5));
			continue;
		} else {
			break;
		}
	}

	// Wait until Ctrl-C is pressed, then Stop() and Join() the server.
	server.RunUntilAskedToQuit();
	return 0;
}
