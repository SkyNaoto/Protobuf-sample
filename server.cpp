#include "menu.pb.h"
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <google/protobuf/message.h>

bool ReceiveProtobuf(int client_socket, google::protobuf::Message& message) ;

// ------------------------
// メインサーバー処理
// ------------------------
int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // TCPサーバー設定
    // TCPソケット作成 AF_INET IPv4, SOCK_STREAM TCP, 0 プロトコルは自動選択
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // サーバーアドレス設定 アドレスとはIPアドレスとポート番号の組み合わせ
    sockaddr_in address{};
    // IPv4
    address.sin_family = AF_INET;
    // すべてのアドレスからの接続を受け入れる
    address.sin_addr.s_addr = INADDR_ANY;
    // ポート番号
    address.sin_port = htons(8080);

    // ソケットにアドレスをバインドし、接続待ち状態にする seizeof(address)はアドレス構造体のサイズ
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    // 接続待ち状態にする 最大1つの接続を待機
    listen(server_fd, 1);

    std::cout << "📡 Server listening on port 8080..." << std::endl;

    // クライアント接続待ち addressは接続してきたクライアントのアドレス情報が入る
    int addrlen = sizeof(address);
    // クライアントからの接続を受け入れる
    int client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);

    while(true){
        MenuTree menutree;
        // menutreeにデータを受信
        if (!ReceiveProtobuf(client_socket, menutree)) break;
            
        std::cout << "✅ Message received successfully!" << std::endl;
        std::cout << "Language: " << menutree.language() << std::endl;
        std::cout << "Parent ID: " << menutree.parent_id() << std::endl;
        std::cout << "Status: " << (menutree.status() ? "true" : "false") << std::endl;

        for (const auto& i : menutree.items()) {
            std::cout << "  " << i.message_text()
                    << " (" << i.message_id()
                    << ", " << i.chiled_id()
                    << ", " << i.data_id()
                    << ")" << std::endl;
        }
        std::cout << "-----------------------------" << std::endl;
    
    }
    std::cout << "❌ Client disconnected." << std::endl;

    close(client_socket);
    close(server_fd);
    google::protobuf::ShutdownProtobufLibrary();
}


// 汎用protobuf受信関数
// client_socket: クライアントソケット message: 受信したprotobufメッセージを格納するオブジェクト
bool ReceiveProtobuf(int client_socket, google::protobuf::Message& message) {
    // メッセージサイズ受信 まず4バイトのサイズ情報を受信 その後データ本体を受信　ここではまず4バイトのサイズ情報を受信
    uint32_t msg_size;
    // 4バイトのメッセージサイズを先頭から受信 readはソケットからデータを読み込む関数 msg_sizeに読み込んだデータを格納
    int n = read(client_socket, &msg_size, sizeof(msg_size));
    // 0バイト以下ならエラー
    if (n <= 0) return false;
    // 4バイト読み込めなかった場合はエラー
    if (n != sizeof(msg_size)) {
        std::cerr << "Failed to read message size." << std::endl;
        return false;
    }
    // ネットワークバイトオーダーからホストバイトオーダーに変換
    msg_size = ntohl(msg_size);

    // メッセージ本体を受信
    // msg_sizeバイト分のバッファを確保し、そこにデータを読み込む
    std::string buffer(msg_size, '\0');
    // readでmsg_sizeバイト分のデータを読み込み、bufferに格納
    int bytes_read = read(client_socket, buffer.data(), msg_size);
    // 期待したバイト数を読み込めなかった場合はエラー
    if (bytes_read != msg_size) {
        std::cerr << "Failed to read full message body." << std::endl;
        return false;
    }

    if (!message.ParseFromString(buffer)) {
        std::cerr << "Failed to parse protobuf message." << std::endl;
        return false;
    }

    return true;
}
