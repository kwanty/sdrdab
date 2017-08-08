#include "AudioDecoder/player.h"
#include "AudioDecoder/fake_sink.h"
#include "AudioDecoder/file_src.h"
#include "AudioDecoder/ring_src.h"
#include "AudioDecoder/pulse_sink.h"
#include "AudioDecoder/ogg_sink.h"
#include "audio_decoder.h"
#include "gtest/gtest.h"
#include <fstream>
#include <string>

using namespace std;

struct Data {
    void *player;
    void *sink;
};

static void SaveTags(const std::map<const std::string, std::string> *tags_map, void *other_data) {
    map<const string, string> *other_tags_map = reinterpret_cast<map<const string, string> *>(other_data);
    *other_tags_map = *tags_map;
}

static gboolean RemoveSink(gpointer data) {
    Player *player = reinterpret_cast<Player *>(reinterpret_cast<Data *>(data)->player);
    PulseSink *sink = reinterpret_cast<PulseSink *>(reinterpret_cast<Data *>(data)->sink);
    player->RemoveSink(sink);

    return FALSE;
}

class PlayerTestAAC: public testing::Test {
    protected:

        PlayerTestAAC():
            src_(NULL),
            player_(NULL) {
            }

        virtual ~PlayerTestAAC() {
        }

        virtual void SetUp() {
            src_ = new FileSrc("./data/ut/player_unittest_file.aac");
            src_ = new FileSrc("./data/ut/player_unittest_file.aac");
            player_ = new Player(src_, PLAYER_AAC);
        }

        virtual void TearDown() {
            delete player_;
            delete src_;
        }

        FileSrc *src_;
        Player *player_;
};

class PlayerTestMPEG: public testing::Test {
    protected:

        PlayerTestMPEG():
            src_(NULL),
            player_(NULL) {
            }

        virtual ~PlayerTestMPEG() {
        }

        virtual void SetUp() {
            src_ = new FileSrc("./data/ut/player_unittest_file.mp2");
            player_ = new Player(src_, PLAYER_MPEG);
        }

        virtual void TearDown() {
            delete player_;
            delete src_;
        }

        FileSrc *src_;
        Player *player_;
};

class RingSrcTest: public testing::Test {
    protected:

        RingSrcTest():
            src_(NULL) {
            }

        virtual ~RingSrcTest() {
        }

        virtual void SetUp() {
            src_ = new RingSrc(0.2, 100);
        }

        virtual void TearDown() {
            delete src_;
        }

        RingSrc *src_;
};

TEST_F(PlayerTestAAC, number_of_processed_bytes) {
    remove("./data/ut/player_unittest_file.raw");
    int ret = system("gst-launch-1.0 filesrc location=./data/ut/player_unittest_file.aac \
            ! id3demux \
            ! aacparse \
            ! faad \
            ! audioconvert \
            ! pitch \
            ! filesink location=./data/ut/player_unittest_file.raw \
            1>/dev/null");
    ASSERT_TRUE( ret!=-1 ) <<"gst-launch-1.0 unavailable";

    uint32_t size;

    ifstream in_file("./data/ut/player_unittest_file.raw", ifstream::binary);
    ASSERT_TRUE(in_file.good()) << "TESTING CODE FAILED... could not load data";

    in_file.seekg(0, ios::end);
    size = in_file.tellg();
    in_file.close();

    FakeSink *sink = new FakeSink();
    player_->AddSink(sink);
    player_->Process();

    EXPECT_EQ(size, sink->bytes_returned());

    player_->RemoveSink(sink);
    delete sink;
}

TEST_F(PlayerTestAAC, save_to_ogg) {
    remove("./data/ut/player_unittest_file_expected_results.ogg");
    remove("./data/ut/player_unittest_file.ogg");
    int ret = system("gst-launch-1.0 filesrc location=./data/ut/player_unittest_file.aac \
            ! id3demux \
            ! aacparse \
            ! faad \
            ! audioconvert \
            ! pitch \
            ! vorbisenc \
            ! oggmux \
            ! filesink location=./data/ut/player_unittest_file_expected_results.ogg \
            1>/dev/null");

    ASSERT_TRUE( ret!=-1 ) <<"gst-launch-1.0 unavailable: ";


    uint32_t size_er;
    uint32_t size_r;

    OggSink *sink = new OggSink("./data/ut/player_unittest_file.ogg");
    player_->AddSink(sink);
    player_->Process();

    ifstream in_file_er("./data/ut/player_unittest_file_expected_results.ogg", ifstream::binary);
    ASSERT_TRUE(in_file_er.good()) << "TESTING CODE FAILED... could not load expected data";

    ifstream in_file_r("./data/ut/player_unittest_file.ogg", ifstream::binary);
    ASSERT_TRUE(in_file_r.good()) << "TESTING CODE FAILED... could not load data";

    in_file_er.seekg(0, ifstream::end);
    size_er = in_file_er.tellg();
    in_file_er.close();

    in_file_r.seekg(0, ifstream::end);
    size_r = in_file_r.tellg();
    in_file_r.close();

    EXPECT_EQ(size_er, size_r) << "TESTING CODE FAILED... file size mismatch";

    player_->RemoveSink(sink);
    delete sink;
}

TEST_F(PlayerTestAAC, type_aac) {
    EXPECT_EQ(player_->type(), PLAYER_AAC);
}

TEST_F(PlayerTestMPEG, type_mpeg) {
    EXPECT_EQ(player_->type(), PLAYER_MPEG);
}


TEST(PlayerTestPlayAAC, play_aac) {
    FileSrc *src = new FileSrc("./data/ut/player_unittest_file.aac");
    NullSink *n_sink = new NullSink();
    PulseSink *p_sink = new PulseSink();

    Player *player = new Player(src, PLAYER_AAC);

    Data data;
    data.player = player;
    data.sink = p_sink;

    player->AddSink(n_sink);
    player->AddSink(p_sink);
    g_timeout_add_seconds(10, RemoveSink, &data);

    player->Process();

    delete player;
    delete p_sink;
    delete n_sink;
    delete src;
}

TEST(PlayerTestPlayMPEG, play_mpeg) {
    FileSrc *src = new FileSrc("./data/ut/player_unittest_file.mp2");
    NullSink *n_sink = new NullSink();
    PulseSink *p_sink = new PulseSink();

    Player *player = new Player(src, PLAYER_MPEG);

    Data data;
    data.player = player;
    data.sink = p_sink;

    player->AddSink(n_sink);
    player->AddSink(p_sink);
    g_timeout_add_seconds(10, RemoveSink, &data);

    player->Process();

    delete player;
    delete p_sink;
    delete n_sink;
    delete src;
}

TEST(PlayerTestTags, tags_returned) {
    ifstream in_file("./data/ut/player_unittest_tags_expected.txt");
    ASSERT_TRUE(in_file.good()) << "TESTING CODE FAILED... could not load data";

    FileSrc *src = new FileSrc("./data/ut/player_unittest_file.aac");
    FakeSink *sink = new FakeSink();

    map<const string, string> temp;
    map<const string, string>::iterator it;

    Player *player = new Player(src, PLAYER_AAC);

    player->AddSink(sink);
    player->RegisterTagsMapCallback(SaveTags, &temp);
    player->Process();

    string line;

    while(getline(in_file, line)) {
        it = temp.find(line);
        ASSERT_TRUE(it != temp.end());
        getline(in_file, line);
        EXPECT_FALSE(line.compare(it->second));
    }

    delete player;
    delete sink;
    delete src;
}

TEST_F(PlayerTestAAC, src_getter) {
    EXPECT_EQ(src_, player_->abstract_src());
}

TEST_F(PlayerTestMPEG, src_getter) {
    EXPECT_EQ(src_, player_->abstract_src());
}

TEST(PlayerTestTee, no_src_pads) {
    FileSrc *src = new FileSrc(NULL);
    FakeSink *sink = new FakeSink();
    uint32_t result = 1;

    Player *player = new Player(src, -1);

    player->AddSink(sink);

    EXPECT_EQ(result, sink->num_src_pads());

    delete player;
    delete sink;
    delete src;
}

TEST(PlayerTestAddSink, incrementation_of_sink_pads) {
    FileSrc *src = new FileSrc(NULL);
    FakeSink *sink1 = new FakeSink();
    FakeSink *sink2 = new FakeSink();
    FakeSink *sink3 = new FakeSink();
    uint32_t result = 3;

    Player *player = new Player(src, -1);

    EXPECT_EQ(sink1, player->AddSink(sink1));
    EXPECT_EQ(sink2, player->AddSink(sink2));
    EXPECT_EQ(sink3, player->AddSink(sink3));

    EXPECT_EQ(result, sink1->num_src_pads());

    delete player;
    delete sink1;
    delete sink2;
    delete sink3;
    delete src;
}

TEST(PlayerTestRemoveSink, decrementation_of_sink_pads) {
    FileSrc *src = new FileSrc(NULL);
    FakeSink *sink1 = new FakeSink();
    FakeSink *sink2 = new FakeSink();
    FakeSink *sink3 = new FakeSink();
    uint32_t result = 2;

    Player *player = new Player(src, -1);

    player->AddSink(sink1);
    player->AddSink(sink2);
    player->AddSink(sink3);

    player->RemoveSink(sink3);

    EXPECT_EQ(result, sink1->num_src_pads());

    delete player;
    delete sink1;
    delete sink2;
    delete sink3;
    delete src;
}

TEST(PlayerTestMultipleAdd, multiple_addition_of_sink) {
    FileSrc *src = new FileSrc(NULL);
    PulseSink *p_sink = new PulseSink();
    AbstractSink *a_sink;

    Player *player = new Player(src, -1);

    a_sink = player->AddSink(p_sink);
    EXPECT_FALSE(a_sink == NULL);

    a_sink = player->AddSink(p_sink);
    EXPECT_TRUE(a_sink == NULL);

    delete player;
    delete p_sink;
    delete src;
}

TEST(PlayerTestAddRemoveAddAAC, sink_add_remove_add) {
    FileSrc *src = new FileSrc("./data/ut/player_unittest_file.aac");
    NullSink *n_sink = new NullSink();
    PulseSink *p_sink = new PulseSink();

    Player *player = new Player(src, PLAYER_AAC);

    Data data;
    data.player = player;
    data.sink = p_sink;

    player->AddSink(n_sink);
    player->AddSink(p_sink);
    player->RemoveSink(p_sink);
    player->AddSink(p_sink);

    g_timeout_add_seconds(3, RemoveSink, &data);

    player->Process();

    delete player;
    delete p_sink;
    delete n_sink;
    delete src;
}

TEST(PlayerTestAddRemoveAddMPEG, sink_add_remove_add) {
    FileSrc *src = new FileSrc("./data/ut/player_unittest_file.mp2");
    NullSink *n_sink = new NullSink();
    PulseSink *p_sink = new PulseSink();

    Player *player = new Player(src, PLAYER_MPEG);

    Data data;
    data.player = player;
    data.sink = p_sink;

    player->AddSink(n_sink);
    player->AddSink(p_sink);
    player->RemoveSink(p_sink);
    player->AddSink(p_sink);

    g_timeout_add_seconds(3, RemoveSink, &data);

    player->Process();

    delete player;
    delete p_sink;
    delete n_sink;
    delete src;
}

TEST(PlayerTestPlaybackSpeed, setting_playback_speed) {
    FileSrc *src = new FileSrc(NULL);
    FakeSink *sink = new FakeSink();

    Player *player = new Player(src, -1);

    player->AddSink(sink);
    player->set_playback_speed(0.9);

    EXPECT_NEAR(0.9, sink->playback_speed(), 1e-3);

    delete player;
    delete sink;
    delete src;
}

TEST(SinkTestLinkage, test_of_linkage) {
    FileSrc *src = new FileSrc(NULL);
    FakeSink *sink = new FakeSink();

    EXPECT_FALSE(sink->linked());

    Player *player = new Player(src, -1);

    player->AddSink(sink);

    EXPECT_TRUE(sink->linked());

    player->RemoveSink(sink);

    EXPECT_FALSE(sink->linked());

    delete player;
    delete sink;
    delete src;
}

TEST(RingSrcTestDecrementRatio, decrementation_of_ratio) {
    RingSrc *src = new RingSrc(0.2, 100);
    FakeSink *sink = new FakeSink();

    Player *player = new Player(src, -1);

    player->AddSink(sink);
    src->DecrementRatio();

    EXPECT_LT(sink->playback_speed(), 1.0);

    delete player;
    delete sink;
    delete src;
}

TEST(RingSrcTestIncrementRatio, incrementation_of_ratio) {
    RingSrc *src = new RingSrc(0.2, 100);
    FakeSink *sink = new FakeSink();

    Player *player = new Player(src, -1);

    player->AddSink(sink);
    src->IncrementRatio();

    EXPECT_GT(sink->playback_speed(), 1.0);

    delete player;
    delete sink;
    delete src;
}

TEST_F(RingSrcTest, last_frame) {
    EXPECT_FALSE(src_->last_frame());
    src_->set_last_frame(true);
    EXPECT_TRUE(src_->last_frame());
}

TEST_F(RingSrcTest, flush) {
    EXPECT_FALSE(src_->flush());
    src_->set_flush(true);
    EXPECT_TRUE(src_->flush());
}

TEST_F(RingSrcTest, write_into) {
    int result = 0;
    int length = 100;
    uint8_t t[100] = {};

    EXPECT_EQ(src_->ring_buffer()->DataStored(), result);
    result += src_->Write(t, length);
    EXPECT_EQ(src_->ring_buffer()->DataStored(), result);
}

