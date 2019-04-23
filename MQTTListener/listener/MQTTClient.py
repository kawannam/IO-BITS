import paho.mqtt.client as mqtt
import paho.mqtt.subscribe as subscribe
from listener.EventHandler import EventHandler


class MQTTClient:
    PRESS = "iobits/ButtonPress"
    REQUEST = "iobits/DataRequest"

    def __init__(self, host_name):
        self.server = host_name
        self.client = mqtt.Client()
        print("Connecting to {}...".format(host_name))
        self.client.connect(host_name)
        print("Connected to {}!".format(host_name))
        self.handler = EventHandler()

    def listen(self):
        subscribe.callback(self.on_message, [MQTTClient.PRESS, MQTTClient.REQUEST], hostname=self.server)

    def on_message(self, cdata, userdata, msg):
        if msg.topic == MQTTClient.PRESS:
            self.handler.button_press_event(msg.payload.decode("utf-8"))
        elif msg.topic == MQTTClient.REQUEST:
            self.handler.data_request_event(msg.payload.decode("utf-8"), self.server)
        else:
            print("UNKNOWN MESSAGE")
