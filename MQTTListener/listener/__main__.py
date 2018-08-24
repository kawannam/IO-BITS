from listener.MQTTClient import MQTTClient

if __name__ == "__main__":
    client = MQTTClient("test.mosquitto.org")
    client.listen()
