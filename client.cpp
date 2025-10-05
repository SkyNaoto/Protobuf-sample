#include "menu.pb.h"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// プロトタイプ宣言
void SendProtobuf(int sock, const google::protobuf::Message& message);

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
    item2->set_message_text("HVAC");
    item2->set_message_id("Message_2000");
    item2->set_chiled_id(2051);
    item2->set_data_id("LCAN_FF50");;

    // TCP送信
    // TCPソケット作成 AF_INET IPv4, SOCK_STREAM TCP, 0 プロトコルは自動選択
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // サーバーアドレス設定
    sockaddr_in serv_addr{};
    // IPv4
    serv_addr.sin_family = AF_INET;
    // ポート番号
    serv_addr.sin_port = htons(8080);
    // IPアドレス localhost
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    // サーバーに接続
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

     // 3改メッセージ送信
     for (int i = 0; i < 3; ++i) {
        // データ作成
        MenuTree menutree;
        menutree.set_language("Japanese");
        menutree.set_parent_id(1001);
        menutree.set_status(true);

        item* item = menutree.add_items();
        item->set_message_text("Gauge_"+ std::to_string(i));
        item->set_message_id("Message_"+ std::to_string(1000+i));
        item->set_chiled_id(1051+i);
        item->set_data_id("LCAN_FF40_" + std::to_string(i));
        // メッセージ送信
        SendProtobuf(sock, menutree);
        std::cout << "📤 MenuTree message " << i+1 << " sent" << std::endl;
        sleep(1); // 1秒待機
     }

     std::cout << " All MenuTree messages sent. Closing connetion." << std::endl;

    SendProtobuf(sock, menutree);

    close(sock);
    google::protobuf::ShutdownProtobufLibrary();
}

// TCPでprotobufメッセージを送信する関数
void SendProtobuf(int sock, const google::protobuf::Message& message) {
    //シリアライズ　バイナリー化
    std::string out;
    message.SerializeToString(&out);

    // htonl()は、ホストバイトオーダーからネットワークバイトオーダー（ビッグエンディアン）に変換する関数
    uint32_t msg_size = htonl(out.size());
    // メッセージサイズを先頭に送信し、続けてメッセージ本体を送信
    send(sock, &msg_size, sizeof(msg_size), 0);
    send(sock, out.data(), out.size(), 0);
    std::cout << "📤 MenuTree message sent (" << out.size() << " bytes)" << std::endl;
}