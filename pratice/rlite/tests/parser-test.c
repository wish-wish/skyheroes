#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "greatest.h"
#include "rlite/hirlite.h"

TEST test_format_noparam() {
	rliteClient client;
	if (rliteFormatCommand(&client, "ECHO helloworld") != 0) {
		FAIL();
	}
	ASSERT_EQ(client.argc, 2);
	ASSERT_EQ(client.argvlen[0], 4);
	ASSERT_EQ(memcmp(client.argv[0], "ECHO", 4), 0);
	ASSERT_EQ(client.argvlen[1], 10);
	ASSERT_EQ(memcmp(client.argv[1], "helloworld", 10), 0);

	int i;
	for (i = 0; i < client.argc; i++) {
		free(client.argv[i]);
	}
	free(client.argvlen);
	free(client.argv);
	PASS();
}

TEST test_format_sparam() {
	rliteClient client;
	if (rliteFormatCommand(&client, "ECHO %s", "helloworld") != 0) {
		FAIL();
	}
	ASSERT_EQ(client.argc, 2);
	ASSERT_EQ(client.argvlen[0], 4);
	ASSERT_EQ(memcmp(client.argv[0], "ECHO", 4), 0);
	ASSERT_EQ(client.argvlen[1], 10);
	ASSERT_EQ(memcmp(client.argv[1], "helloworld", 10), 0);
	int i;
	for (i = 0; i < client.argc; i++) {
		free(client.argv[i]);
	}
	free(client.argvlen);
	free(client.argv);
	return 0;
}

TEST test_format_bparam() {
	rliteClient client;
	// extra 1 to test that's not being read
	if (rliteFormatCommand(&client, "ECHO %b", "helloworld1", 10) != 0) {
		FAIL();
	}
	ASSERT_EQ(client.argc, 2);
	ASSERT_EQ(client.argvlen[0], 4);
	ASSERT_EQ(memcmp(client.argv[0], "ECHO", 4), 0);
	ASSERT_EQ(client.argvlen[1], 10);
	ASSERT_EQ(memcmp(client.argv[1], "helloworld", 10), 0);
	int i;
	for (i = 0; i < client.argc; i++) {
		free(client.argv[i]);
	}
	free(client.argvlen);
	free(client.argv);
	return 0;
}

TEST test_format_bdparam() {
	rliteClient client;
	// extra 1 to test that's not being read
	if (rliteFormatCommand(&client, "ECHO %b%d", "helloworld1", 10, 1) != 0) {
		FAIL();
	}
	ASSERT_EQ(client.argc, 2);
	ASSERT_EQ(client.argvlen[0], 4);
	ASSERT_EQ(memcmp(client.argv[0], "ECHO", 4), 0);
	ASSERT_EQ(client.argvlen[1], 11);
	ASSERT_EQ(memcmp(client.argv[1], "helloworld1", 11), 0);
	int i;
	for (i = 0; i < client.argc; i++) {
		free(client.argv[i]);
	}
	free(client.argvlen);
	free(client.argv);
	return 0;
}

TEST test_format_dparam() {
	rliteClient client;
	ASSERT_EQ(rliteFormatCommand(&client, "ECHO %d", 10), 0);
	ASSERT_EQ(client.argc, 2);
	ASSERT_EQ(client.argvlen[0], 4);
	ASSERT_EQ(memcmp(client.argv[0], "ECHO", 4), 0);
	ASSERT_EQ(client.argvlen[1], 2);
	ASSERT_EQ(memcmp(client.argv[1], "10", 2), 0);
	int i;
	for (i = 0; i < client.argc; i++) {
		free(client.argv[i]);
	}
	free(client.argvlen);
	free(client.argv);
	return 0;
}

TEST test_append_sparam() {
	rliteContext *context = rliteConnect(":memory:", 0);
	rliteReply *reply;
	void *_reply;
	ASSERT_EQ(rliteAppendCommand(context, "ECHO %s", "helloworld"), 0);
	ASSERT_EQ(rliteGetReply(context, &_reply), 0);
	reply = _reply;
	ASSERT_EQ(reply->type, RLITE_REPLY_STRING);
	ASSERT_EQ(memcmp(reply->str, "helloworld", 10), 0);

	rliteFreeReplyObject(reply);
	rliteFree(context);
	return 0;
}

TEST test_command_bparam() {
	rliteContext *context = rliteConnect(":memory:", 0);
	rliteReply *reply = rliteCommand(context, "ECHO %b", "helloworld", 10);
	ASSERT_EQ(reply->type, RLITE_REPLY_STRING);
	ASSERT_EQ(memcmp(reply->str, "helloworld", 10), 0);

	rliteFreeReplyObject(reply);
	rliteFree(context);
	return 0;
}

TEST test_command_bdparam() {
	rliteContext *context = rliteConnect(":memory:", 0);
	rliteReply *reply = rliteCommand(context, "ECHO %b%d", "helloworld", 10, 1);
	ASSERT_EQ(reply->type, RLITE_REPLY_STRING);
	ASSERT_EQ(memcmp(reply->str, "helloworld1", 11), 0);

	rliteFreeReplyObject(reply);
	rliteFree(context);
	return 0;
}

TEST test_command_lldparam() {
	rliteContext *context = rliteConnect(":memory:", 0);
	rliteReply *reply = rliteCommand(context, "ECHO %d", 123);
	ASSERT_EQ(reply->type, RLITE_REPLY_STRING);
	ASSERT_EQ(memcmp(reply->str, "123", 3), 0);

	rliteFreeReplyObject(reply);
	rliteFree(context);
	return 0;
}

TEST test_hset_hmget_dparam() {
	rliteContext *context = rliteConnect(":memory:", 0);
	rliteFreeReplyObject(rliteCommand(context, "HSET a f1 %d", 1));
	rliteReply* reply = rliteCommand(context, "HMGET a f1");
	ASSERT_EQ(reply->elements, 1);

	rliteFreeReplyObject(reply);
	rliteFree(context);
	return 0;
}

TEST test_zrange() {
	rliteContext *context = rliteConnect(":memory:", 0);
	rliteFreeReplyObject(rliteCommand(context, "ZRANGEBYLEX key + - LIMIT 0 30"));
	rliteFreeReplyObject(rliteCommand(context, "ZRANGEBYSCORE key -inf +inf"));
	rliteFree(context);
	return 0;
}

TEST test_invalid_command() {
	rliteContext *context = rliteConnect(":memory:", 0);
	rliteFreeReplyObject(rliteCommand(context, "ASD ayy ayy"));
	rliteFree(context);
	return 0;
}

TEST test_file_set_del_set() {
	unlink("user.db");
	rliteContext *context = rliteConnect("user.db", 0);
	rliteFreeReplyObject(rliteCommand(context, "SET a b"));
	rliteFreeReplyObject(rliteCommand(context, "DEL a"));
	rliteFreeReplyObject(rliteCommand(context, "SET a b"));
	rliteFree(context);
	return 0;
}

SUITE(parser_test)
{
	RUN_TEST(test_format_noparam);
	RUN_TEST(test_format_sparam);
	RUN_TEST(test_format_bparam);
	RUN_TEST(test_format_bdparam);
	RUN_TEST(test_append_sparam);
	RUN_TEST(test_command_bparam);
	RUN_TEST(test_command_bdparam);
	RUN_TEST(test_command_lldparam);
	RUN_TEST(test_hset_hmget_dparam);
	RUN_TEST(test_zrange);
	RUN_TEST(test_invalid_command);
	RUN_TEST(test_file_set_del_set);
}
