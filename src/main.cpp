#include <Arduino.h> // PIO cần dòng này
#include <WiFi.h>
#include <PubSubClient.h> // Thư viện MQTT

// --- CẤU HÌNH CỦA BẠN ---
const char *ssid = "sondhg_shared";            // Tên Wi-Fi Hotspot của bạn
const char *password = "12345678";             // Mật khẩu Wi-Fi
const char *mqtt_server = "broker.hivemq.com"; // Máy chủ MQTT
// --- HẾT CẤU HÌNH ---

WiFiClient espClient;
PubSubClient client(espClient);

// Hàm này chạy 1 lần khi ESP32 khởi động
void setup()
{
  Serial.begin(115200); // Mở cổng Serial để theo dõi log
  Serial.println();
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);

  // Bắt đầu kết nối Wi-Fi
  WiFi.begin(ssid, password);

  // Chờ đến khi kết nối thành công
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Cấu hình máy chủ MQTT
  client.setServer(mqtt_server, 1883);
}

// Hàm này kết nối lại MQTT nếu bị mất kết nối
void reconnect()
{
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Tạo một Client ID ngẫu nhiên
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    // Thử kết nối
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Gửi (Publish) 1 tin nhắn ngay khi kết nối
      client.publish("iot/project/test", "Hello from ESP32 (via PIO)");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Chờ 5 giây trước khi thử lại
      delay(5000);
    }
  }
}

// --- Bắt đầu phần code mới ---
// Biến này để theo dõi thời gian
long lastMsg = 0;
// --- Hết phần code mới ---

// Hàm này lặp đi lặp lại mãi mãi
void loop()
{
  if (!client.connected())
  {
    reconnect(); // Nếu mất kết nối, kết nối lại
  }
  client.loop(); // Duy trì kết nối MQTT

  // --- Bắt đầu phần code mới ---
  // Lấy thời gian hiện tại
  long now = millis();

  // Cứ mỗi 5 giây (5000 mili giây) thì gửi 1 tin
  if (now - lastMsg > 5000)
  {
    lastMsg = now;

    // Tạo nội dung tin nhắn, ví dụ "Message so 1", "Message so 2"...
    static int count = 0;
    count++;
    String payload = "Message so " + String(count);

    // Gửi (Publish) tin nhắn
    client.publish("iot/project/test", payload.c_str());

    Serial.print("Published message: ");
    Serial.println(payload);
  }
  // --- Hết phần code mới ---
}