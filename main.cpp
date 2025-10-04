#include "menu.pb.h"
#include <iostream>
#include <fstream>

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // データ作成
    MenuTree menutree;
    menutree.set_language("Japanese");
    menutree.set_parent_id(1001);
    menutree.set_status(true);

    item* item1 = menutree.add_items();
    item1->set_message_text("Gauge");
    item1->set_message_id("Message_1000");
    item1->set_chiled_id(1051);
    item1->set_data_id("LCAN_FF40");

    item* item2 = menutree.add_items();
    item1->set_message_text("HVAC");
    item1->set_message_id("Message_2000");
    item1->set_chiled_id(2051);
    item1->set_data_id("LCAN_FF50");


    std::cout << "Parent ID: " << menutree.parent_id() << std::endl;
    for (const auto& item : menutree.items()) {
        std::cout << "  Item: " << item.message_text() << " (" << item.message_id() << ")" << std::endl;
    }

    // シリアライズ（バイナリ保存）
    {
        std::ofstream ofs("menu.bin", std::ios::binary);
        if (!menutree.SerializeToOstream(&ofs)) {
            std::cerr << "Failed to write menu." << std::endl;
            return 1;
        }
    }

    // デシリアライズ（読み込み）
    MenuTree menutree2;
    {
        std::ifstream ifs("menu.bin", std::ios::binary);
        if (!menutree2.ParseFromIstream(&ifs)) {
            std::cerr << "Failed to read menu." << std::endl;
            return 1;
        }
    }

    // 結果表示
    std::cout << "Parent ID: " << menutree2.parent_id() << std::endl;
    for (const auto& item : menutree2.items()) {
        std::cout << "  Text: " << item.message_text()
                  << " (messageID=" << item.message_id()
                  << ", chiled_id=" << item.chiled_id()
                  << ", data_id=" << item.data_id()
                  << ")" << std::endl;
    }

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
