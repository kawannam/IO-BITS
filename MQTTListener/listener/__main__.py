from listener.MQTTClient import MQTTClient

if __name__ == "__main__":
    client = MQTTClient("localhost")
    client.listen()
